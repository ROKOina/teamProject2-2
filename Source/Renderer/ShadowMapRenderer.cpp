#include "ShadowMapRenderer.h"

#include "Camera.h"
#include "Graphics/Graphics.h"
#include "CameraController.h"
#include "Graphics/LightManager.h"
#include "Graphics/Shader.h"
ShadowMapRenderer::ShadowMapRenderer(UINT shadowmapSize)
{
	for(auto& depthStencil:shadowmapDepthStencil)
	{
		depthStencil.release();
		depthStencil=std::make_unique<DepthStencil>(shadowmapSize, shadowmapSize);
	}
}

ShadowMapRenderer::~ShadowMapRenderer()
{
}

ShadowmapData ShadowMapRenderer::GetShadowMapData()
{
	ShadowmapData shadowMapData;
	DirectX::XMFLOAT4X4 view, projection;

	//ビュー行列とプロジェクション行列を算出
	CalcShadowmapMatrix(view, projection);

	//ビュープロジェクション行列を算出
	//CalcLightviewProjectionMatrix(view, projection);

	for(int i=0;i<ShadowmapCount;++i)
	{
		shadowMapData.shadowMap[i] = shadowmapDepthStencil[i]->GetShaderResourceView().Get();
		//shadowMapData.lightViewProjection[i] = lightViewProjection[i];
		shadowMapData.shadowBias[i] = shadowBias[i];
	}
	//shadowMapData.lightViewProjection = lightViewProjection[0];
	DirectX::XMStoreFloat4x4(&shadowMapData.lightViewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	shadowMapData.shadowColor = shadowColor;
	return shadowMapData;
}

//void ShadowMapRenderer::Render(ID3D11DeviceContext* dc,RenderContext& rc)
void ShadowMapRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// 画面クリア＆レンダーターゲット設定
	ID3D11DepthStencilView* dsv = shadowmapDepthStencil[0]->GetDepthStencilView().Get();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(0, nullptr, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(shadowmapDepthStencil[0]->GetWidth());
	vp.Height = static_cast<float>(shadowmapDepthStencil[0]->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	RenderContext rc;

	//シャドウマップに使う行列を計算
	CalcShadowmapMatrix(rc.view, rc.projection);

	DirectX::XMStoreFloat4x4(&lightViewProjection[0], DirectX::XMLoadFloat4x4(&rc.view) * DirectX::XMLoadFloat4x4(&rc.projection));

	//3Dモデル描画
	{
		ModelShader* shader = graphics.GetModelShader(ModelShaderId::ShadowmapCaster);
		shader->Begin(dc, rc);

		for (Model* model : renderModels)
		{
			shader->Draw(dc, rc, model);
		}

		shader->End(dc, rc);

	}

	//for(int i=0;i<ShadowmapCount;++i)
	//{
	//	//シャドウマップにレンダリング
	//	ID3D11RenderTargetView* rtv = nullptr;
	//	ID3D11DepthStencilView* dsv = shadowmapDepthStencil[i]->GetDepthStencilView().Get();

	//	//画面クリア
	//	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//	//レンダーターゲット設定
	//	dc->OMSetRenderTargets(0, &rtv, dsv);

	//	// ビューポートの設定
	//	D3D11_VIEWPORT	vp = {};
	//	vp.Width = static_cast<float>(shadowmapDepthStencil[i]->GetWidth());
	//	vp.Height = static_cast<float>(shadowmapDepthStencil[i]->GetHeight());
	//	vp.MinDepth = 0.0f;
	//	vp.MaxDepth = 1.0f;
	//	dc->RSSetViewports(1, &vp);


	//	//レンダーコンテキストに情報を設定
	//	rc.shadowmapData = GetShadowMapData();
	//	rc.shadowmapData.lightViewProjection = lightViewProjection[i];

	//	//3Dモデル描画
	//	{
	//		ModelShader* shader = graphics.GetModelShader(ModelShaderId::ShadowmapCaster);
	//		shader->Begin(dc,rc);

	//		for (Model* model : renderModels)
	//		{
	//			shader->Draw(dc,rc, model);
	//		}

	//		shader->End(dc,rc);

	//	}

	//}
	//元のバッファに戻す
	RestoreRenderTargets(dc);
}

void ShadowMapRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("Shadowmap Rendering"))
	{
		ImGui::SliderFloat("Rectangle", &shadowRect, 1.0f, 2048.0f);
		ImGui::ColorEdit3("Color", &shadowColor.x);
		for(int i=0;i<ShadowmapCount;++i)
		{
			ImGui::SliderFloat("Bias", &shadowBias[i], 0.0f, 0.1f, "%0.4f");
			if (ImGui::TreeNode("RenderTargets"))
			{
				ImGui::Text("Shadowmap");
				ImGui::Image(shadowmapDepthStencil[i]->GetShaderResourceView().Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}

void ShadowMapRenderer::CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection)
{
	Light* mainDirectionalLight = LightManager::Instance().GetShadowmapLight();
	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
	if (!mainDirectionalLight)
	{
		view       = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		projection = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
		return;
	}


	//平行高原からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
	DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&mainDirectionalLight->GetDirection());
	LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(LightPosition,
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixOrthographicLH(15, 15, Near, Far);

	//シャドウマップに描画したい範囲の射影行列を生成
	DirectX::XMStoreFloat4x4(&view, viewMatrix);
	DirectX::XMStoreFloat4x4(&projection, projectionMatrix);
}

void ShadowMapRenderer::CalcLightviewProjectionMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection)
{
	//シャドウマップの分割エリア定義
	float splitAreaTable[] =
	{
		Camera::Instance().GetNearZ(),
		250.0f,
		500.0f,
		1000.0f,
		Camera::Instance().GetFarZ()
	};

	//とりあえず4枚使用の大前提として作成しているのでアサートで止める
	assert(ShadowmapCount == 4);

	//カメラのパラメーターを取得
	DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetFront()));
	DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetRight()));
	DirectX::XMVECTOR cameraUp    = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetUp()));
	DirectX::XMVECTOR cameraPos   = DirectX::XMLoadFloat3(&Camera::Instance().GetEye());

	DirectX::XMMATRIX viewProjectionMatrix = DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection);

	for (int i = 0; i < ShadowmapCount; ++i)
	{
		float nearDepth = splitAreaTable[i + 0];
		float farDepth = splitAreaTable[i + 1];

		//エリアを内包する視錐台の8頂点を算出する
		DirectX::XMVECTOR vertex[8];
		{
			//	エリアの近平面の中心からの上面までの距離を求める
			float	nearY = tanf(Camera::Instance().GetFovY() / 2.0f) * nearDepth;

			//	エリアの近平面の中心からの右面までの距離を求める
			float	nearX = nearY * Camera::Instance().GetAspect();

			//	エリアの遠平面の中心からの上面までの距離を求める
			float	farY = tanf(Camera::Instance().GetFovY() / 2.0f) * farDepth;

			//	エリアの遠平面の中心からの右面までの距離を求める
			float	farX = farY * Camera::Instance().GetAspect();

			//	エリアの近平面の中心座標を求める
			DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&Camera::Instance().GetEye());
			DirectX::XMVECTOR Front = DirectX::XMLoadFloat3(&Camera::Instance().GetFront());
			DirectX::XMVECTOR nearPos = DirectX::XMVectorAdd(Eye, DirectX::XMVectorScale(Front, nearDepth));


			//	エリアの遠平面の中心座標を求める
			DirectX::XMVECTOR	farPos = DirectX::XMVectorAdd(Eye, DirectX::XMVectorScale(Front, farDepth));


			//	8頂点を求める
			{
				// 近平面の右上
				XMVECTOR Up = XMLoadFloat3(&Camera::Instance().GetUp());
				XMVECTOR Right = XMLoadFloat3(&Camera::Instance().GetRight());
				vertex[0] = XMVectorAdd(XMVectorAdd(nearPos, XMVectorScale(Up, nearY)), XMVectorScale(Right, nearX));
				// 近平面の左上
				vertex[1] = XMVectorAdd(XMVectorAdd(nearPos, XMVectorScale(Up, nearY)), XMVectorScale(Right, -nearX));
				// 近平面の右下
				vertex[2] = XMVectorAdd(XMVectorAdd(nearPos, XMVectorScale(Up, -nearY)), XMVectorScale(Right, nearX));
				// 近平面の左下
				vertex[3] = XMVectorAdd(XMVectorAdd(nearPos, XMVectorScale(Up, -nearY)), XMVectorScale(Right, -nearX));

				// 遠平面の右上
				vertex[4] = XMVectorAdd(XMVectorAdd(farPos, XMVectorScale(Up, farY)), XMVectorScale(Right, farX));
				// 遠平面の左上
				vertex[5] = XMVectorAdd(XMVectorAdd(farPos, XMVectorScale(Up, farY)), XMVectorScale(Right, -farX));
				// 遠平面の右下
				vertex[6] = XMVectorAdd(XMVectorAdd(farPos, XMVectorScale(Up, -farY)), XMVectorScale(Right, farX));
				// 遠平面の左下
				vertex[7] = XMVectorAdd(XMVectorAdd(farPos, XMVectorScale(Up, -farY)), XMVectorScale(Right, -farX));
			}

			//8頂点をライトビュープロジェクション空間に変換して、最大値、最小値を求める
			DirectX::XMFLOAT3 vertexMin(FLT_MAX, FLT_MAX, FLT_MAX), vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (auto& it : vertex)
			{
				DirectX::XMFLOAT3 p;
				DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(it, viewProjectionMatrix));

				vertexMin.x = min(p.x, vertexMin.x);
				vertexMin.y = min(p.y, vertexMin.y);
				vertexMin.z = min(p.z, vertexMin.z);
				vertexMax.x = max(p.x, vertexMax.x);
				vertexMax.y = max(p.y, vertexMax.y);
				vertexMax.z = max(p.z, vertexMax.z);

			}

			//クロップ行列を求める
			float	xScale = 2.0f / (vertexMax.x - vertexMin.x);
			float	yScale = 2.0f / (vertexMax.x - vertexMin.x);

			float	xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
			float	yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;

			DirectX::XMFLOAT4X4	clopMatrix;
			clopMatrix._11 = xScale;
			clopMatrix._12 = 0;
			clopMatrix._13 = 0;
			clopMatrix._14 = 0;
			clopMatrix._21 = 0;
			clopMatrix._22 = yScale;
			clopMatrix._23 = 0;
			clopMatrix._24 = 0;
			clopMatrix._31 = 0;
			clopMatrix._32 = 0;
			clopMatrix._33 = 1;
			clopMatrix._34 = 0;
			clopMatrix._41 = xOffset;
			clopMatrix._42 = yOffset;
			clopMatrix._43 = 0;
			clopMatrix._44 = 1;


			//	ライトビュープロジェクション行列にクロップ行列を乗算
			XMMATRIX ClopMatrix = DirectX::XMLoadFloat4x4(&clopMatrix);
			DirectX::XMStoreFloat4x4(&lightViewProjection[i], viewProjectionMatrix * ClopMatrix);

		}
	}
}



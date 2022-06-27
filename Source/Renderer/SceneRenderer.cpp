#include "SceneRenderer.h"

#include "Camera.h"
#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"

SceneRenderer::SceneRenderer(UINT width, UINT height)
{
	renderTarget.release();
	depthStencil.release();

	renderTarget = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);
	depthStencil = std::make_unique<DepthStencil>(width, height);

	// 空描画用スプライト
	skyboxRenderer = std::make_unique<Sprite>();

	// ライトデバッグフラグ
	drawDebugPrimitive = false;
}

void SceneRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// 画面クリア＆レンダーターゲット設定
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView * dsv = graphics.GetDepthStencilView();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(renderTarget->GetWidth());
	vp.Height = static_cast<float>(renderTarget->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	RenderContext rc;
	rc.directionalLightData.direction = { 1.0f,-1.0f,-1.0f,0.0f };

	//カメラパラメーター設定
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	//ライトの情報をセット
	LightManager::Instance().PushRenderContext(rc);
	
	
	//シャドウマップの情報をセット
	rc.shadowmapData = shadowmapData;

	//モデル描画
	{
		ModelShader* shader = graphics.GetModelShader(ModelShaderId::PBR);
		shader->Begin(dc, rc);
		for(Model* model:renderModels)
		{
			shader->Draw(dc, rc,model);
		}
		shader->End(dc, rc);
	}
}

void SceneRenderer::DrawDebugGUI()
{
}

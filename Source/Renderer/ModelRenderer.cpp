#include "ModelRenderer.h"
//	描画モデルの登録
void ModelRenderer::RegisterRenderModel(Model* model)
{
	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it != renderModels.end())
		return;

	renderModels.push_back(model);
}

//	描画モデルの登録解除
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it == renderModels.end())
		return;

	renderModels.erase(it);
}

//	描画モデル登録全解除
void ModelRenderer::ClearRenderModel()
{
	renderModels.clear();
}

// 描画ターゲットの退避
void ModelRenderer::CacheRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSGetViewports(&cachedViewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
}

// 描画ターゲットを戻す
void ModelRenderer::RestoreRenderTargets(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(cachedViewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}
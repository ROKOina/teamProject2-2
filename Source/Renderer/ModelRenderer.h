#pragma once
#include "Graphics/Model.h"
#include "Graphics/RenderContext.h"

class ModelRenderer
{
public:
	ModelRenderer() {}
	virtual ~ModelRenderer() {}
public:
	// 描画実行
	//virtual void Render(ID3D11DeviceContext* dc,RenderContext& rc) = 0;
	virtual void Render(ID3D11DeviceContext* dc) = 0;

	//	描画モデルの登録
	void RegisterRenderModel(Model* model);

	//	描画モデルの登録解除
	void UnregisterRenderModel(Model* model);

	//	描画モデル登録全解除
	void ClearRenderModel();

	// デバッグ描画
	virtual void DrawDebugGUI() = 0;
protected:
	// 描画ターゲットの退避
	void CacheRenderTargets(ID3D11DeviceContext* dc);

	// 描画ターゲットを戻す
	void RestoreRenderTargets(ID3D11DeviceContext* dc);

protected:
	std::vector<Model*> renderModels;
	UINT			cachedViewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT	cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	cachedDepthStencilView;

};

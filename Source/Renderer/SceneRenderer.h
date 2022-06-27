#pragma once

#include "Renderer/ModelRenderer.h"
#include "Graphics/RenderContext.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/Sprite.h"
#include "d3d11.h"

class SceneRenderer:public ModelRenderer
{
public:
	SceneRenderer(UINT width, UINT height);
	~SceneRenderer(){}

	//シャドウマップ情報をRenderContextに積む
	void SetShadowmapData(ShadowmapData shadowMapData) { this->shadowmapData = shadowMapData; }

public:
	//	描画実行
	void Render(ID3D11DeviceContext* dc) override;

	//	デバッグGUI描画
	void DrawDebugGUI() override;

private:
	std::unique_ptr<RenderTarget>	renderTarget;		//	オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<DepthStencil>	depthStencil;		//	オフスクリーンレンダリング用深度バッファ
	std::unique_ptr<Sprite>			skyboxRenderer;		//	空描画用スプライト
	ShadowmapData					shadowmapData;		//	シャドウマップ情報
	bool							drawDebugPrimitive;	//	デバッグプリミティブ描画
	int                             timer;
};

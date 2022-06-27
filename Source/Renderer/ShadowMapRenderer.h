#pragma once

#include <memory>
#include "Graphics/RenderContext.h"
#include "Graphics/DepthStencil.h"
#include "Graphics/Light.h"
#include "Renderer/ModelRenderer.h"

class ShadowMapRenderer:public ModelRenderer
{
    static constexpr float Near = 1.0f;
    static constexpr float Far = 10000.0f;
public:
    ShadowMapRenderer(UINT shadowmapSize);
    ~ShadowMapRenderer();

    //シャドウマップ情報をRenderContextに積む
    ShadowmapData GetShadowMapData();
public:
    //描画実行
    //void Render(ID3D11DeviceContext* dc,RenderContext& rc)override;
    void Render(ID3D11DeviceContext* dc)override;

    //デバッグGUI描画
    void DrawDebugGUI()override;

private:
    //シャドウマップで使用する各行列を算出
    void CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);

    //ライトビュープロジェクション行列を算出
    void CalcLightviewProjectionMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);
private:
    ////シャドウマップを生成する平行光源
    //Light* mainDirectionalLight = nullptr;

    
    std::unique_ptr<DepthStencil>	shadowmapDepthStencil[ShadowmapCount];                          //シャドウマップ用深度ステンシルバッファ
    float                           shadowRect = 500.0f;                                            //描画範囲
    float							shadowBias[ShadowmapCount] = { 0.001f, 0.002f, 0.004f, 0.01f }; //深度比較用のオフセット値
    DirectX::XMFLOAT3				shadowColor = { 0.2f, 0.2f, 0.2f };                    //影の色
    DirectX::XMFLOAT4X4 lightViewProjection[ShadowmapCount];                                        //ライトビュープロジェクション行列

};

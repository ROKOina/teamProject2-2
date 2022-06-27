#include "PBR.hlsli"
#include "CommonFunctions.hlsli"

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1); // 法線マップ
Texture2D metallicSmoothnessMap : register(t2); // r:金属度, a:滑らかさ
Texture2D ambientOcclusionMap : register(t3); // r:環境光遮蔽

Texture2D shadowMap : register(t4); // シャドウマップ
Texture2D skyboxMap : register(t5); // 空テクスチャ

SamplerState albedoMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin):SV_TARGET
{
   
    // アルベドカラー(非金属部分)
    float4 albedoColor = albedoMap.Sample(albedoMapSamplerState, pin.texcoord) * pin.color;
    //albedoColor.rgb = sRGBToLinear(albedoColor.rgb);

    // 金属度
    float metallic = metallicSmoothnessMap.Sample(albedoMapSamplerState, pin.texcoord).r;
	// 滑らかさ
    float smoothness = metallicSmoothnessMap.Sample(albedoMapSamplerState, pin.texcoord).a;
    // 環境光遮蔽度
    float ambientOcculusion = ambientOcclusionMap.Sample(albedoMapSamplerState, pin.texcoord).r;

    	// 調整
    metallic = saturate(metallic + adjustMetalness);
    smoothness = saturate(smoothness + adjustSmoothness);

    // 粗さ
    float roughness = 1.0f - smoothness;
	// 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, metallic);

    // 垂直反射時のフレネル反射率
    float3 F0 = lerp(Dielectric, albedoColor.rgb, metallic);
	// 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
    float3 normal = normalMap.Sample(albedoMapSamplerState, pin.texcoord).xyz * 2.0f - 1.0f;
    float3 N = normalize(normal.x * normalize(pin.tangent) +
							normal.y * normalize(pin.binormal) +
							normal.z * normalize(pin.normal));

    float3 E = normalize(viewPosition.xyz - pin.world_position.xyz);

    // ライティング計算
    float3 directDiffuse = 0, directSpecular = 0;
	{
		// 平行光源の処理
        DirectBDRF(diffuseReflectance, F0, N, E, directionalLightData.direction.xyz,
				   directionalLightData.color.rgb, roughness,
				   directDiffuse, directSpecular);
		// 平行光源の影
        float3 shadow = CalcShadowColorPCFFilter(shadowMap, shadowMapSamplerState, pin.shadowTexcoord,
										shadowColor, shadowBias);
        directDiffuse *= shadow;
        directSpecular *= shadow;

    }

    return float4(directDiffuse + directSpecular, albedoColor.a);
}
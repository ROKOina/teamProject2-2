#include "PBR.hlsli"
#include "CommonFunctions.hlsli"

Texture2D albedoMap : register(t0);
Texture2D normalMap : register(t1); // �@���}�b�v
Texture2D metallicSmoothnessMap : register(t2); // r:�����x, a:���炩��
Texture2D ambientOcclusionMap : register(t3); // r:�����Օ�

Texture2D shadowMap : register(t4); // �V���h�E�}�b�v
Texture2D skyboxMap : register(t5); // ��e�N�X�`��

SamplerState albedoMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin):SV_TARGET
{
   
    // �A���x�h�J���[(���������)
    float4 albedoColor = albedoMap.Sample(albedoMapSamplerState, pin.texcoord) * pin.color;
    //albedoColor.rgb = sRGBToLinear(albedoColor.rgb);

    // �����x
    float metallic = metallicSmoothnessMap.Sample(albedoMapSamplerState, pin.texcoord).r;
	// ���炩��
    float smoothness = metallicSmoothnessMap.Sample(albedoMapSamplerState, pin.texcoord).a;
    // �����Օ��x
    float ambientOcculusion = ambientOcclusionMap.Sample(albedoMapSamplerState, pin.texcoord).r;

    	// ����
    metallic = saturate(metallic + adjustMetalness);
    smoothness = saturate(smoothness + adjustSmoothness);

    // �e��
    float roughness = 1.0f - smoothness;
	// ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, metallic);

    // �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(Dielectric, albedoColor.rgb, metallic);
	// �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
    float3 normal = normalMap.Sample(albedoMapSamplerState, pin.texcoord).xyz * 2.0f - 1.0f;
    float3 N = normalize(normal.x * normalize(pin.tangent) +
							normal.y * normalize(pin.binormal) +
							normal.z * normalize(pin.normal));

    float3 E = normalize(viewPosition.xyz - pin.world_position.xyz);

    // ���C�e�B���O�v�Z
    float3 directDiffuse = 0, directSpecular = 0;
	{
		// ���s�����̏���
        DirectBDRF(diffuseReflectance, F0, N, E, directionalLightData.direction.xyz,
				   directionalLightData.color.rgb, roughness,
				   directDiffuse, directSpecular);
		// ���s�����̉e
        float3 shadow = CalcShadowColorPCFFilter(shadowMap, shadowMapSamplerState, pin.shadowTexcoord,
										shadowColor, shadowBias);
        directDiffuse *= shadow;
        directSpecular *= shadow;

    }

    return float4(directDiffuse + directSpecular, albedoColor.a);
}
#include	"Constants.hlsli"
#include	"Light.hlsli"
#include	"ShadowmapFunctions.hlsli"
struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

    float3 shadowTexcoord : TEXCOORD1; //	�V���h�E�}�b�v��������擾���邽�߂�UV���W�{�[�x�l
};

cbuffer CbScene : register(b0)
{
    float4 viewPosition;
    row_major float4x4 viewProjection;
    float4 ambientLightColor;
    DirectionalLightData directionalLightData;
    int timer;
    float3 dummy2;
};

#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};

#define MAX_INSTANCES 512
//cbuffer CbInstancingMesh : register(b2)
//{
//    row_major float4x4 worldTransform[MAX_INSTANCES];
//};

cbuffer CbSubset : register(b2)
{
    float4 materialColor;
    float adjustMetalness;  // ���^���l�X�̒����l
    float adjustSmoothness; // ���炩���̒����l
    //float taMoveScale;      // �ړ��ʂ̃X�P�[���l
    //float taRepeatTime;     // ���s�[�g����
    //HemisphereLightData hemisphereLightData; // �������C�g���
};

cbuffer CbShadowmap : register(b3)
{
    row_major float4x4 lightViewProjection; // ���C�g�r���[�v���W�F�N�V�����s��
    float3 shadowColor; // �e�̐F
    float shadowBias; // �[�x�l��r���̃I�t�Z�b�g
};

// �U�d��(������ł��Œ�4%(0.0f4)�͋��ʔ��˂���)
static const float Dielectric = 0.04f;

//--------------------------------------------
// �g�U����BRDF(���K�������o�[�g�̊g�U����)
//--------------------------------------------
// diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// �t���l����
//--------------------------------------------
// F0 : �������ˎ��̔��˗�
// VdotH: �����x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g��
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1 - F0) * pow(1 - VdotH, 5);
}
//--------------------------------------------
// �@�����z�֐�
//--------------------------------------------
// NdotH : �@���x�N�g���ƃn�[�t�x�N�g���i�����ւ̃x�N�g���Ǝ��_�ւ̃x�N�g���̒��ԃx�N�g���j�̓���
// roughness : �e��
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = pow(roughness, 2);
    return a / PI * pow((pow(NdotH, 2) * pow(a, 2) + 1),2);
}
//--------------------------------------------
// �􉽌������̎Z�o
//--------------------------------------------
// Ndotv : �@���Ƒ��x�N�g���Ƃ̓���
// k : �e��/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / Ndotv * (1 - k) + k;

}
// NdotL : �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotV : �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// roughness : �e��
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
//--------------------------------------------
// NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
// NdotL		: �@���x�N�g���ƌ����ւ̃x�N�g���Ƃ̓���
// NdotH		: �@���x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// VdotH		: �����ւ̃x�N�g���ƃn�[�t�x�N�g���Ƃ̓���
// fresnelF0	: �������ˎ��̃t���l�����ːF
// roughness	: �e��
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	// D��(�@�����z)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	// G��(�􉽌�����)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	// F��(�t���l������)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return D * G * F / 4 * (NdotL) * (NdotV);

}

//--------------------------------------------
//	���ڌ��̕����x�[�X���C�e�B���O
//--------------------------------------------
// diffuseReflectance	: ���ˌ��̂����g�U���˂ɂȂ銄��
// F0					: �������ˎ��̃t���l�����ːF
// normal				: �@���x�N�g��(���K���ς�)
// eyeVector			: ���_�Ɍ������x�N�g��(���K���ς�)
// lightVector			: �����Ɍ������x�N�g��(���K���ς�)
// lightColor			: ���C�g�J���[
// roughness			: �e��
void DirectBDRF(float3 diffuseReflectance,
				float3 F0,
				float3 normal,
				float3 eyeVector,
				float3 lightVector,
				float3 lightColor,
				float roughness,
				out float3 outDiffuse,
		out float3 outSpecular)
{
    float3 N = normal;
    float3 L = -lightVector;
    float3 V = eyeVector;
    float3 H = normalize(L + V);

    float NdotV = max(0.2f, dot(N, V));
    float NdotL = max(0.2f, dot(N, L));
    float NdotH = max(0.2f, dot(N, H));
    float VdotH = max(0.2f, dot(V, H));

    float3 irradiance = lightColor * NdotL;

	// ���K�V�[�ȃ��C�e�B���O�Ƃ̌݊�����ۂꍇ��PI�ŏ�Z����
    irradiance *= PI;

	// �g�U����BRDF(���K�������o�[�g�̊g�U����)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;

	// ���ʔ���BRDF�i�N�b�N�E�g�����X�̃}�C�N���t�@�Z�b�g���f���j
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}

//--------------------------------------------
// ������BRDF
//--------------------------------------------
// F0			: �������ˎ��̃t���l�����ːF
// roughness	: �e��
// NdotV		: �@���x�N�g���Ǝ����ւ̃x�N�g���Ƃ̓���
float3 EnvBRDFApprox(float3 F0, float roughness, float NdotV)
{
    const float4 c0 = { -1.0f, -0.0275f, -0.572f, 0.022f };
    const float4 c1 = { 1.0f, 0.0425f, 1.04f, -0.04f };
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28f * NdotV)) * r.x + r.y;
    float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
    return F0 * AB.x + AB.y;
}

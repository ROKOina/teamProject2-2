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

    float3 shadowTexcoord : TEXCOORD1; //	シャドウマップから情報を取得するためのUV座標＋深度値
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
    float adjustMetalness;  // メタルネスの調整値
    float adjustSmoothness; // 滑らかさの調整値
    //float taMoveScale;      // 移動量のスケール値
    //float taRepeatTime;     // リピート時間
    //HemisphereLightData hemisphereLightData; // 半球ライト情報
};

cbuffer CbShadowmap : register(b3)
{
    row_major float4x4 lightViewProjection; // ライトビュープロジェクション行列
    float3 shadowColor; // 影の色
    float shadowBias; // 深度値比較時のオフセット
};

// 誘電率(非金属でも最低4%(0.0f4)は鏡面反射する)
static const float Dielectric = 0.04f;

//--------------------------------------------
// 拡散反射BRDF(正規化ランバートの拡散反射)
//--------------------------------------------
// diffuseReflectance	: 入射光のうち拡散反射になる割合
float3 DiffuseBRDF(float3 diffuseReflectance)
{
    return diffuseReflectance / PI;
}

//--------------------------------------------
// フレネル項
//--------------------------------------------
// F0 : 垂直入射時の反射率
// VdotH: 視線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル
float3 CalcFresnel(float3 F0, float VdotH)
{
    return F0 + (1 - F0) * pow(1 - VdotH, 5);
}
//--------------------------------------------
// 法線分布関数
//--------------------------------------------
// NdotH : 法線ベクトルとハーフベクトル（光源へのベクトルと視点へのベクトルの中間ベクトル）の内積
// roughness : 粗さ
float CalcNormalDistributionFunction(float NdotH, float roughness)
{
    float a = pow(roughness, 2);
    return a / PI * pow((pow(NdotH, 2) * pow(a, 2) + 1),2);
}
//--------------------------------------------
// 幾何減衰項の算出
//--------------------------------------------
// Ndotv : 法線と他ベクトルとの内積
// k : 粗さ/2
float CalcGeometryFunction(float Ndotv, float k)
{
    return Ndotv / Ndotv * (1 - k) + k;

}
// NdotL : 法線ベクトルと光源へのベクトルとの内積
// NdotV : 法線ベクトルと視線へのベクトルとの内積
// roughness : 粗さ
float CalcGeometryFunction(float NdotL, float NdotV, float roughness)
{
    float r = roughness * 0.5f;
    float shadowing = CalcGeometryFunction(NdotL, r);
    float masking = CalcGeometryFunction(NdotV, r);
    return shadowing * masking;
}
//--------------------------------------------
// 鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
//--------------------------------------------
// NdotV		: 法線ベクトルと視線へのベクトルとの内積
// NdotL		: 法線ベクトルと光源へのベクトルとの内積
// NdotH		: 法線ベクトルとハーフベクトルとの内積
// VdotH		: 視線へのベクトルとハーフベクトルとの内積
// fresnelF0	: 垂直入射時のフレネル反射色
// roughness	: 粗さ
float3 SpecularBRDF(float NdotV, float NdotL, float NdotH, float VdotH, float3 fresnelF0, float roughness)
{
	// D項(法線分布)
    float D = CalcNormalDistributionFunction(NdotH, roughness);
	// G項(幾何減衰項)
    float G = CalcGeometryFunction(NdotL, NdotV, roughness);
	// F項(フレネル反射)
    float3 F = CalcFresnel(fresnelF0, VdotH);
    return D * G * F / 4 * (NdotL) * (NdotV);

}

//--------------------------------------------
//	直接光の物理ベースライティング
//--------------------------------------------
// diffuseReflectance	: 入射光のうち拡散反射になる割合
// F0					: 垂直入射時のフレネル反射色
// normal				: 法線ベクトル(正規化済み)
// eyeVector			: 視点に向かうベクトル(正規化済み)
// lightVector			: 光源に向かうベクトル(正規化済み)
// lightColor			: ライトカラー
// roughness			: 粗さ
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

	// レガシーなライティングとの互換性を保つ場合はPIで乗算する
    irradiance *= PI;

	// 拡散反射BRDF(正規化ランバートの拡散反射)
    outDiffuse = DiffuseBRDF(diffuseReflectance) * irradiance;

	// 鏡面反射BRDF（クック・トランスのマイクロファセットモデル）
    outSpecular = SpecularBRDF(NdotV, NdotL, NdotH, VdotH, F0, roughness) * irradiance;
}

//--------------------------------------------
// 環境光のBRDF
//--------------------------------------------
// F0			: 垂直入射時のフレネル反射色
// roughness	: 粗さ
// NdotV		: 法線ベクトルと視線へのベクトルとの内積
float3 EnvBRDFApprox(float3 F0, float roughness, float NdotV)
{
    const float4 c0 = { -1.0f, -0.0275f, -0.572f, 0.022f };
    const float4 c1 = { 1.0f, 0.0425f, 1.04f, -0.04f };
    float4 r = roughness * c0 + c1;
    float a004 = min(r.x * r.x, exp2(-9.28f * NdotV)) * r.x + r.y;
    float2 AB = float2(-1.04f, 1.04f) * a004 + r.zw;
    return F0 * AB.x + AB.y;
}

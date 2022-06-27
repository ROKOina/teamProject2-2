#ifndef	__COMMONFUNCTIONS_HLSLI__
#define	__COMMONFUNCTIONS_HLSLI__
#include	"Constants.hlsli"
//--------------------------------------------
//	正距円筒図法に則ったUV座標計算関数
//--------------------------------------------
// v		: ベクトル
// 返す値	: UV座標
float2 EquirectangularProjection(float3 v)
{
    float2 texcoord;
    texcoord = float2(1.0f - atan2(v.z, v.x) / 2.0f, -atan2(v.y, length(v.xz)));
    texcoord = texcoord / PI + 0.5f;
    return texcoord;
}

//--------------------------------------------
//	sRGB色空間から線形色空間への変換
//--------------------------------------------
// srgb		: sRGB色空間色
// 返す値	: 線形色空間色
inline	float3 sRGBToLinear(float3 colorsRGB)
{
    return pow(colorsRGB, 2.2f);
}

//--------------------------------------------
//	線形色空間からsRGB色空間への変換
//--------------------------------------------
// linearRGB	: 線形色空間色
// 返す値		: sRGB色空間色
inline	float3 LinearTosRGB(float3 colorLinear)
{
    return pow(colorLinear, 1.0f / 2.2f);

}
#endif // #endif __COMMONFUNCTIONS_HLSLI__

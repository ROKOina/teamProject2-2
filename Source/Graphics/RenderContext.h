#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

//平行高原情報
struct DirectionalLightData
{
	//ライトの向き
	XMFLOAT4 direction;
	//ライトの色
	XMFLOAT4 color;
};

// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	color;		// 色
	float			    range;		// 範囲
	DirectX::XMFLOAT3	dummy;
};

// 点光源の最大数
static	constexpr	int	PointLightMax = 8;

//スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
	float range;
	float innerCorn;
	float outerCorn;
	float dummy;
};

static constexpr int SpotLightMax = 8;

static constexpr int ShadowmapCount = 4;

struct ShadowmapData
{
	//シャドウマップ要震度ステンシルバッファ
	ID3D11ShaderResourceView* shadowMap[ShadowmapCount];
	//	ライトビュープロジェクション行列
	//DirectX::XMFLOAT4X4		lightViewProjection[ShadowmapCount];
	DirectX::XMFLOAT4X4		lightViewProjection;
	//	影の色
	DirectX::XMFLOAT3			shadowColor;
	//	深度比較用のオフセット値
	float					shadowBias[ShadowmapCount];
};

//色調補正情報
struct ColorGradingData
{
	float	hueShift = 0;	// 色相調整
	float	saturation = 1;	// 彩度調整
	float	brightness = 1;	// 明度調整
	float	exposure = 1;
	float   gamma = 1;
	DirectX::XMFLOAT3 dummy;
};

// レンダーコンテキスト(シェーダー側に転送するデータ達)
struct RenderContext
{
	//カメラ情報
	DirectX::XMFLOAT4       viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	//ライト情報
	XMFLOAT4 ambientLightColor;
	DirectionalLightData directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// 点光源情報
	int					pointLightCount = 0;			// 点光源数

	SpotLightData spotLightData[SpotLightMax];
	int spotLightCount = 0;

	//色調補正データ
	ColorGradingData colorGradingData;

	//シャドウマップ情報
	ShadowmapData shadowmapData;

};

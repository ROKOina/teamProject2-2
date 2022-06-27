//--------------------------------------------
//	シャドウマップ参照用情報の計算
//--------------------------------------------
// worldPosition		: ワールド座標
// lightViewProjection	: ライトビュープロジェクション行列
// 返す値				: シャドウマップ参照用のUV座標及び深度情報
float3	CalcShadowTexcoord(float3 worldPosition, matrix lightViewProjection)
{
	// クリップ空間の座標に変換
	float4 worldViewProjectionPosition = mul(float4(worldPosition, 1), lightViewProjection);

	// W除算してNDC座標に変換
	worldViewProjectionPosition /= worldViewProjectionPosition.w;

	// NDC座標からUV座標に変換
	// ただし、Z値はすでに0〜1の範囲に変換されているので、何もしなくて良い
	worldViewProjectionPosition.y = -worldViewProjectionPosition.y;
	worldViewProjectionPosition.xy = 0.5f * worldViewProjectionPosition.xy + 0.5f;
	return worldViewProjectionPosition.xyz;
}

//--------------------------------------------
//	シャドウマップから深度値を取得して影かどうかを渡す
//--------------------------------------------
// tex				: シャドウマップ
// samplerState		: サンプラステート
// shadowTexcoord	: シャドウマップ参照用情報
// shadowBias		: 深度比較用のオフセット値
// shadowColor		: 影の色
// 返す値			: 影の色
float3	CalcShadowColor(Texture2D tex, SamplerState samplerState, float3 shadowTexcoord, float3 shadowColor, float shadowBias)
{
	// シャドウマップから深度値取得
	float depth = tex.Sample(samplerState, shadowTexcoord.xy).r;
	// 深度値を比較して影かどうかを判定する
	return lerp(shadowColor, 1, step(shadowTexcoord.z - depth, 0.00005));
}

//--------------------------------------------
//	PCFフィルター付きソフトシャドウマップ
//--------------------------------------------
// tex				: シャドウマップ
// samplerState		: サンプラステート
// shadowTexcoord	: シャドウマップ参照用情報
// shadowBias		: 深度比較用のオフセット値
// shadowColor		: 影の色
// 返す値			: 影の色
float3	CalcShadowColorPCFFilter(Texture2D tex, SamplerState samplerState, float3 shadowTexcoord, float3 shadowColor, float shadowBias)
{
	// テクセルサイズの計算
	float2 texelSize;
	{
		// テクスチャの縦幅横幅を取得する
		uint width, height;
		tex.GetDimensions(width, height);

		// 算出
		texelSize = float2(1.0f / width, 1.0f / height);
	}

	float	factor	= 0;
	static	const	int	PCFKernelSize = 5;	//	指定は奇数にすること
	for (int x = -PCFKernelSize / 2; x <= PCFKernelSize / 2; ++x)
	{
		for (int y = -PCFKernelSize / 2; y <= PCFKernelSize / 2; ++y)
		{
			// シャドウマップから深度値取得
			float depth = tex.Sample(samplerState, shadowTexcoord.xy + texelSize * float2(x, y)).r;
            factor += step(shadowTexcoord.z - depth, 0.00005f);
        }
	}
	// 深度値を比較して影かどうかを判定する
	return lerp(shadowColor, 1, factor / (PCFKernelSize * PCFKernelSize));
}

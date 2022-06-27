#ifndef	__COMMONFUNCTIONS_HLSLI__
#define	__COMMONFUNCTIONS_HLSLI__
#include	"Constants.hlsli"
//--------------------------------------------
//	�����~���}�@�ɑ�����UV���W�v�Z�֐�
//--------------------------------------------
// v		: �x�N�g��
// �Ԃ��l	: UV���W
float2 EquirectangularProjection(float3 v)
{
    float2 texcoord;
    texcoord = float2(1.0f - atan2(v.z, v.x) / 2.0f, -atan2(v.y, length(v.xz)));
    texcoord = texcoord / PI + 0.5f;
    return texcoord;
}

//--------------------------------------------
//	sRGB�F��Ԃ�����`�F��Ԃւ̕ϊ�
//--------------------------------------------
// srgb		: sRGB�F��ԐF
// �Ԃ��l	: ���`�F��ԐF
inline	float3 sRGBToLinear(float3 colorsRGB)
{
    return pow(colorsRGB, 2.2f);
}

//--------------------------------------------
//	���`�F��Ԃ���sRGB�F��Ԃւ̕ϊ�
//--------------------------------------------
// linearRGB	: ���`�F��ԐF
// �Ԃ��l		: sRGB�F��ԐF
inline	float3 LinearTosRGB(float3 colorLinear)
{
    return pow(colorLinear, 1.0f / 2.2f);

}
#endif // #endif __COMMONFUNCTIONS_HLSLI__

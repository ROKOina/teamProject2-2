#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

//���s�������
struct DirectionalLightData
{
	//���C�g�̌���
	XMFLOAT4 direction;
	//���C�g�̐F
	XMFLOAT4 color;
};

// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	color;		// �F
	float			    range;		// �͈�
	DirectX::XMFLOAT3	dummy;
};

// �_�����̍ő吔
static	constexpr	int	PointLightMax = 8;

//�X�|�b�g���C�g���
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
	//�V���h�E�}�b�v�v�k�x�X�e���V���o�b�t�@
	ID3D11ShaderResourceView* shadowMap[ShadowmapCount];
	//	���C�g�r���[�v���W�F�N�V�����s��
	//DirectX::XMFLOAT4X4		lightViewProjection[ShadowmapCount];
	DirectX::XMFLOAT4X4		lightViewProjection;
	//	�e�̐F
	DirectX::XMFLOAT3			shadowColor;
	//	�[�x��r�p�̃I�t�Z�b�g�l
	float					shadowBias[ShadowmapCount];
};

//�F���␳���
struct ColorGradingData
{
	float	hueShift = 0;	// �F������
	float	saturation = 1;	// �ʓx����
	float	brightness = 1;	// ���x����
	float	exposure = 1;
	float   gamma = 1;
	DirectX::XMFLOAT3 dummy;
};

// �����_�[�R���e�L�X�g(�V�F�[�_�[���ɓ]������f�[�^�B)
struct RenderContext
{
	//�J�������
	DirectX::XMFLOAT4       viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	//���C�g���
	XMFLOAT4 ambientLightColor;
	DirectionalLightData directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// �_�������
	int					pointLightCount = 0;			// �_������

	SpotLightData spotLightData[SpotLightMax];
	int spotLightCount = 0;

	//�F���␳�f�[�^
	ColorGradingData colorGradingData;

	//�V���h�E�}�b�v���
	ShadowmapData shadowmapData;

};

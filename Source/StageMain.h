#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision.h"
#include <memory>
#include <vector>
using namespace std;

//�}�b�v�̑傫���i�}�b�v�`�b�v�j
#define  MAP_X (13)
#define  MAP_Z (7)
#define  MAP_Y (2)

//�X�e�[�W
class StageMain
{
public:
    StageMain();
    ~StageMain();

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    void UpdateTransform();


    //���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //�C���X�^���X�擾
    static StageMain& Instance();

private:
    //vector<vector<Model::Node>> map[MAP_X][MAP_Y]; //�}�b�v�`�b�v�����ꕨ
    
    //Y,Z,X�Ȃ̂Œ��ӁI�I�I�I�I�I�I
    Model::Node map[MAP_Y][MAP_Z][MAP_X] = {};
    Model* model = nullptr;


};
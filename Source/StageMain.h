#pragma once

#include "Graphics/shader.h"
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
    void Render(ID3D11DeviceContext* dc, RenderContext rc,ModelShader* shader);

    void UpdateTransform();

    //���C�L���X�g
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //�C���X�^���X�擾
    static StageMain& Instance();

    //���f���Q�b�^�[
    Model* GetModel()const { return model; }

    //ImGui�f�o�b�O�\��
    void DrawDebugGUI();

    //Y,Z,X�Ȃ̂Œ��ӁI�I�I�I�I�I�I
    Model::Node map[MAP_Y][MAP_Z][MAP_X] = {};


private:
    //vector<vector<Model::Node>> map[MAP_X][MAP_Y]; //�}�b�v�`�b�v�����ꕨ
    
    Model* model = nullptr;


};
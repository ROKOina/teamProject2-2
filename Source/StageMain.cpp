#include "StageMain.h"

static Stage* instance = nullptr;

DirectX::XMFLOAT4X4 transform = {
1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 1, 0,
0, 0, 0, 1
};


//�C���X�^���X�擾
Stage& Stage::Instance()
{
    return *instance;
}
Model::Node Barel;
//�R���X�g���N�^
Stage::Stage()
{
    instance = this;

    //�X�e�[�W���f����ǂݍ���
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/Jammo/Jammo.mdl");
    model = new Model("Data/Model/blockMap1/map1.mdl");
    //model = new Model("Data/Model/blockMap/blockMap.mdl");

    model->nodeSearch("Button_Platform_01_Red", &Barel);
}

Stage::~Stage()
{
    //�X�e�[�W���f����j��
    delete model;
}

void Stage::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(0.01f, 0.01f, 0.01f);
    //��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&model->GetNodes().at(0).rotate));
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(model->GetNodes().at(0).translate.x, model->GetNodes().at(0).translate.y, model->GetNodes().at(0).translate.z);
    //3�̍s���g�ݍ��킹�āA���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

//�X�V����
void Stage::Update(float elapsedTime)
{
    //���͂�邱�ƂȂ�
    //UpdateTransform();
    model->UpdateTransform(transform);
}

//���C�L���X�g
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}


//�`�揈��
void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //�V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}

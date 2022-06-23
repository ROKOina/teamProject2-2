#include "StageMain.h"

static StageMain* instance = nullptr;

DirectX::XMFLOAT4X4 transform = {
1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 1, 0,
0, 0, 0, 1
};

//�C���X�^���X�擾
StageMain& StageMain::Instance()
{
    return *instance;
}

//�R���X�g���N�^
StageMain::StageMain()
{
    instance = this;

    //�X�e�[�W���f����ǂݍ���
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/Jammo/Jammo.mdl");
    model = new Model("Data/Model/blockMap1/map1.mdl");
    //model = new Model("Data/Model/blockMap/blockMap.mdl");
    
    model->nodeSearch("Button_Platform_01_Red", map);   //map�Ƀm�[�h������(char�^�̈����g��Ȃ�)
}

StageMain::~StageMain()
{
    //�X�e�[�W���f����j��
    delete model;
}

void StageMain::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    //��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&model->GetNodes().at(0).rotate));
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(model->GetNodes().at(0).translate.x, model->GetNodes().at(0).translate.y, model->GetNodes().at(0).translate.z);
    //3�̍s���g�ݍ��킹�āA���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}


#include "Input/Input.h"
//�X�V����
void StageMain::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {   //���肤������
        map[0][0][0].translate.x += 1;
        model->GetNodes()[map[0][0][0].Index]= map[0][0][0];
    }

    model->UpdateTransform(transform);
}

//���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}


//�`�揈��
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //�V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
    shader->Draw(dc, model);
}

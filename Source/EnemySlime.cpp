#include <imgui.h>
#include "EnemySlime.h"

//�R���X�g���N�^
EnemySlime::EnemySlime()
{
    model = new Model("Data/Model/Slime/Slime.mdl");

    //���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    //���A�����ݒ�
    radius = 0.5f;
    height = 1.0f;
}

//�f�X�g���N�^
EnemySlime::~EnemySlime()
{
    delete model;
}

//�X�V����
void EnemySlime::Update(float elapsedTime)
{

    //���G���ԍX�V
    UpdateInvincible(elapsedTime);

    UpdateVelocity(elapsedTime);

    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);

}

//�`�揈��
void EnemySlime::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

//�f�o�b�OGUI
void EnemySlime::GuiEnemy()
{
        ImGui::InputFloat3("Position", &this->position.x);
        ImGui::InputFloat("Radius", &this->radius);
        ImGui::InputFloat("Height", &this->height);
        ImGui::InputInt("Health", &this->health);

}

//���S�������ɌĂ΂��
void EnemySlime::OnDead()
{
    this->Destroy();
}
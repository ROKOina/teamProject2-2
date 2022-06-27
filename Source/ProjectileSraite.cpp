#include "ProjectileStraite.h"

//�R���X�g���N�^
ProjectileStraite::ProjectileStraite(ProjectileManager* manager)
    :Projectile(manager)    //���N���X�̃R���X�g���N�^���Ăяo��
{
    model = new Model("Data/Model/Sword/Sword.mdl");
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");

    //�\���T�C�Y�𒲐�
    scale.x = scale.y = scale.z = 3.0f;
    //scale.x = scale.y = scale.z = 0.5f;
}

ProjectileStraite::~ProjectileStraite()
{
    delete model;
}

//�X�V����
void ProjectileStraite::Update(float elapsedTime)
{
    //��������
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0)
    {
        //�������폜
        Destroy();
    }

    //�ړ�
    float speed = this->speed * elapsedTime;
    //direction(�����x�N�g�����g���āj���������߂�
    position.x += direction.x *  speed;
    position.y += direction.y *  speed;
    position.z += direction.z *  speed;
    
    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);
}

//�`�揈��
void ProjectileStraite::Render(ID3D11DeviceContext* dc, ModelShader* shader)
{
    RenderContext rc = {};
    shader->Draw(dc, rc, model);
}

//����
void ProjectileStraite::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}
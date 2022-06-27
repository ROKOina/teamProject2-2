#include "ProjectileHoming.h"

//�R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager)
    :Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");

    //���f�����������̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 3.0f;
}

//�f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

//�X�V����
void ProjectileHoming::Update(float elapsedTime)
{
    //��������
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0)
    {
        //�������폜
        Destroy();
    }

    //�ړ�
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        position.x += direction.x *  moveSpeed;
        position.y += direction.y *  moveSpeed;
        position.z += direction.z *  moveSpeed;
    }

    //����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;
        //�^�[�Q�b�g�܂ł̃x�N�g���Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);

        //�[���x�N�g���łȂ��Ȃ��]����
        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 0.00001f)
        {
            //�^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
            Vec = DirectX::XMVector3Normalize(Vec);

            //�����Ă�������x�N�g�����Z�o
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);

            //�O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓��ρi�p�x�j���Z�o
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction, Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            //�Q�̒P�ʃx�N�g���̊p�x���������ق�
            //1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
            float rot = 1.0f - dot;//�␳��
            if (rot > turnSpeed)rot = turnSpeed;

            //��]�p�x������Ȃ��]����������
            if (rot > 0&&dot>-0.3f) //dot�Œǔ��p�x�����߂�
            {
                //��]�����Z�o
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);   //�^�[�Q�b�g�܂łƑO���̊O�ς����

                //��]���Ɖ�]�ʂ����]�s����Z�o
                DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationRollPitchYaw(
                    DirectX::XMVectorGetX(Axis) * rot,
                    DirectX::XMVectorGetY(Axis) * rot,
                    DirectX::XMVectorGetZ(Axis) * rot);

                //���݂̍s�����]������
                DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                Transform = DirectX::XMMatrixMultiply(Transform, Rotation);

                //��]��̑O�����������o���A�P�ʃx�N�g��������
                Direction = Transform.r[2];
                Direction = DirectX::XMVector3Normalize(Direction);
                DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }

    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);
}

//�`�揈��
void ProjectileHoming::Render(ID3D11DeviceContext* dc, ModelShader* shader)
{
    RenderContext rc = {};
    shader->Draw(dc, rc , model);
}

//����
void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}
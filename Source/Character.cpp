#include "Character.h"
#include "Stage.h"
#include <imgui.h>
#include "Logger.h"
//�s��X�V����
void Character::UpdateTransform()
{
    //�X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    //��]�s����쐬
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    //DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    //DirectX::XMMATRIX Y = DirectX::XMMatrixRotationX(angle.y);
    //DirectX::XMMATRIX Z = DirectX::XMMatrixRotationX(angle.z);
    //DirectX::XMMATRIX R = X * Y * Z;
    
    //�ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    //3�̍s���g�ݍ��킹�āA���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    //�v�Z�������[���h�s������o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

//�ړ�����
void Character::Move(float vx, float vz, float speed)
{
    //speed *= elapsedTime;
    //position.x += vx * speed;
    //position.z += vz * speed;

    //�ړ������x�N�g����ݒ�
    moveVecX = vx;
    moveVecZ = vz;

    //�ő呬�x�ݒ�
    maxMoveSpeed = speed;
}

//�W�����v����
void Character::Jump(float speed)
{
    //������̗͂�ݒ�
    velocity.y = speed;
}

//�Ռ���^����
void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
    //���͂ɗ͂�������
    //DirectX::XMVECTOR Impulse=DirectX::XMLoadFloat3(&impulse);
    //DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity);
    //Velocity=DirectX::XMVectorAdd(Impulse, Velocity);
    //DirectX::XMStoreFloat3(&velocity, Velocity);

    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

//���͏����X�V
void Character::UpdateVelocity(float elapsedTime)
{
    //�o�߃t���[��
    float elapsedFrame = 60.0f * elapsedTime;

    //�������͍X�V����
    UpdateVertialVelocity(elapsedFrame);

    //�������͍X�V����
    UpdateHorizontalVelocity(elapsedFrame);

    //�����ړ��X�V����
    UpdateVertialMove(elapsedTime);

    //�����ړ��X�V����
    UpdateHorizontalMove(elapsedTime);
}

//�������͍X�V����
void Character::UpdateVertialVelocity(float elapsedFrame)
{
    //�d�͏���
    velocity.y += gravity * elapsedFrame;
}

//�������͍X�V����
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{
    //XZ���ʂ̑��͂���������
    float length = fabsf(velocity.x) + fabsf(velocity.z);
    if (length > 0.0f)
    {
        //���C��
        float Friction = this->friction * elapsedFrame;

       //�󒆂ɂ���Ƃ��͖��C�͂����炷
        if (!isGround)Friction -= airControl;

        //���C�ɂ�鉡�����̌�������
        if (length > friction)
        {
            DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity);
            Velocity=DirectX::XMVectorSetY(Velocity, 0);//xz���ʂɂ���
            Velocity = DirectX::XMVector3Normalize(Velocity);   //���K������1�ɂ���
            Velocity=DirectX::XMVectorScale(Velocity, friction);    //���C���|����
            velocity.x -= DirectX::XMVectorGetX(Velocity);
            velocity.z -= DirectX::XMVectorGetZ(Velocity);
        }
        //�������̑��͂����C�͈ȉ��ɂȂ����̂ŁA���͂𖳌���
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }

    }

    //XZ���ʂ̑��͂���������
    if (length <= maxMoveSpeed)
    {
        //�ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
        float moveVecLength = fabsf(moveVecX) + fabsf(moveVecZ);
        if (moveVecLength > 0.0f)
        {
            //������
            float acceleration = this->acceleration * elapsedFrame;
           
            //�󒆂ɂ���Ƃ��͉����͂����炷
            if (!isGround)acceleration -= airControl;
            
            //�ړ��x�N�g���ɂ���������
            velocity.x += moveVecX*acceleration;
            velocity.z += moveVecZ*acceleration;
            
            //�ő呬�x����
            DirectX::XMVECTOR Velocity = DirectX::XMLoadFloat3(&velocity);
            Velocity = DirectX::XMVectorSetY(Velocity, 0);   //y����0��
            DirectX::XMVECTOR Length = DirectX::XMVector3Length(Velocity);

            float length = DirectX::XMVectorGetX(Length);
            //float length = fabsf(velocity.x) + fabsf(velocity.z);
            if (length > maxMoveSpeed)
            {
               //DirectX::XMVECTOR Velocity=DirectX::XMLoadFloat3(&velocity);
               //Velocity = DirectX::XMVectorSetY(Velocity, 0);   //y����0��

               Velocity = DirectX::XMVector3Normalize(Velocity);    //���K������
               Velocity = DirectX::XMVectorScale(Velocity, maxMoveSpeed);//�ő呬�x�ɐݒ�
               
               velocity.x = DirectX::XMVectorGetX(Velocity);
               velocity.z = DirectX::XMVectorGetZ(Velocity);
            }

            //�����ŃK�^�K�^���Ȃ��悤�ɂ���
            if (slopeRate > 0)
            {
                //�d�͏���
                velocity.y += gravity * elapsedFrame;
            }
        }
    }

    //�ړ��x�N�g�������Z�b�g
    moveVecX = 0;
    moveVecZ = 0;
}

//�����ړ��X�V����
void Character::UpdateVertialMove(float elapsedTime)
{
    ////�ړ�����
    //position.y += velocity.y * elapsedTime;

    ////�n�ʔ���
    //if (position.y < 0.0f)
    //{
    //    position.y = 0.0f;
    //    velocity.y = 0.0f;

    //    //���n����
    //    if (!IsGround())
    //    {
    //        OnLoadding();
    //        isGround = true;
    //    }
    //}
    //else
    //{
    //    isGround = false;
    //}

    //���������̈ړ���
    float my = velocity.y * elapsedTime;

    //�L�����N�^�[��Y�������ƂȂ�@���x�N�g��
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    slopeRate = 0.0f;   //�⓹���������p

    //������
    if (my < 0.0f)
    {
        //���C�̊J�n�ʒu�͑�����菭����
        DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
        //���C�̏I�_�ʒu�͈ړ���̈ʒu
        DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };

        //���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;
        if (Stage::Instance().RayCast(start, end, hit))
        {
            //�@���x�N�g���擾

            //�n�ʂɐڒn���Ă���
            position.y = hit.position.y;

            //�X�Η��̌v�Z( ����/(���+����) )������hit.distance

            float syahen = hit.normal.x * hit.normal.x + hit.normal.y * hit.normal.y + hit.normal.z * hit.normal.z;
            float bottom = syahen / 5 * 4;
            float height = syahen / 5 * 3;
            slopeRate = height / (bottom + height);

            //���n����
            if (!isGround)
            {
                OnLanding();
            }
            isGround = true;
            velocity.y = 0.0f;
        }
        else
        {
            //�󒆂ɕ����Ă���
            position.y += my;
            isGround = false;
        }
    }
    //�㏸��
    else if (my > 0.0f)
    {
        position.y += my;
        isGround = false;
    }
}

//�����ړ��X�V����
void Character::UpdateHorizontalMove(float elapsedTime)
{
    ////�ړ�����
    //position.x += velocity.x * elapsedTime;
    //position.z += velocity.z * elapsedTime;
   
    //�������͗ʌv�Z
    float velocityLengthXZ = fabsf(velocity.x) + fabsf(velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        //�����ړ���
        float mx = velocity.x * elapsedTime;
        float mz = velocity.z * elapsedTime;

        //���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x ,position.y + stepOffset ,position.z };
        
        DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
        DirectX::XMVECTOR StartC = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Start), this->radius);
        Start = DirectX::XMVectorAdd(Start, StartC);
        DirectX::XMStoreFloat3(&start, Start);

        DirectX::XMFLOAT3 end = { start.x + mx ,start.y ,start.z + mz };

        //���C�L���X�g�ɂ��ǔ���
        HitResult hit;
        if (Stage::Instance().RayCast(start, end, hit))
        {
            //�ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR StartHit = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(StartHit, End);

            //�ǂ̖@��
            DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, Normal);

            //Logger::Print("x:%.2f y:%.2f z:%.2f\n", normal.x, normal.y, normal.z);

            //���˃x�N�g����@���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Normal, Vec);

            //�␳��̌v�Z
            DirectX::XMVECTOR ND = DirectX::XMVectorScale(Normal, DirectX::XMVectorGetX(Dot));
            DirectX::XMVECTOR EndWall = DirectX::XMVectorScale(Normal, DirectX::XMVectorGetX(Dot)*1.2f);

            //�␳��ɕ��̂��������ꍇ�~�܂�
            DirectX::XMStoreFloat3(&start, Start);
            DirectX::XMStoreFloat3(&end, EndWall);
            start.y += stepOffset * 5;
            end.y += stepOffset * 5;
            if (Stage::Instance().RayCast(start, end, hit))return;  //���̂��������ꍇreturn

            DirectX::XMVECTOR VecRay = DirectX::XMVectorSubtract(End, Start);
            DirectX::XMVECTOR R = DirectX::XMVectorAdd(VecRay, ND); //�ǂ���ׂ��Ƃ�擾



            //�ǂ���ړ�
            position.x += DirectX::XMVectorGetX(R);
            position.z += DirectX::XMVectorGetZ(R);

            //�R���p�C�����萔(https://www.slideshare.net/GenyaMurakami/constexpr-10458089
            constexpr float epsilon = 0.001;

            position.x += normal.x * epsilon;
            position.z += normal.z * epsilon;


            //DirectX::XMVECTOR Pos = DirectX::XMLoadFloat3(&position);
            //Pos = DirectX::XMVectorAdd(Pos,R);
            //DirectX::XMStoreFloat3(&position, Pos);

        }
        else
        {
            //�ړ�
            position.x += mx;
            position.z += mz;
        }
    }
}

//���񏈗�
void Character::Turn(float elapsedTime, float vx, float vz, float speed)
{
    speed *= elapsedTime;


    //�i�s�x�N�g����P�ʃx�N�g����
    float moveVecLength = sqrtf(vx * vx + vz * vz);
    if (moveVecLength <= 0)return;  //�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
    if (moveVecLength > 0.0f)
    {
        //�P�ʃx�N�g����
        vx /= moveVecLength;
        vz /= moveVecLength;
    }

    //���g�̉�]�l����O���������߂�
    float frontX = sinf(angle.y);
    float frontZ = cosf(angle.y);

    //��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
    float dot = (vx * frontX) + (vz * frontZ);

    //���ϒl��-1.0~1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
    //�������قǂ�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
    float rot = 1.0f - dot; //�␳��
    if (rot > speed)rot = speed;

    //���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
    float cross = (vx * frontZ) - (vz * frontX);

    //2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
    //���E������s�����Ƃɂ���č��E��]��I������
    if (cross < 0.0f)   //�E
    {
        angle.y -= rot;
    }
    else
    {
        angle.y += rot;
    }

}

//�_���[�W��^����
bool Character::ApplyDamage(int damage, float invincibleTime)
{
    //�_���[�W��0�̏ꍇ�͌��N��Ԃ�ύX����K�v���Ȃ�
    if (damage <= 0)return false;

    //���S���Ă���ꍇ�͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0)return false;

    //���G����
    if (invincibleTimer > 0)return false;

    //�_���[�W����
    health -= damage;

    //���G���ԏ���
    invincibleTimer = invincibleTime;

    //���S�ʒm
    if (health <= 0)
    {
        OnDead();
    }
    //�_���[�W�ʒm
    else
    {
        OnDamage();
    }

    //���N��Ԃ��ύX�����ꍇ��true��Ԃ�
    return true;
}

//���G���ԍX�V
void Character::UpdateInvincible(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}
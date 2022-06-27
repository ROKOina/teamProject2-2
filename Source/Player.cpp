#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "StageMain.h"

//�R���X�g���N�^
Player::Player()
{
    model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    //���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;
    position.z -= 3;    //kari
    this->radius = 0.3f;

    //�q�b�g�G�t�F�N�g�ǂݍ���
    //hitEffect = new Effect("Data/Effect/Hit.efk");
}

//�f�X�g���N�^
Player::~Player()
{
    delete hitEffect;
    delete model;
}


//�X�e�B�b�N���͒l
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    //���͏����擾
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    ////�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    //Camera& camera = Camera::Instance();
    //const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    //const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    ////�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

    ////�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    //float cameraRightX = cameraRight.x;
    //float cameraRightZ = cameraRight.z;
    //float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    //if (cameraRightLength > 0.0f)
    //{
    //    //�P�ʃx�N�g����
    //    cameraRightX /= cameraRightLength;
    //    cameraRightZ /= cameraRightLength;
    //}

    ////�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    //float cameraFrontX = cameraFront.x;
    //float cameraFrontZ = cameraFront.z;
    //float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    //if (cameraFrontLength > 0.0f)
    //{
    //    //�P�ʃx�N�g����
    //    cameraFrontX /= cameraFrontLength;
    //    cameraFrontZ /= cameraFrontLength;
    //}

   

    //�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    //�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    //�i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x =  ax;
    vec.z =  ay ;
    //vec.x = cameraFrontX * ay + cameraRightX * ax;
    //vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    //Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}

//�X�V����
void Player::Update(float elapsedTime)
{
    DirectX::XMFLOAT3 oldPlayerPos = this->GetPosition();

    //�ړ����͏���
    InputMove(elapsedTime);

    //�W�����v���͏���
    //InputJump();
    
    //�e�ۓ��͏���
    //InputProjectile();

    //���͏����X�V
    UpdateVelocity(elapsedTime);

    //�e�ۍX�V����
    //projectileManager.Update(elapsedTime);

    //�v���C���[�ƓG�Ƃ̏Փˏ���
    //CollisionPlayerVsEnemies();

    //�e�ۂƓG�̏Փ˔���
    //CollisionProjectileVsEnemies();

    //�v���C���[��stage�̓����蔻��
    CollisionStage(oldPlayerPos);

    //�I�u�W�F�N�g�̍s����X�V
    UpdateTransform();

    //���f���s��X�V
    model->UpdateTransform(transform);  //�ʒu�s���n��
}


void Player::InputMove(float elapsedTime)
{
    //�i�s�x�N�g���擾
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    //�ړ�����
    Move(moveVec.x, moveVec.z, moveSpeed);

    //���񏈗�
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//�W�����v���͏���
void Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) 
    {
        if (JumpCount < JumpLimit)  //�W�����v�񐔐���
        {
            Jump(jumpSpeed);
            ++JumpCount;
        }
    }
}

//���n�������ɌĂ΂��
void Player::OnLanding()
{
    JumpCount = 0;
}

////�W�����v����
//void Player::Jump(float speed)
//{
//    //������̗͂�ݒ�
//    velocity.y = speed;
//}
//
////���͏����X�V
//void Player::UpdateVelocity(float elapsedTime)
//{
//    //�o�߃t���[��
//    float elapsedFrame = 60.0f * elapsedTime;
//
//    //�d�͏���
//    velocity.y += gravity * elapsedFrame;
//
//    //�ړ�����
//    position.y += velocity.y * elapsedTime;
//
//    //�n�ʔ���
//    if (position.y < 0.0f)
//    {
//        position.y = 0.0f;
//        velocity.y = 0.0f;
//    }
//}
//
////���񏈗�
//void Player::Turn(float elapsedTime, float vx, float vz, float speed)
//{
//    speed *= elapsedTime;
//
//
//    //�i�s�x�N�g����P�ʃx�N�g����
//    float moveVecLength = sqrtf(vx * vx + vz * vz);
//    if (moveVecLength <= 0)return;  //�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
//    if (moveVecLength > 0.0f)
//    {
//        //�P�ʃx�N�g����
//        vx /= moveVecLength;
//        vz /= moveVecLength;
//    }
//
//    //���g�̉�]�l����O���������߂�
//    float frontX = sinf(angle.y);
//    float frontZ = cosf(angle.y);
//
//    //��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
//    float dot = (vx * frontX) + (vz * frontZ);
//
//    //���ϒl��-1.0~1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
//    //�������قǂ�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
//    float rot = 1.0f - dot; //�␳��
//    if (rot > speed)rot = speed;
//
//    //���E������s�����߂�2�̒P�ʃx�N�g���̊O�ς��v�Z����
//    float cross = (vx * frontZ) - (vz * frontX);
//
//    //2D�̊O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
//    //���E������s�����Ƃɂ���č��E��]��I������
//    if (cross < 0.0f)   //�E
//    {
//        angle.y -= rot;
//    }
//    else
//    {
//        angle.y += rot;
//    }
//
//}


//�v���C���[�Ƃ̓����蔻��
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    //�S�Ă̐��Ƒ�������ŏՓˏ���
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        //�Փˏ���
        DirectX::XMFLOAT3 outPosition;
        //if (Collision::IntersectSphereVsSphere(   //�~
        //    this->GetPosition(), this->GetRadius(),
        //    enemy->GetPosition(), enemy->GetRadius(),
        //    outPosition))
        //{
        //    //�����o����̈ʒu�ݒ�
        //    enemy->SetPosition(outPosition);
        //}

        if (Collision::IntersectCylinder(
            this->GetPosition(), this->GetRadius(), this->GetHeight(),
            enemy->GetPosition(), enemy->GetRadius(), enemy->GetHeight(),
            outPosition
        ))
        {
            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&this->GetPosition());
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR EP = DirectX::XMVectorSubtract(P, E);
            DirectX::XMVECTOR N = DirectX::XMVector3Normalize(EP);

            //�����o����̈ʒu�ݒ�
            if(DirectX::XMVectorGetY(N)>0.8f) Jump(10);
            /*if(IsGround())*/
            else enemy->SetPosition(outPosition);
        }
    }
}

//�v���C���[��stage�̓����蔻��
void Player::CollisionStage(DirectX::XMFLOAT3 oldPlayerPos)
{

    //�X�e�[�W�͈͓�
    for (int y = 0; y < MAP_Y; ++y) 
    {
        for (int z = 0; z < MAP_Z; ++z) //���ʂŉ�
        {
            for (int x = 0; x < MAP_X; ++x)
            {
                    DirectX::XMFLOAT3 outPosition;
                if (y == 0) {
                    if (StageMain::Instance().map[y][z][x].name)continue;   //none�u���b�N�Ȃ画��
                    outPosition = oldPlayerPos; //�P�t���[�����O��position����

                    int xp = x - mapLeft;   //���W�ɕϊ�
                    int zp = -z + mapForward;   //�����W�͉�����̂�-��

                    if (Collision::IntersectRectSphere(this->GetPosition(), this->GetRadius(), { (float)xp,(float)y,(float)zp }, outPosition))//�����蔻��
                    {
                        this->SetPosition(outPosition);
                    }
                }
                else
                {
                    if (!StageMain::Instance().map[y][z][x].name)continue;   //�u���b�N�Ȃ画��
                    if (Collision::IntersectRectSphere(this->GetPosition(), this->GetRadius(), StageMain::Instance().map[y][z][x].translate, outPosition))//�����蔻��
                    {
                        this->SetPosition(outPosition);
                    }
                }

            }
        }
    }

    //�X�e�[�W�͈͊O
    if (this->GetPosition().x > mapLeft)this->SetPosition({ (float)mapLeft ,this->GetPosition().y,this->GetPosition().z });
    if (this->GetPosition().x < -mapLeft)this->SetPosition({ (float)-mapLeft ,this->GetPosition().y,this->GetPosition().z });
    if (this->GetPosition().z > mapForward)this->SetPosition({ this->GetPosition().x ,this->GetPosition().y,(float)mapForward });
    if (this->GetPosition().z < -mapForward)this->SetPosition({ this->GetPosition().x ,this->GetPosition().y,(float)-mapForward });
}

//�`��X�V
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);

}

//�f�o�b�O�v���~�e�B�u�`��
void Player::DrawDebugPrimitive()
{
    if (!VisibleDebugPrimitive)return;
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //�Փ˔���p�̃f�o�b�O����`��
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));  
    //�Փ˔���p�̃f�o�b�O�~����`��
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));  
    
}

//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
    //ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    //if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("VisibleDebugPrimitive", &VisibleDebugPrimitive);
            //�ʒu
            ImGui::InputFloat3("Position", &position.x);
            //��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //�X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);
            //speed
            ImGui::InputFloat3("velocity", &velocity.x);
            ImGui::SliderFloat("MoveSpeed", &moveSpeed, 0, 100);
            ImGui::SliderFloat("TurnSpeed", &turnSpeed, 0, 100);
            ImGui::InputFloat("Radius", &radius);
            ImGui::InputFloat("Height", &height);
            ImGui::InputFloat("Friction", &friction);
            ImGui::InputFloat("Acceleration", &acceleration);
            ImGui::InputFloat("AirControl", &airControl);
    }
    //ImGui::End();
}
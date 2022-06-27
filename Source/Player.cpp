#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraite.h"
#include "ProjectileHoming.h"

//�R���X�g���N�^
Player::Player()
{
    model = new Model("Data/Model/Jammo/Jammo.mdl");

    //���f�����傫���̂ŃX�P�[�����O
    scale.x = scale.y = scale.z = 0.01f;

    //�q�b�g�G�t�F�N�g�ǂݍ���
    hitEffect = new Effect("Data/Effect/Hit.efk");
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

    //�J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    //�ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

    //�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    //�J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        //�P�ʃx�N�g����
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    //�X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
    //�X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
    //�i�s�x�N�g�����v�Z����
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    //Y�������ɂ͈ړ����Ȃ�
    vec.y = 0.0f;

    return vec;
}

//�X�V����
void Player::Update(float elapsedTime)
{
    //�ړ����͏���
    InputMove(elapsedTime);

    //�W�����v���͏���
    InputJump();
    
    //�e�ۓ��͏���
    InputProjectile();

    //���͏����X�V
    UpdateVelocity(elapsedTime);

    //�e�ۍX�V����
    projectileManager.Update(elapsedTime);

    //�v���C���[�ƓG�Ƃ̏Փˏ���
    CollisionPlayerVsEnemies();

    //�e�ۂƓG�̏Փ˔���
    CollisionProjectileVsEnemies();

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


//�e�ۓ��͏���
void Player::InputProjectile()
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    //���i�e�۔���
    if (gamePad.GetButtonDown() & GamePad::BTN_X|| gamePad.GetButtonDown() & GamePad::BTN_Y)    //C:�܂������AV:�ǔ�
    {
        //�O����
        DirectX::XMFLOAT3 dir;  //Z������transform������
        dir.x = transform._31;
        dir.y = transform._32;
        dir.z = transform._33;
        //���ˈʒu�i�v���C���[�̍�������j
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + GetHeight() / 2;
        pos.z = position.z;
        if (gamePad.GetButtonDown() & GamePad::BTN_Y) {
            //�^�[�Q�b�g(�f�t�H���g�ł̓v���C���[�̑O��
            DirectX::XMFLOAT3 target;
            target.x = position.x * 2;
            target.y = position.y + GetHeight() / 2 * 2;
            target.z = position.z * 2;

            //��ԋ߂��̓G���^�[�Q�b�g�ɂ���
            EnemyManager& enemyManager = EnemyManager::Instance();
            int enemyCount = enemyManager.GetEnemyCount();  //�G�̐��������Ă���
            if (enemyCount > 0) //�G�������ꍇ
            {
                for (int e = 0; e < enemyCount; ++e)    //�G�̐��������肩����
                {
                    if (e == 0) //�P�̖ڂ����^�[�Q�b�g�ɓ���Ė߂�
                    {
                        target = enemyManager.GetEnemy(e)->GetPosition();    //target�ɓ����
                        continue;
                    }

                    //�G�̃x�N�g���擾
                    DirectX::XMVECTOR enemy0 = DirectX::XMLoadFloat3(&target);
                    //���̓G�̃x�N�g���擾
                    DirectX::XMVECTOR enemy = DirectX::XMLoadFloat3(&enemyManager.GetEnemy(e)->GetPosition());
                    //�L�����̃x�N�g���擾
                    DirectX::XMVECTOR ziki = DirectX::XMLoadFloat3(&pos);
                    //�傫���𑪂�
                    DirectX::XMVECTOR E0Z = DirectX::XMVectorSubtract(enemy0, ziki);
                    E0Z = DirectX::XMVector3Length(E0Z);
                    float length0;
                    DirectX::XMStoreFloat(&length0, E0Z);
                    DirectX::XMVECTOR EZ = DirectX::XMVectorSubtract(enemy, ziki);
                    EZ = DirectX::XMVector3Length(EZ);
                    float length;
                    DirectX::XMStoreFloat(&length, EZ);

                    //�߂������^�[�Q�b�g�ɂ���
                    if (length < length0)target = enemyManager.GetEnemy(e)->GetPosition();

                }
            }
        //����
        ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
        projectile->Launch(dir, pos,target);
        }
        else
        {
            //����
            ProjectileStraite* projectile = new ProjectileStraite(&projectileManager);
            projectile->Launch(dir, pos);
        }
        ////projectileManager.Register(projectile);
    }
}

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

//�e�ۂƓG�̏Փˏ���
void Player::CollisionProjectileVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    //�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProjectile(i);
        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);
            //�Փˏ���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSphereVsCylider(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                //�_���[�W��^����
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    //������΂�
                    {
                        DirectX::XMFLOAT3 impulse;

                        DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&projectile->GetPosition());
                        DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&enemy->GetPosition());
                        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);  //�����Z�ŕ��������߂�a->b
                        Vec = DirectX::XMVector3Normalize(Vec);
                        Vec = DirectX::XMVectorScale(Vec, 10.0f);   //���ƓG�̋����𐁂���΂�
                        DirectX::XMStoreFloat3(&impulse, Vec);

                        impulse.y = 3.0f;//�����W�����v
                        enemy->AddImpulse(impulse);
                    }

                        //�q�b�g�G�t�F�N�g�Đ�
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }

                    //�e�۔j��
                    projectile->Destroy();
                }
            }
        }
    }
}

//�`��X�V
void Player::Render(ID3D11DeviceContext* dc,RenderContext rc, ModelShader* shader)
{
    shader->Draw(dc, rc, model);
    //�e�ە`�揈��
    projectileManager.Render(dc, shader);
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
    
    //�e�ۃf�o�b�O�v���~�e�B�u�`��
    projectileManager.DrawDebugPrimitive();
}

//�f�o�b�O�pGUI�`��
void Player::DrawDebugGUI()
{
    const ModelResource* resource = model->GetResource();
    const std::vector<Model::Node>& nodes = model->GetNodes();

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
            if (ImGui::TreeNode("Material"))
            {
                for (const ModelResource::Mesh& mesh : resource->GetMeshes())
                {
                    for (const ModelResource::Subset& subset : mesh.subsets)
                    {
                        ImGui::SliderFloat("smooth", &subset.material->pbr.adjustSmoothness, -1.0f, 1.0f);
                        ImGui::SliderFloat("Metalic", &subset.material->pbr.adjustMetalness, -1.0f, 1.0f);
                    }
                }
                ImGui::TreePop();
            }
        
    }
    //ImGui::End();
}
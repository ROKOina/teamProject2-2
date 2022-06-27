#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Camera.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "StageMain.h"

//コンストラクタ
Player::Player()
{
    model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    //モデルが大きいのでスケーリング
    scale.x = scale.y = scale.z = 0.01f;
    position.z -= 3;    //kari
    this->radius = 0.3f;

    //ヒットエフェクト読み込み
    //hitEffect = new Effect("Data/Effect/Hit.efk");
}

//デストラクタ
Player::~Player()
{
    delete hitEffect;
    delete model;
}


//スティック入力値
DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    //入力情報を取得
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    ////カメラ方向とスティックの入力値によって進行方向を計算する
    //Camera& camera = Camera::Instance();
    //const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    //const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    ////移動ベクトルはXZ平面に水平なベクトルになるようにする

    ////カメラ右方向ベクトルをXZ単位ベクトルに変換
    //float cameraRightX = cameraRight.x;
    //float cameraRightZ = cameraRight.z;
    //float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    //if (cameraRightLength > 0.0f)
    //{
    //    //単位ベクトル化
    //    cameraRightX /= cameraRightLength;
    //    cameraRightZ /= cameraRightLength;
    //}

    ////カメラ前方向ベクトルをXZ単位ベクトルに変換
    //float cameraFrontX = cameraFront.x;
    //float cameraFrontZ = cameraFront.z;
    //float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    //if (cameraFrontLength > 0.0f)
    //{
    //    //単位ベクトル化
    //    cameraFrontX /= cameraFrontLength;
    //    cameraFrontZ /= cameraFrontLength;
    //}

   

    //スティックの水平入力値をカメラ右方向に反映し、
    //スティックの垂直入力値をカメラ前方向に反映し、
    //進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x =  ax;
    vec.z =  ay ;
    //vec.x = cameraFrontX * ay + cameraRightX * ax;
    //vec.z = cameraFrontZ * ay + cameraRightZ * ax;
    //Y軸方向には移動しない
    vec.y = 0.0f;

    return vec;
}

//更新処理
void Player::Update(float elapsedTime)
{
    DirectX::XMFLOAT3 oldPlayerPos = this->GetPosition();

    //移動入力処理
    InputMove(elapsedTime);

    //ジャンプ入力処理
    //InputJump();
    
    //弾丸入力処理
    //InputProjectile();

    //速力処理更新
    UpdateVelocity(elapsedTime);

    //弾丸更新処理
    //projectileManager.Update(elapsedTime);

    //プレイヤーと敵との衝突処理
    //CollisionPlayerVsEnemies();

    //弾丸と敵の衝突判定
    //CollisionProjectileVsEnemies();

    //プレイヤーとstageの当たり判定
    CollisionStage(oldPlayerPos);

    //オブジェクトの行列を更新
    UpdateTransform();

    //モデル行列更新
    model->UpdateTransform(transform);  //位置行列を渡す
}


void Player::InputMove(float elapsedTime)
{
    //進行ベクトル取得
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    //移動処理
    Move(moveVec.x, moveVec.z, moveSpeed);

    //旋回処理
    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

}

//ジャンプ入力処理
void Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (gamePad.GetButtonDown() & GamePad::BTN_A) 
    {
        if (JumpCount < JumpLimit)  //ジャンプ回数制限
        {
            Jump(jumpSpeed);
            ++JumpCount;
        }
    }
}

//着地した時に呼ばれる
void Player::OnLanding()
{
    JumpCount = 0;
}

////ジャンプ処理
//void Player::Jump(float speed)
//{
//    //上方向の力を設定
//    velocity.y = speed;
//}
//
////速力処理更新
//void Player::UpdateVelocity(float elapsedTime)
//{
//    //経過フレーム
//    float elapsedFrame = 60.0f * elapsedTime;
//
//    //重力処理
//    velocity.y += gravity * elapsedFrame;
//
//    //移動処理
//    position.y += velocity.y * elapsedTime;
//
//    //地面判定
//    if (position.y < 0.0f)
//    {
//        position.y = 0.0f;
//        velocity.y = 0.0f;
//    }
//}
//
////旋回処理
//void Player::Turn(float elapsedTime, float vx, float vz, float speed)
//{
//    speed *= elapsedTime;
//
//
//    //進行ベクトルを単位ベクトル化
//    float moveVecLength = sqrtf(vx * vx + vz * vz);
//    if (moveVecLength <= 0)return;  //進行ベクトルがゼロベクトルの場合は処理する必要なし
//    if (moveVecLength > 0.0f)
//    {
//        //単位ベクトル化
//        vx /= moveVecLength;
//        vz /= moveVecLength;
//    }
//
//    //自身の回転値から前方向を決める
//    float frontX = sinf(angle.y);
//    float frontZ = cosf(angle.y);
//
//    //回転角を求めるため、２つの単位ベクトルの内積を計算する
//    float dot = (vx * frontX) + (vz * frontZ);
//
//    //内積値は-1.0~1.0で表現されており、２つの単位ベクトルの角度が
//    //小さいほどに1.0に近づくという性質を利用して回転速度を調整する
//    float rot = 1.0f - dot; //補正量
//    if (rot > speed)rot = speed;
//
//    //左右判定を行うために2つの単位ベクトルの外積を計算する
//    float cross = (vx * frontZ) - (vz * frontX);
//
//    //2Dの外積値が正の場合か負の場合によって左右判定が行える
//    //左右判定を行うことによって左右回転を選択する
//    if (cross < 0.0f)   //右
//    {
//        angle.y -= rot;
//    }
//    else
//    {
//        angle.y += rot;
//    }
//
//}


//プレイヤーとの当たり判定
void Player::CollisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    //全ての数と総当たりで衝突処理
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        //衝突処理
        DirectX::XMFLOAT3 outPosition;
        //if (Collision::IntersectSphereVsSphere(   //円
        //    this->GetPosition(), this->GetRadius(),
        //    enemy->GetPosition(), enemy->GetRadius(),
        //    outPosition))
        //{
        //    //押し出し後の位置設定
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

            //押し出し後の位置設定
            if(DirectX::XMVectorGetY(N)>0.8f) Jump(10);
            /*if(IsGround())*/
            else enemy->SetPosition(outPosition);
        }
    }
}

//プレイヤーとstageの当たり判定
void Player::CollisionStage(DirectX::XMFLOAT3 oldPlayerPos)
{

    //ステージ範囲内
    for (int y = 0; y < MAP_Y; ++y) 
    {
        for (int z = 0; z < MAP_Z; ++z) //平面で回す
        {
            for (int x = 0; x < MAP_X; ++x)
            {
                    DirectX::XMFLOAT3 outPosition;
                if (y == 0) {
                    if (StageMain::Instance().map[y][z][x].name)continue;   //noneブロックなら判定
                    outPosition = oldPlayerPos; //１フレーム分前のpositionを代入

                    int xp = x - mapLeft;   //座標に変換
                    int zp = -z + mapForward;   //ｚ座標は下がるので-ｚ

                    if (Collision::IntersectRectSphere(this->GetPosition(), this->GetRadius(), { (float)xp,(float)y,(float)zp }, outPosition))//当たり判定
                    {
                        this->SetPosition(outPosition);
                    }
                }
                else
                {
                    if (!StageMain::Instance().map[y][z][x].name)continue;   //ブロックなら判定
                    if (Collision::IntersectRectSphere(this->GetPosition(), this->GetRadius(), StageMain::Instance().map[y][z][x].translate, outPosition))//当たり判定
                    {
                        this->SetPosition(outPosition);
                    }
                }

            }
        }
    }

    //ステージ範囲外
    if (this->GetPosition().x > mapLeft)this->SetPosition({ (float)mapLeft ,this->GetPosition().y,this->GetPosition().z });
    if (this->GetPosition().x < -mapLeft)this->SetPosition({ (float)-mapLeft ,this->GetPosition().y,this->GetPosition().z });
    if (this->GetPosition().z > mapForward)this->SetPosition({ this->GetPosition().x ,this->GetPosition().y,(float)mapForward });
    if (this->GetPosition().z < -mapForward)this->SetPosition({ this->GetPosition().x ,this->GetPosition().y,(float)-mapForward });
}

//描画更新
void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);

}

//デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
    if (!VisibleDebugPrimitive)return;
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //衝突判定用のデバッグ球を描画
    //debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));  
    //衝突判定用のデバッグ円柱を描画
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));  
    
}

//デバッグ用GUI描画
void Player::DrawDebugGUI()
{
    //ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    //if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("VisibleDebugPrimitive", &VisibleDebugPrimitive);
            //位置
            ImGui::InputFloat3("Position", &position.x);
            //回転
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);
            //スケール
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
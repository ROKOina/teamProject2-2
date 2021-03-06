#pragma once

#include "Graphics/shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

//プレイヤー
class Player : public Character
{
public: //関数
    Player();
    ~Player() override;

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(ID3D11DeviceContext* dc,RenderContext rc, ModelShader* shader);

    //プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();

    //弾丸と敵の衝突処理
    void CollisionProjectileVsEnemies();

    //デバッグ用GUI描画
    void DrawDebugGUI();

    //デバッグプリミティブ描画
    void DrawDebugPrimitive();

    ////ジャンプ処理
    //void Jump(float speed);

    ////速力処理更新
    //void UpdateVelocity(float elapsedTime);

    //ジャンプ入力処理
    void InputJump();

    //モデルゲッター
    Model* GetModel()const { return model; }

private:    //関数
    //スティック入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 GetMoveVec() const;

    //移動入力処理
    void InputMove(float elapsedTime);

    //弾丸入力処理
    void InputProjectile();

    ////旋回処理
    //void Turn(float elapsedTime, float vx, float vz, float speed);

    
protected:
    //着地した時に呼ばれる
    void OnLanding()override;

private:    //変数
    Model* model = nullptr;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);

    //ジャンプ
    float jumpSpeed = 20.0f;
    //float gravity = -1.0f;
    //DirectX::XMFLOAT3 velocity = { 0,0,0 }; //速力

    int JumpCount = 0;
    int JumpLimit = 2;

    ProjectileManager projectileManager;

    //ImGui
    bool VisibleDebugPrimitive = true;

    //エフェクト
    Effect* hitEffect = nullptr;
};
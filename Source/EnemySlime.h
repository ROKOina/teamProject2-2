#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

//スライム
class EnemySlime :public Enemy
{
public:
    EnemySlime();
    ~EnemySlime() override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画処理
    void Render(ID3D11DeviceContext* dc, ModelShader* shader)override;

    void GuiEnemy()override;

protected:
    //死亡した時に呼ばれる
    void OnDead()override;

private:
    Model* model = nullptr;
};
#include "ProjectileStraite.h"

//コンストラクタ
ProjectileStraite::ProjectileStraite(ProjectileManager* manager)
    :Projectile(manager)    //基底クラスのコンストラクタを呼び出す
{
    model = new Model("Data/Model/Sword/Sword.mdl");
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");

    //表示サイズを調整
    scale.x = scale.y = scale.z = 3.0f;
    //scale.x = scale.y = scale.z = 0.5f;
}

ProjectileStraite::~ProjectileStraite()
{
    delete model;
}

//更新処理
void ProjectileStraite::Update(float elapsedTime)
{
    //寿命処理
    lifeTimer -= elapsedTime;
    if (lifeTimer < 0)
    {
        //自分を削除
        Destroy();
    }

    //移動
    float speed = this->speed * elapsedTime;
    //direction(向きベクトルを使って）方向を決める
    position.x += direction.x *  speed;
    position.y += direction.y *  speed;
    position.z += direction.z *  speed;
    
    //オブジェクト行列を更新
    UpdateTransform();

    //モデル行列更新
    model->UpdateTransform(transform);
}

//描画処理
void ProjectileStraite::Render(ID3D11DeviceContext* dc, ModelShader* shader)
{
    RenderContext rc = {};
    shader->Draw(dc, rc, model);
}

//発射
void ProjectileStraite::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}
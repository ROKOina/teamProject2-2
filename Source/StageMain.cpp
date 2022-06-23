#include "StageMain.h"

static StageMain* instance = nullptr;

DirectX::XMFLOAT4X4 transform = {
1, 0, 0, 0,
0, 1, 0, 0,
0, 0, 1, 0,
0, 0, 0, 1
};

//インスタンス取得
StageMain& StageMain::Instance()
{
    return *instance;
}

//コンストラクタ
StageMain::StageMain()
{
    instance = this;

    //ステージモデルを読み込み
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/Jammo/Jammo.mdl");
    model = new Model("Data/Model/blockMap1/map1.mdl");
    //model = new Model("Data/Model/blockMap/blockMap.mdl");
    
    model->nodeSearch("Button_Platform_01_Red", map);   //mapにノードを入れる(char型の引数使わない)
}

StageMain::~StageMain()
{
    //ステージモデルを破棄
    delete model;
}

void StageMain::UpdateTransform()
{
    //スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    //回転行列を作成
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&model->GetNodes().at(0).rotate));
    //位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(model->GetNodes().at(0).translate.x, model->GetNodes().at(0).translate.y, model->GetNodes().at(0).translate.z);
    //3つの行列を組み合わせて、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    //計算したワールド行列を取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}


#include "Input/Input.h"
//更新処理
void StageMain::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {   //かりうごかし
        map[0][0][0].translate.x += 1;
        model->GetNodes()[map[0][0][0].Index]= map[0][0][0];
    }

    model->UpdateTransform(transform);
}

//レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}


//描画処理
void StageMain::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    //シェーダーにモデルを描画してもらう
    shader->Draw(dc, model);
}

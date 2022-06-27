#include "StageMain.h"

#include "Camera.h"
#include "imgui.h"

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

void Stage::DrawDebugGUI()
{
    const ModelResource* resource = model->GetResource();
    const std::vector<Model::Node>& nodes = model->GetNodes();
    if (ImGui::CollapsingHeader("Stage", ImGuiTreeNodeFlags_DefaultOpen))
    {
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
   
}
Model::Node Barel;

//コンストラクタ
StageMain::StageMain()
{
    instance = this;

    //ステージモデルを読み込み
    //model = new Model("Data/Model/ExampleStage/ExampleStage.mdl");
    //model = new Model("Data/Model/blockMap1/map1.mdl");
    model = new Model("Data/Model/blockMap1/map2/map2.mdl");
    //model = new Model("Data/Model/blockMap/blockMap.mdl");
    
    model->nodeSearch("map2", map,&mapBottom);   //mapにノードを入れる(mapのおやを入れる)
    map[1][2][6] = {};  //仮でitemを消す
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


    model->UpdateTransform(transform);

}

Model::Node* StageMain::SearchMap(int index)
{
    for (int y = 0; y < MAP_Y; ++y)
    {
        for (int z = 0; z < MAP_Z; ++z)
        {
            for (int x = 0; x < MAP_X; ++x)
            {
                if (map[y][z][x].Index == index)
                    return &map[y][z][x];
            }
        }
    }
    return {};
}

void StageMain::MoveBlock(Model::Node* block,DirectX::XMFLOAT3 move)
{
    //かりうごかし
    if (block->name) {
        Model::Node* Block = SearchMap(block->Index);
        Block->translate = { Block->translate.x + move.x,Block->translate.y + move.y,Block->translate.z + move.z };
        model->GetNodes()[Block->Index] = *Block;   //動かしたらnodeに上書き
        BlockDropJudge(Block);
    }
}

void StageMain::BlockDropJudge(Model::Node* mapB)
{
    //仮落とし
    if (bottomSearch(mapB)) //ステージの下判定(y0)
    {
        mapB->translate.y -= 1; //ステージがない場合落とす
        model->GetNodes()[mapB->Index] = *mapB;
    }

    GravityBlock(mapB); //y-1より下判定
}

void StageMain::GravityBlock(Model::Node* mapB)
{
    int mx = mapB->translate.x;
    int mz = mapB->translate.z;
    int my = mapB->translate.y - 1;
    for (Model::Node mapNode : mapBottom)  //マップy0より下をさがす
    {
        if (mapNode.translate.y != my)continue;    //一つ下を探す
        if (mapNode.translate.x == mx && mapNode.translate.z == mz)
        {
            mx = mapB->translate.x + mapLeft;
            mz = mapB->translate.z - mapForward;
            my = mapB->translate.y;
            map[my][-mz][mx] = *mapB;
            *mapB = {};
            model->GetNodes()[map[my][-mz][mx].Index] = map[my][-mz][mx];
            model->UpdateTransform(transform);

        }
    }
}

bool StageMain::bottomSearch(Model::Node* mapB)
{
    int mx = mapB->translate.x+mapLeft;
    int mz = mapB->translate.z-mapForward;
    int my = mapB->translate.y - 1;  //一つ下を指定
    //マップ-yを探す
    if (my >= 0) {
        if (map[my][-mz][mx].name)return false;
    }

    return true;    //何もない場合
}

//レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::IntersectRayVsModel(start, end, model, hit);
}


//描画処理
void Stage::Render(ID3D11DeviceContext* dc,RenderContext rc, ModelShader* shader)
{
    //シェーダーにモデルを描画してもらう
    shader->Draw(dc, rc, model);
}

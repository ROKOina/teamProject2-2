#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision.h"
#include <memory>
#include <vector>
using namespace std;

//マップの大きさ（マップチップ）
#define  MAP_X (13)
#define  MAP_Z (7)
#define  MAP_Y (2)
#define mapLeft  ((MAP_X - 1) / 2)  //ステージ半分の値
#define mapForward  ((MAP_Z - 1) / 2)


//ステージ
class StageMain
{
public:
    StageMain();
    ~StageMain();

    //更新処理
    void Update(float elapsedTime);

    void MoveBlock(Model::Node* block, DirectX::XMFLOAT3 move);   //ブロック動かす


    void BlockDropJudge(Model::Node* map);  //下にブロックが無い場合落とす

    Model::Node* SearchMap(int index);   //Indexからnodeを探す


    //描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    void UpdateTransform();

    //レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //インスタンス取得
    static StageMain& Instance();

private:
    bool bottomSearch(Model::Node* map);    //下ブロックにブロックが無い場合true
    void GravityBlock(Model::Node* mapB);   //落ちたブロックの下にブロックがあれば代入する

public:
    //Y,Z,Xなので注意！！！！！！！
    Model::Node map[MAP_Y][MAP_Z][MAP_X] = {};
    vector<Model::Node> mapBottom;

private:
    //vector<vector<Model::Node>> map[MAP_X][MAP_Y]; //マップチップ情報入れ物
    
    Model* model = nullptr;


};
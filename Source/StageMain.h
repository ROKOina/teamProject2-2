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

//ステージ
class StageMain
{
public:
    StageMain();
    ~StageMain();

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(ID3D11DeviceContext* dc, Shader* shader);

    void UpdateTransform();


    //レイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    //インスタンス取得
    static StageMain& Instance();

private:
    //vector<vector<Model::Node>> map[MAP_X][MAP_Y]; //マップチップ情報入れ物
    
    //Y,Z,Xなので注意！！！！！！！
    Model::Node map[MAP_Y][MAP_Z][MAP_X] = {};
    Model* model = nullptr;


};
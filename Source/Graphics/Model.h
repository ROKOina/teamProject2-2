#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics/ModelResource.h"

//マップの大きさ（マップチップ）
#define  MAP_X (13)
#define  MAP_Z (7)
#define  MAP_Y (2)

// モデル
class Model
{
public:
	Model(const char* filename);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		DirectX::XMFLOAT3	scale;
		DirectX::XMFLOAT4	rotate;
		DirectX::XMFLOAT3	translate;
		DirectX::XMFLOAT4X4	localTransform;
		DirectX::XMFLOAT4X4	worldTransform;

		std::vector<Node*>	children;

		int Index;
	};

	// 行列計算
	void UpdateTransform(const DirectX::XMFLOAT4X4& transform);

	// ノードリスト取得
	const std::vector<Node>& GetNodes() const { return nodes; }
	std::vector<Node>& GetNodes() { return nodes; }

	// リソース取得
	const ModelResource* GetResource() const { return resource.get(); }

	//ノード名前検索
	bool nodeSearch(const char* parentName, Model::Node nodeRe[MAP_Y][MAP_Z][MAP_X], std::vector<Node>* bottom);



private:
	std::shared_ptr<ModelResource>	resource;
	std::vector<Node>				nodes;
};

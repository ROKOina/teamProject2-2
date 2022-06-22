#include "Graphics/Graphics.h"
#include "Graphics/Model.h"

// コンストラクタ
Model::Model(const char* filename)
{
	// リソース読み込み
	resource = std::make_shared<ModelResource>();
	resource->Load(Graphics::Instance().GetDevice(), filename);

	// ノード
	const std::vector<ModelResource::Node>& resNodes = resource->GetNodes();

	nodes.resize(resNodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = resNodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
		dst.Index = nodeIndex;

		if (dst.parent != nullptr)
		{
			dst.parent->children.emplace_back(&dst);
		}
	}

	// 行列計算
	const DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	UpdateTransform(transform);
}

// 変換行列計算
void Model::UpdateTransform(const DirectX::XMFLOAT4X4& transform)
{
	DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);

	for (Node& node : nodes)
	{
		// ローカル行列算出
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(node.scale.x, node.scale.y , node.scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&node.rotate));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);
		DirectX::XMMATRIX LocalTransform = S * R * T;

		// ワールド行列算出
		DirectX::XMMATRIX ParentTransform;
		if (node.parent != nullptr)
		{
			ParentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
		}
		else
		{
			ParentTransform = Transform;
		}
		DirectX::XMMATRIX WorldTransform = LocalTransform * ParentTransform;

		// 計算結果を格納
		DirectX::XMStoreFloat4x4(&node.localTransform, LocalTransform);
		DirectX::XMStoreFloat4x4(&node.worldTransform, WorldTransform);
	}
}

//ノード名前検索
bool Model::nodeSearch(const char* nodeName, Model::Node map[MAP_Y][MAP_Z][MAP_X])
{
	for (const Node node : nodes)	//ノードを検索
	{
		//mapにnodeを左上カラ入れていく
		//原点が中心なので、その分ずらす
		int mapLeft = (MAP_X - 1) / 2;
		int mapForward = (MAP_Z - 1) / 2;
		//nodeの座標をmap基準にする（左上を原点に
		int x = (int)node.translate.x + mapLeft;
		int z = (int)node.translate.z - mapForward;

		//座標でmapに入れていく
		if ((int)node.translate.y != 0)continue;	//とりあえず高さが0以外ははじく
		if (x < 0 && x > MAP_X && z > 0 && z < -MAP_Z)continue;	//範囲外ははじく

		map[0][-z][x] = node;

		//if (strcmp(node.name, nodeName) == 0)	//同じ名前を取得
		//{
		//	map[0][0][0] = node;

		//	return true;
		//}

		//return true;
	}
	return false;
}


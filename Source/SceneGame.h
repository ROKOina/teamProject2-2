#pragma once

#include "StageMain.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics\Sprite.h"

#include "Renderer/ShadowMapRenderer.h"
#include "Renderer/SceneRenderer.h"

// ゲームシーン
class SceneGame :public Scene
{
public:
	SceneGame() {}
	~SceneGame()override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

private:
	//エネミーHPゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	Sprite* gauge = nullptr;

	//レンダラー
	std::unique_ptr<ShadowMapRenderer>   shadowmapRenderer;
	std::unique_ptr<SceneRenderer>       sceneRenderer;

};

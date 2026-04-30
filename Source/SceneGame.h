#pragma once

#include"Stage.h"
#include"StageGrid.h"
#include"Laser.h"
#include"LaserTexture.h"
#include"CameraController.h"
#include"Scene.h"
#include"Player.h"
// ゲームシーン
class SceneGame:public Scene
{
public:
	SceneGame(){}
	~SceneGame()override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsedTime)override;

	// 描画処理
	void Render()override;

	// GUI描画
	void DrawGUI()override;

private:
	Stage* stage = nullptr;
	StageGrid* stageGrid = nullptr;
	CameraController* cameraController = nullptr;

	Player* players[2] = {};
	int controlPlayerIndex = 0;

	void InputChangePlayer();
	Player* GetControlPlayer();
};
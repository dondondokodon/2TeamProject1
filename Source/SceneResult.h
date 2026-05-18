#pragma once
#include "Scene.h"
#include "Sprite2D.h"
#include "Button.h"
#include "fade.h"
#include "System/Model.h"
#include "AnimationController.h"
#include <memory>

class SceneResult:public Scene
{
public:
	SceneResult();
	~SceneResult();

	//初期化
	void Initialize()override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

	//GUI描画
	void DrawGUI()override;

private:
	Sprite2D sprites[2];
	Button nextSceneButton;
	Sprite2D back;
	Fade fade;
	std::unique_ptr<Model> resultPlayerModel = nullptr;
	AnimationController resultPlayerAnimation;
	bool changeScene;
	bool isClear;
};


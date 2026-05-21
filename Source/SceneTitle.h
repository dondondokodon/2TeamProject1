#pragma once
#include"System/Sprite.h"
#include "Scene.h"
#include "Button.h"
#include "Fade.h"
#include "Sprite2D.h"
#include "System/Model.h"
#include "AnimationController.h"
#include "Effect.h"
#include <memory>

class SceneTitle :public Scene
{
public:
	SceneTitle();
	~SceneTitle()override{}

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
	Sprite* sprite = nullptr;
	Button buttons[2];
	Sprite2D title;
	Sprite2D nowChoiceButton[2];
	std::unique_ptr<Model> titlePlayerModel;
	AnimationController titlePlayerAnimation;
	std::unique_ptr<Effect> titleLaserEffect;
	Effekseer::Handle titleLaserHandle = -1;
	short ButtonIndex = 0;

	Fade fade;

	bool changeScene = false;

	int num=0;	//点滅用
	int nextSceneIndex=0;	//適当
};


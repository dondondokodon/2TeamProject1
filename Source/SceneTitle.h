#pragma once
#include"System/Sprite.h"
#include "Scene.h"
#include "Button.h"
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
	Button buttons[3];
	short ButtonIndex = 1;
};


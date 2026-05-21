#pragma once
#include "Scene.h"

#include "Sprite2D.h"
#include "Button.h"
#include "Fade.h"


class SceneStageSelect :
    public Scene
{
public:
	SceneStageSelect();
	~SceneStageSelect() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void Finalize() override;
	void DrawGUI()override;

private:
	Sprite2D back;
	Button buttons[4];
	Sprite2D selectButtons[4];
	bool changeScene = false;
	short ButtonIndex = 0;
	Fade fade;

	float prevAx = 0.0f;
	float prevAy = 0.0f;

	int num = 0;
};


#pragma once
#include "Scene.h"

#include "Sprite2D.h"
#include "Button.h"
#include "Fade.h"


class SceneStageSelect :
    public Scene
{
public:
	static constexpr int STAGE_COUNT = 7;

	SceneStageSelect();
	~SceneStageSelect() {}
	void Initialize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void Finalize() override;
	void DrawGUI()override;

private:
	void RenderStageNumber(const RenderContext& rc, int number, float x, float y);
	DirectX::XMFLOAT2 GetStageCenter(int index) const;

	Sprite2D back;
	Button buttons[STAGE_COUNT];
	Sprite2D selectButtons[STAGE_COUNT];
	bool changeScene = false;
	short ButtonIndex = 0;
	Fade fade;
	bool nextSceneTitle = false;

	std::unique_ptr<Sprite> checkMarks[STAGE_COUNT];
	std::unique_ptr<Sprite> stageNumberSprite;

	float prevAx = 0.0f;
	float prevAy = 0.0f;

	int num = 0;
};

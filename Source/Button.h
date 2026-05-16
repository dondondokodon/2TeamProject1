#pragma once
#include"System/Sprite.h"
#include<memory>
#include<DirectXMath.h>
#include "Sprite2D.h"


class Button
{
public:
	Button();
	~Button();

	void Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height);

	void Update(float elapsedTime);

	void render(const RenderContext& rc);

	void OnClick();

	void setStageIndex(int index) { stageIndex = index; }

private:
	Sprite2D sprite;

	 int stageIndex=0;//これ基底クラスにして派生クラスで個別に設定するほうがいいけどめんどいからこれにする
};


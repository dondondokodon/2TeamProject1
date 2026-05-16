#pragma once
#include"System/Sprite.h"
#include<memory>
#include<DirectXMath.h>


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
	 std::unique_ptr<Sprite>sprite;
	 DirectX::XMFLOAT2 position;
	 DirectX::XMFLOAT2 offset;
	 float width;
	 float height;

	 int stageIndex=0;//これ基底クラスにして派生クラスで個別に設定するほうがいいけどめんどいからこれにする
};


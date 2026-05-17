#pragma once
#include "Sprite2D.h"
class LoadSprite :
    public Sprite2D
{
public:
	LoadSprite(){}
	~LoadSprite(){}

	void Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height)override;

	void setRotateSpeed(float speed) { this->speed = speed; }

	void Update(float elapsedTime)override;

	void render(const RenderContext& rc)override;
private:
	float angle = 0.0f;
	float speed = 0.0f;
};



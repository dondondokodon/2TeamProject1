#pragma once
#include"System/Sprite.h"
#include<memory>

class Sprite2D
{
public:
	Sprite2D();

	Sprite2D(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height);

	~Sprite2D();

	virtual void Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height);

	virtual void Update(float elapsedTime);

	virtual void render(const RenderContext& rc);

protected:
	std::unique_ptr<Sprite>sprite;
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT2 offset;
	float width;
	float height;
};


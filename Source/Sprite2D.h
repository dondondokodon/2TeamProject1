#pragma once
#include"System/Sprite.h"
#include<memory>

class Sprite2D
{
public:
	Sprite2D();
	~Sprite2D();

	void Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height);

	void Update(float elapsedTime);

	void render(const RenderContext& rc);

private:
	std::unique_ptr<Sprite>sprite;
	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT2 offset;
	float width;
	float height;
};


#include "LoadSprite.h"

void LoadSprite::Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height)
{
	sprite = std::make_unique<Sprite>(filepath);
	this->position = position;
	this->width = width;
	this->height = height;
	offset.x = width / 2.0f;
	offset.y = height / 2.0f;
	this->position.x -= offset.x;
	this->position.y -= offset.y;

	//angle = 0.0f;
	speed = 0.0f;
}

void LoadSprite::Update(float elapsedTime)
{
	angle += speed * elapsedTime;

	// fmodf を使って、どんなに数値が大きくなっても 0度 〜 360度の間に綺麗に収める
	angle = fmodf(angle, 360.0f);
	if (angle < 0.0f)
	{
		angle += 360.0f;
	}
}

void LoadSprite::render(const RenderContext& rc)
{
	if(sprite)
		sprite->Render(rc, position.x, position.y, 0, width, height, angle, 1, 1, 1, 1);
}
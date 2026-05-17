#include "Sprite2D.h"

Sprite2D::Sprite2D() :sprite(nullptr)
{

}

Sprite2D::~Sprite2D() 
{

}

//真ん中基準点
void Sprite2D::Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height)
{
	sprite = std::make_unique<Sprite>(filepath);
	this->position = position;
	this->width = width;
	this->height = height;
	offset.x = width / 2.0f;
	offset.y = height / 2.0f;
	this->position.x -= offset.x;
	this->position.y -= offset.y;
}

void Sprite2D::Update(float elapsedTime)
{

}

void Sprite2D::render(const RenderContext& rc)
{
	if (sprite)
		sprite->Render(rc, position.x, position.y, 0, width, height, 0, 1, 1, 1, 1);
}
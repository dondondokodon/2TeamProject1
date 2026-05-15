#include "Button.h"
#include "StageObjectManager.h"

Button::Button():sprite(nullptr)
{
}
Button::~Button()
{

}

void Button::Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height)
{
	sprite = std::make_unique<Sprite>(filepath);
	this->position = position;
	this->width = width;
	this->height = height;
	offset.x=width/2.0f;
	offset.y=height/2.0f;
	this->position.x-=offset.x;
	this->position.y-=offset.y;
}


void Button::Update(float elapsedTime)
{

}

void Button::OnClick()
{
	StageObjectManager::Instance().setStageIndex(stageIndex);
}


void Button::render(const RenderContext& rc)
{
	if(sprite)
	sprite->Render(rc, position.x, position.y, 0, width, height, 1, 1, 1, 1, 1);
}
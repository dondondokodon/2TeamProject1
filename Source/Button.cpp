#include "Button.h"
#include "StageObjectManager.h"

Button::Button()
{
}
Button::~Button()
{

}

void Button::Initialize(const char* filepath, const DirectX::XMFLOAT2& position, float width, float height)
{
	sprite.Initialize(filepath, position, width, height);
}


void Button::Update(float elapsedTime)
{
	sprite.Update(elapsedTime);
}

void Button::OnClick()
{
	StageObjectManager::Instance().setStageIndex(stageIndex);
}


void Button::render(const RenderContext& rc)
{
	sprite.render(rc);
}
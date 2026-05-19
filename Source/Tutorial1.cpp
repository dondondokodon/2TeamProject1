#include "Tutorial1.h"
#include "ScreenSize.h"
#include"System/Graphics.h"
#include "Flag.h"
#include<imgui.h>


void Tutorial1::Initialize()
{
	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_01.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_02.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_03.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_04.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_05.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_06.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 600.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_07.png", DirectX::XMFLOAT2(SCREEN_W * 0.9f, SCREEN_H * 0.15f), 800.0f, 500.0f));

	index = 0;

	isEnd = false;
	isRender = true;

	renderIndex = 0;
}

void Tutorial1::Finalize()
{
	sprites.clear();
}

void Tutorial1::Update(float elapsedTime)
{
	if (isEnd) return;

	{
		buttonState[1] = buttonState[0];	// スイッチ履歴
		buttonState[0] = GetAsyncKeyState(VK_RETURN) & 0x8000;

		buttonDown = ~buttonState[1] & GetAsyncKeyState(VK_RETURN) & 0x8000;	// 押した瞬間
	}

	switch (index)
	{
	case 0:case 5:case 6: case 7:
		if(buttonDown)
		{
			index++;
			renderIndex++;
		}
		break;

	case 1:case 3:
		if (buttonDown)
		{
			index++;
			isRender = false;
		}
		break;

	case 2:
		if (Flag::Instance().getFlag(Flag::eventName::rotateLaser))
		{
			index++;
			renderIndex++;
			isRender = true;
		}
		break;

	case 4:
		if (Flag::Instance().getFlag(Flag::eventName::rotateMirror))
		{
			index++;
			renderIndex++;
			isRender = true;
		}
		break;

	default:
		break;
	}

	if (renderIndex >= sprites.size() - 1) isEnd = true;
}

void Tutorial1::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	if(isRender)
	sprites[renderIndex]->render(rc);
}
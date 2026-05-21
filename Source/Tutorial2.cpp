#include "Tutorial2.h"

#include "ScreenSize.h"
#include"System/Graphics.h"
#include<imgui.h>


void Tutorial2::Initialize()
{
	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_08.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.75f), 700.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_09.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.75f), 700.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial10.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.75f), 700.0f, 400.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_11.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.7f), 800.0f, 600.0f));

	sprites.push_back(std::make_unique<Sprite2D>("Data/Sprite/Tutorials/tutorial_12.png", DirectX::XMFLOAT2(SCREEN_W * 0.85f, SCREEN_H * 0.2f), 500.0f, 400.0f));

	index = 0;

	isEnd = false;
	isRender = true;

	renderIndex = 0;
}

void Tutorial2::Finalize()
{
	sprites.clear();
}

void Tutorial2::Update(float elapsedTime)
{
	if (isEnd) return;

	{
		buttonState[1] = buttonState[0];	// スイッチ履歴
		buttonState[0] = GetAsyncKeyState(VK_RETURN) & 0x8000;

		buttonDown = ~buttonState[1] & GetAsyncKeyState(VK_RETURN) & 0x8000;	// 押した瞬間
	}

	switch (index)
	{
	case 0:case 1: case 2:case 3:
		if (buttonDown)
		{
			index++;
			renderIndex++;
		}
		break;

	//case 1:case 3:
	//	if (buttonDown)
	//	{
	//		index++;
	//		isRender = false;
	//	}
	//	break;
	default:
		break;
	}

	if (renderIndex >= sprites.size() - 1) isEnd = true;
}

void Tutorial2::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	if (isRender)
		sprites[renderIndex]->render(rc);

	//if (ImGui::Begin("Tutorial", nullptr, ImGuiWindowFlags_None))
	//{
	//	//位置
	//	ImGui::InputInt("index", &index);
	//}
	//ImGui::End();
}
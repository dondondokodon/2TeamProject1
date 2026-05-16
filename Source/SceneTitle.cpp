#include"System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"

#include "ScreenSize.h"

SceneTitle::SceneTitle()
{
	ButtonIndex = 0;
}

//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");

	title.Initialize("Data/Sprite/titleName.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H *0.23f), 1200.0f, 240.0f);
	buttons[0].Initialize("Data/Sprite/Start.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H *0.59f), 290.0f, 120.0f);
	buttons[0].setStageIndex(0);
	buttons[1].Initialize("Data/Sprite/Tutorial.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 500.0f, 120.0f);
	buttons[1].setStageIndex(1);

	ButtonIndex = 0;
	changeScene = false;
}

//終了化
void SceneTitle::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;

	}
}

//更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();

	if (ay>0.2f)
	{
		ButtonIndex = 1;
	}
	else if(ay<-0.2f)
	{
		ButtonIndex = 0;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		buttons[ButtonIndex].OnClick();
		if (!changeScene)
		{
			fade.StartFadeOut(1.0f, 0.5f);
			changeScene = true;		
		}
		//SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}

	//何かボタンを押したらゲームシーンへ切り替え
	/*const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		| GamePad::BTN_UP
		| GamePad::BTN_RIGHT
		| GamePad::BTN_DOWN
		| GamePad::BTN_LEFT
		| GamePad::BTN_START
		| GamePad::BTN_BACK
		| GamePad::BTN_LEFT_THUMB
		| GamePad::BTN_RIGHT_THUMB
		| GamePad::BTN_LEFT_SHOULDER
		| GamePad::BTN_RIGHT_SHOULDER
		| GamePad::BTN_LEFT_TRIGGER
		| GamePad::BTN_RIGHT_TRIGGER;
	if (gamePad.GetButtonDown() & anyButton)
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));*/

	for (auto& button : buttons)
	{
		button.Update(elapsedTime);
	}

	title.Update(elapsedTime);

	fade.Update(elapsedTime);
	
	if (!fade.IsFading()&&changeScene)
	{
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneGame)
		);
	}
	
}

//描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	//2Dスプライト描画
	{
		//タイトル描画
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		sprite->Render(rc,
			0, 0, 0, screenWidth, screenHeight,
			0,
			1, 1, 1, 1);
		title.render(rc);
	}


	for (auto& button : buttons)
	{
		button.render(rc);
	}
    fade.Render(rc);
}

//GUI描画
void SceneTitle::DrawGUI()
{

}

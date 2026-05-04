#include"System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneManager.h"

//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");
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

	//何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
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
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
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
	}
}

//GUI描画
void SceneTitle::DrawGUI()
{

}

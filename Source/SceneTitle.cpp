#include"System/Graphics.h"
#include "SceneTitle.h"
#include "System/Input.h"
#include "SceneGame.h"
#include "SceneLoading.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"

#include "ScreenSize.h"
#include "AudioManager.h"
#include <DirectXMath.h>

SceneTitle::SceneTitle()
{
	ButtonIndex = 0;
	nextSceneIndex = 0;
	num = 0;
}

//初期化
void SceneTitle::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title_background.png");

	//フェード初期化
	fade.Initialize();

	title.Initialize("Data/Sprite/titleName.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H *0.23f), 1200.0f, 240.0f);
	buttons[0].Initialize("Data/Sprite/Start.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H *0.59f), 290.0f, 120.0f);
	buttons[0].setStageIndex(1);
	buttons[1].Initialize("Data/Sprite/Tutorial.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 500.0f, 120.0f);
	buttons[1].setStageIndex(0);

	nowChoiceButton[0].Initialize("Data/Sprite/selectbox.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.59f), 340.0f, 170.0f);
	nowChoiceButton[1].Initialize("Data/Sprite/selectbox.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.8f), 550.0f, 170.0f);

	// タイトル画面の右側に出すプレイヤー。
	// 本編のPlayer処理は使わず、見た目用のモデルとしてIdleだけ再生する。
	titlePlayerModel = std::make_unique<Model>("Data/Model/Player/Player_animation.mdl");
	titlePlayerAnimation.setModel(titlePlayerModel.get());
	titlePlayerAnimation.PlayAnimation("Idle", true);

	ButtonIndex = 0;
	nextSceneIndex = 0;
	num = 0;
	changeScene = false;

	AudioManager::Instance().Initialize();
	Flag::Instance().SetFlag(Flag::eventName::TitleBGM, true);
}

//終了化
void SceneTitle::Finalize()
{
	if (sprite)
	{
		delete sprite;
		sprite = nullptr;
	}

	titlePlayerModel.reset();
	
}



//更新処理
void SceneTitle::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();

	if (ay>0.2f)
	{
		ButtonIndex = 0;
		num = 0;
	}
	else if(ay<-0.2f)
	{
		ButtonIndex = 1;
		num = 0;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		nextSceneIndex = ButtonIndex;
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
	if (titlePlayerModel)
	{
		titlePlayerAnimation.UpdateAnimation(elapsedTime);
		titlePlayerModel->UpdateTransform();
	}

	fade.Update(elapsedTime);

	for(auto& selectBox : nowChoiceButton)
	{
		selectBox.Update(elapsedTime);
	}
	
	if (!fade.IsFading()&&changeScene)
	{
		if(nextSceneIndex)
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneGame)
		);
		else
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneStageSelect)
		);
		Flag::Instance().SetFlag(Flag::eventName::TitleBGM, false);
	}

	num++;
	if (num >= 500000) num -= 500000;
	
	//音
	AudioManager::Instance().Update(elapsedTime);
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

	// タイトル用3D演出。
	// 右側にIdle中のプレイヤーだけ表示する。
	{
		ModelRenderer* modelRenderer = graphics.GetModelRenderer();

		float aspect = static_cast<float>(graphics.GetScreenWidth()) / static_cast<float>(graphics.GetScreenHeight());

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(0.0f, 1.8f, -6.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 0.9f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		);
		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(45.0f),
			aspect,
			0.1f,
			100.0f
		);

		DirectX::XMStoreFloat4x4(&rc.view, view);
		DirectX::XMStoreFloat4x4(&rc.projection, projection);
		rc.lightDirection = { 0.0f, -1.0f, 0.0f };

		//ここで場所変える
		const DirectX::XMFLOAT3 titlePlayerPos = { 1.0f, 0.35f, 0.0f };

		if (titlePlayerModel)
		{
			DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
			DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(0.0f, DirectX::XMConvertToRadians(205.0f), 0.0f);
			DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(titlePlayerPos.x, titlePlayerPos.y, titlePlayerPos.z);
			DirectX::XMMATRIX world = scale * rotation * translation;

			DirectX::XMFLOAT4X4 worldTransform;
			DirectX::XMStoreFloat4x4(&worldTransform, world);

			modelRenderer->Render(rc, worldTransform, titlePlayerModel.get(), ShaderId::Lambert);
		}
	}


	for (auto& button : buttons)
	{
		button.render(rc);
	}

	if(num%100<50)
	nowChoiceButton[ButtonIndex].render(rc);
    fade.Render(rc);
}

//GUI描画
void SceneTitle::DrawGUI()
{

}

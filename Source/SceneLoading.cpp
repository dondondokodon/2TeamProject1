#include <thread>
#include "System/Graphics.h"
#include "System/Input.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "ScreenSize.h"



//初期化
void SceneLoading::Initialize()
{
	//スプライト初期化
	//sprite = new Sprite("Data/Sprite/LoadingIcon.png");

	sprite.Initialize("Data/Sprite/Load_back.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), SCREEN_W, SCREEN_H);
	loadSprites[0].Initialize("Data/Sprite/Load_circle.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), 200, 200);
	loadSprites[1].Initialize("Data/Sprite/Loading.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), 300, 300);

	loadSprites[0].setRotateSpeed(180.0f);
	loadSprites[1].setRotateSpeed(40.0f);

	//フェード初期化
	fade.Initialize();
	isFading = false;

	//スレッド開始
	thread = new std::thread(LoadingThread, this);
}

//終了化
void SceneLoading::Finalize()
{
	if (thread)
	{
		thread->join();
		delete thread;
		thread = nullptr;
	
	}

	//スプライト終了化
	/*if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}*/


	
}

//更新処理
void SceneLoading::Update(float elapsedTime)
{
	//constexpr float speed = 180;
	//angle += speed * elapsedTime;
	//次のシーンの準備が完了したらシーンを切り替える
	if (nextScene->IsReady()&&!isFading)
	{
		fade.StartFadeOut(1.0f, 0.3f);
		isFading = true;
	}

	if (isFading&&!fade.IsFading())
	{
		SceneManager::Instance().ChangeScene(nextScene.release());
	}

	sprite.Update(elapsedTime);
	for(auto& sprite : loadSprites) sprite.Update(elapsedTime);

	fade.Update(elapsedTime);
}

//描画処理
void SceneLoading::Render()
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
		//画面右下にローディングアイコンを描画
		float screenWidth  = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float spriteWidth  = 256;
		float spriteHeight = 256;
		float positionX    = screenWidth - spriteWidth;
		float positionY    = screenHeight - spriteHeight;

		sprite.render(rc);

		/*sprite->Render(rc,
			positionX, positionY, 0, spriteWidth, spriteHeight,
			angle,
			1, 1, 1, 1);*/
		for(auto& sprite : loadSprites) sprite.render(rc);
		fade.Render(rc);
	}
}

//GUI描画
void SceneLoading::DrawGUI()
{

}
//ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	//次のシーンの初期化を行う
	scene->nextScene->Initialize();

	//スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	//次のシーンの準備完了設定
	scene->nextScene->SetReady();
}

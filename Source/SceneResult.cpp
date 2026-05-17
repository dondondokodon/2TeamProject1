#include "SceneResult.h"
#include "System/Graphics.h"
#include "ScreenSize.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "StageObjectManager.h"
#include "SceneManager.h"

SceneResult::SceneResult()
{

}

SceneResult::~SceneResult()
{

}

//初期化
void SceneResult::Initialize()
{
	changeScene = false;
	isClear = false;
	back.Initialize("Data/Sprite/Load_back.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), SCREEN_W, SCREEN_H);
	sprites[0].Initialize("Data/Sprite/result_back.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.5f), 900.0f, 900.0f);
	sprites[1].Initialize("Data/Sprite/STAGE_CLEAR.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.25f), 600.0f, 100.0f);
	nextSceneButton.Initialize("Data/Sprite/NEXT.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.6f), 600.0f, 350.0f);
}

//終了化
void SceneResult::Finalize()
{

}

//更新処理
void SceneResult::Update(float elapsedTime)
{
	for(auto& sprite:sprites)
		sprite.Update(elapsedTime);

	nextSceneButton.Update(elapsedTime);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (StageObjectManager::Instance().isLastStage())
		{
			isClear = true;
		}

		if (!changeScene)
		{
			fade.StartFadeOut(1.0f, 0.5f);
			changeScene = true;
		}
	}

	back.Update(elapsedTime);
	fade.Update(elapsedTime);
	if (!fade.IsFading() && changeScene)
	{
		if(isClear)
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneTitle)
		);
		else
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneGame)
		);
	}
}

//描画処理
void SceneResult::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();

	back.render(rc);

	for (auto& sprite : sprites)
		sprite.render(rc);

	nextSceneButton.render(rc);

	fade.Render(rc);
}

//GUI描画
void SceneResult::DrawGUI()
{

}
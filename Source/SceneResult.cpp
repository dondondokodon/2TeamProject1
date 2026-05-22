#include "SceneResult.h"
#include "System/Graphics.h"
#include "ScreenSize.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneGame.h"
#include "StageObjectManager.h"
#include "SceneStageSelect.h"
#include "SceneManager.h"
#include <DirectXMath.h>

#include "AudioManager.h"

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
	//フェード初期化
	fade.Initialize();

	back.Initialize("Data/Sprite/Load_back.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), SCREEN_W, SCREEN_H);
	sprites[0].Initialize("Data/Sprite/result_back.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.5f), 900.0f, 900.0f);
	sprites[1].Initialize("Data/Sprite/STAGE_CLEAR.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.25f), 600.0f, 100.0f);
	nextSceneButton.Initialize("Data/Sprite/NEXT.png", DirectX::XMFLOAT2(SCREEN_W * 0.25f, SCREEN_H * 0.6f), 600.0f, 350.0f);

	// リザルト画面右側に表示するプレイヤー
	resultPlayerModel = std::make_unique<Model>("Data/Model/Player/Player_animation.mdl");
	resultPlayerAnimation.setModel(resultPlayerModel.get());

	AudioManager::Instance().Initialize();
	Flag::Instance().ClearFlag();
	Flag::Instance().SetFlag(Flag::eventName::ResultBGM, true);
	resultPlayerAnimation.PlayAnimation("Goal", true); // リザルト画面だけ勝利アニメーション
}

//終了化
void SceneResult::Finalize()
{
	Flag::Instance().ClearFlag();
}

//更新処理
void SceneResult::Update(float elapsedTime)
{
	for(auto& sprite:sprites)
		sprite.Update(elapsedTime);

	if (resultPlayerModel)
	{
		resultPlayerAnimation.UpdateAnimation(elapsedTime);
	}

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
		if(isClear)	//最期のステージクリアしたらタイトルに戻るようにしてる（仮）
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneTitle)
		);
		else //それ以外の時はステージセレクトに行く
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneStageSelect)
		);
		//Flag::Instance().ClearFlag();
	}
	AudioManager::Instance().Update(elapsedTime);
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
	rc.lightDirection = { 0.0f, -1.0f, 0.0f };

	back.render(rc);

	if (resultPlayerModel)
	{
		ModelRenderer* modelRenderer = graphics.GetModelRenderer();

		// リザルト画面用のカメラ
		DirectX::XMFLOAT3 eye = { 0.0f, 1.2f, -5.0f };
		DirectX::XMFLOAT3 focus = { 0.0f, 0.9f, 0.0f };
		DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };

		DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat3(&eye),
			DirectX::XMLoadFloat3(&focus),
			DirectX::XMLoadFloat3(&up));

		DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
			DirectX::XMConvertToRadians(35.0f),
			graphics.GetScreenWidth() / graphics.GetScreenHeight(),
			0.1f,
			1000.0f);

		DirectX::XMStoreFloat4x4(&rc.view, view);
		DirectX::XMStoreFloat4x4(&rc.projection, projection);

		// プレイヤーモデルの位置・向き・大きさ
		DirectX::XMFLOAT3 position = { 1.5f, -0.7f, 1.0f };
		DirectX::XMFLOAT3 scale = { 0.6f, 0.6f, 0.6f };
		float angleY = DirectX::XMConvertToRadians(180.0f);

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angleY);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, S * R * T);

		modelRenderer->Render(rc, transform, resultPlayerModel.get(), ShaderId::Lambert);
	}

	for (auto& sprite : sprites)
		sprite.render(rc);

	nextSceneButton.render(rc);

	fade.Render(rc);
}

//GUI描画
void SceneResult::DrawGUI()
{

}

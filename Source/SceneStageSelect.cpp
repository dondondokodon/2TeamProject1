#include "SceneStageSelect.h"
#include"System/Graphics.h"
#include "System/Input.h"

#include "ScreenSize.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGame.h"

SceneStageSelect::SceneStageSelect()
{
	ButtonIndex = 0;
}


void SceneStageSelect::Initialize()
{
	back.Initialize("Data/Sprite/Title_background.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), SCREEN_W, SCREEN_H);

	//フェード初期化
	fade.Initialize();

	//ボタン初期化
	buttons[0].Initialize("Data/Sprite/StageSelect/stage1.png", DirectX::XMFLOAT2(SCREEN_W * 0.3f, SCREEN_H * 0.25f), 500.0f, 400.0f);
	buttons[0].setStageIndex(0);

	buttons[1].Initialize("Data/Sprite/StageSelect/stage2.png", DirectX::XMFLOAT2(SCREEN_W * 0.7f, SCREEN_H * 0.25f), 500.0f, 400.0f);
	buttons[1].setStageIndex(1);

	buttons[2].Initialize("Data/Sprite/StageSelect/stage3.png", DirectX::XMFLOAT2(SCREEN_W * 0.3f, SCREEN_H * 0.75f), 500.0f, 400.0f);
	buttons[2].setStageIndex(2);

	buttons[3].Initialize("Data/Sprite/StageSelect/stage4.png", DirectX::XMFLOAT2(SCREEN_W * 0.7f, SCREEN_H * 0.75f), 500.0f, 400.0f);
	buttons[3].setStageIndex(3);

	selectButtons[0].Initialize("Data/Sprite/SelectBox.png", DirectX::XMFLOAT2(SCREEN_W * 0.3f, SCREEN_H * 0.25f), 490.0f, 390.0f);
	selectButtons[1].Initialize("Data/Sprite/SelectBox.png", DirectX::XMFLOAT2(SCREEN_W * 0.7f, SCREEN_H * 0.25f), 490.0f, 390.0f);
	selectButtons[2].Initialize("Data/Sprite/SelectBox.png", DirectX::XMFLOAT2(SCREEN_W * 0.3f, SCREEN_H * 0.75f), 490.0f, 390.0f);
	selectButtons[3].Initialize("Data/Sprite/SelectBox.png", DirectX::XMFLOAT2(SCREEN_W * 0.7f, SCREEN_H * 0.75f), 490.0f, 390.0f);

	checkMarks[0] = std::make_unique<Sprite>("Data/Sprite/check.png");
	checkMarks[1] = std::make_unique<Sprite>("Data/Sprite/check.png");
	checkMarks[2] = std::make_unique<Sprite>("Data/Sprite/check.png");
	checkMarks[3] = std::make_unique<Sprite>("Data/Sprite/check.png");


	prevAx = 0.0f;
	prevAy = 0.0f;
	num = 0;
	changeScene = false;
}

void SceneStageSelect::Update(float elapsedTime)
{
	//操作
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();
	float ax = gamePad.GetAxisLX();

	
	int row = ButtonIndex / 2; // 現在の行 (0〜1)
	int col = ButtonIndex % 2; // 現在の列 (0〜1)

	const float THRESHOLD = 0.5f; //しきい値

	// 横方向の移動（左・右）
	if (ax > THRESHOLD && prevAx <= THRESHOLD) {
		if (col < 1)
		{
			col++; // 右へ
			num = 0;
		}
	}
	else if (ax < -THRESHOLD && prevAx >= -THRESHOLD) {
		if (col > 0)
		{
			col--; // 左へ
			num = 0;
		}
	}

	// 縦方向の移動（上・下） 
	if (ay > THRESHOLD && prevAy <= THRESHOLD) {
		if (row > 0)
		{
			row--; // 上へ（行を減らす）
			num = 0;
		}
	}
	else if (ay < -THRESHOLD && prevAy >= -THRESHOLD) {
		if (row < 1)
		{
			row++; // 下へ（行を増やす）
			num = 0;
		}
	}

	// インデックスの再計算
	ButtonIndex = row * 2 + col;

	// 今回の入力を保存
	prevAx = ax;
	prevAy = ay;


	//決定
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		buttons[ButtonIndex].OnClick();
		if (!changeScene)
		{
			fade.StartFadeOut(1.0f, 0.5f);
			changeScene = true;
		}
	}

	//背景更新
	back.Update(elapsedTime);

	//ボタン更新
	for(auto& button : buttons)
		button.Update(elapsedTime);

	for(auto& selectButton : selectButtons)
		selectButton.Update(elapsedTime);

	num++;
	if (num >= 500000) num -= 500000;

	//フェード終わったら
	if (!fade.IsFading() && changeScene)
	{
		SceneManager::Instance().ChangeScene(
			new SceneLoading(new SceneGame)
		);
	}

	//フェード更新
	fade.Update(elapsedTime);
}

void SceneStageSelect::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	RenderState* renderState = graphics.GetRenderState();

	//描画準備
	RenderContext rc;
	rc.deviceContext = dc;
	rc.renderState = graphics.GetRenderState();
	
	//背景
	back.render(rc);

	//ボタン
	for(auto& button : buttons)
		button.render(rc);

	if (num % 80 < 40)
	selectButtons[ButtonIndex].render(rc);

	for (int i = 0; i < 4; i++)
	{
		if (StageObjectManager::Instance().IsCleared(i))
		{
			// ボタンの中心位置
			float cx = (i % 2 == 0) ? SCREEN_W * 0.3f : SCREEN_W * 0.7f;
			float cy = (i < 2) ? SCREEN_H * 0.25f : SCREEN_H * 0.75f;

			// Sprite は左上座標なので補正
			float x = cx - 60;   // 120px の半分
			float y = cy - 60;

			checkMarks[i]->Render(
				rc,
				x - 150, y - 150,   // 中心補正（半分引く）
				0.0f,
				450, 450,           // 幅・高さを拡大
				0.0f,
				1, 1, 1, 1
			);


		}
	}




	//フェード
	fade.Render(rc);
}

void SceneStageSelect::Finalize()
{

}

void SceneStageSelect::DrawGUI()
{

}
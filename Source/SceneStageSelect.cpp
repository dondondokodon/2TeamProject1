#include "SceneStageSelect.h"
#include"System/Graphics.h"
#include "System/Input.h"

#include "ScreenSize.h"
#include"SceneManager.h"
#include"SceneLoading.h"
#include"SceneGame.h"
#include"SceneTitle.h"
#include "StageObjectManager.h"
#include "Flag.h"

#include "AudioManager.h"

namespace
{
constexpr int GRID_ROWS = 3;
constexpr int GRID_COLS = 3;

// -1 = 空セル
constexpr int GRID_TO_INDEX[GRID_ROWS][GRID_COLS] =
{
	{ 0,  1,  2 },
	{ 3,  4,  5 },
	{ -1, 6, -1 },
};

void IndexToGrid(int index, int& row, int& col)
{
	for (int r = 0; r < GRID_ROWS; ++r)
	{
		for (int c = 0; c < GRID_COLS; ++c)
		{
			if (GRID_TO_INDEX[r][c] == index)
			{
				row = r;
				col = c;
				return;
			}
		}
	}

	row = 0;
	col = 0;
}

int MoveStageIndex(int current, int dRow, int dCol)
{
	int row = 0;
	int col = 0;
	IndexToGrid(current, row, col);

	const int newRow = row + dRow;
	const int newCol = col + dCol;

	if (newRow < 0 || newRow >= GRID_ROWS || newCol < 0 || newCol >= GRID_COLS)
	{
		return current;
	}

	const int next = GRID_TO_INDEX[newRow][newCol];
	return (next >= 0) ? next : current;
}
}

SceneStageSelect::SceneStageSelect()
{
	ButtonIndex = 0;
}

DirectX::XMFLOAT2 SceneStageSelect::GetStageCenter(int index) const
{
	int row = 0;
	int col = 0;
	IndexToGrid(index, row, col);

	const float COL_X[3] = { 0.25f, 0.5f, 0.75f };
	const float ROW_Y[3] = { 0.22f, 0.50f, 0.78f };

	return { SCREEN_W * COL_X[col], SCREEN_H * ROW_Y[row] };
}

void SceneStageSelect::Initialize()
{
	back.Initialize("Data/Sprite/Title_background.png", DirectX::XMFLOAT2(SCREEN_W * 0.5f, SCREEN_H * 0.5f), SCREEN_W, SCREEN_H);

	//フェード初期化
	fade.Initialize();

	const float STAGE_BUTTON_WIDTH = 380.0f;
	const float STAGE_BUTTON_HEIGHT = 280.0f;
	const float SELECT_BOX_WIDTH = 370.0f;
	const float SELECT_BOX_HEIGHT = 270.0f;

	static const char* stageSpritePaths[STAGE_COUNT] =
	{
		"Data/Sprite/StageSelect/stage1.png",
		"Data/Sprite/StageSelect/stage2.png",
		"Data/Sprite/StageSelect/stage3.png",
		"Data/Sprite/StageSelect/stage4.png",
		"Data/Sprite/StageSelect/stage5.png",
		"Data/Sprite/StageSelect/stage6.png",
		"Data/Sprite/StageSelect/stage7.png",
	};

	for (int i = 0; i < STAGE_COUNT; ++i)
	{
		const DirectX::XMFLOAT2 center = GetStageCenter(i);

		buttons[i].Initialize(stageSpritePaths[i], center, STAGE_BUTTON_WIDTH, STAGE_BUTTON_HEIGHT);
		buttons[i].setStageIndex(i);

		selectButtons[i].Initialize("Data/Sprite/SelectBox.png", center, SELECT_BOX_WIDTH, SELECT_BOX_HEIGHT);
		checkMarks[i] = std::make_unique<Sprite>("Data/Sprite/check.png");
	}

	stageNumberSprite = std::make_unique<Sprite>("Data/Sprite/number.png");

	prevAx = 0.0f;
	prevAy = 0.0f;
	num = 0;
	changeScene = false;
	nextSceneTitle = false;

	Flag::Instance().SetFlag(Flag::eventName::TitleBGM, true);
	Flag::Instance().SetFlag(Flag::eventName::ResultBGM, false);
	Flag::Instance().SetFlag(Flag::eventName::StageBGM, false);
}

void SceneStageSelect::Update(float elapsedTime)
{
	//操作
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ay = gamePad.GetAxisLY();
	float ax = gamePad.GetAxisLX();

	const float THRESHOLD = 0.5f; //しきい値

	// 横方向の移動（左・右）
	if (ax > THRESHOLD && prevAx <= THRESHOLD) {
		const int next = MoveStageIndex(ButtonIndex, 0, 1);
		if (next != ButtonIndex)
		{
			ButtonIndex = static_cast<short>(next);
			num = 0;
		}
	}
	else if (ax < -THRESHOLD && prevAx >= -THRESHOLD) {
		const int next = MoveStageIndex(ButtonIndex, 0, -1);
		if (next != ButtonIndex)
		{
			ButtonIndex = static_cast<short>(next);
			num = 0;
		}
	}

	// 縦方向の移動（上・下）
	if (ay > THRESHOLD && prevAy <= THRESHOLD) {
		const int next = MoveStageIndex(ButtonIndex, -1, 0);
		if (next != ButtonIndex)
		{
			ButtonIndex = static_cast<short>(next);
			num = 0;
		}
	}
	else if (ay < -THRESHOLD && prevAy >= -THRESHOLD) {
		const int next = MoveStageIndex(ButtonIndex, 1, 0);
		if (next != ButtonIndex)
		{
			ButtonIndex = static_cast<short>(next);
			num = 0;
		}
	}

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

	//タイトルに戻る
	if (GetAsyncKeyState(VK_BACK) & 0x8000)
	{
		if (!changeScene)
		{
			fade.StartFadeOut(1.0f, 0.5f);
			nextSceneTitle = true;
			changeScene = true;
		}

	}

	//背景更新
	back.Update(elapsedTime);

	//ボタン更新
	for (auto& button : buttons)
		button.Update(elapsedTime);

	for (auto& selectButton : selectButtons)
		selectButton.Update(elapsedTime);

	num++;
	if (num >= 500000) num -= 500000;

	//フェード終わったら
	if (!fade.IsFading() && changeScene)
	{
		if (nextSceneTitle)
		{
			SceneManager::Instance().ChangeScene(
				new SceneLoading(new SceneTitle)
			);
		}
		else
		{
			SceneManager::Instance().ChangeScene(
				new SceneLoading(new SceneGame)
			);
		}
	}

	//フェード更新
	fade.Update(elapsedTime);

	AudioManager::Instance().Update(elapsedTime);
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
	for (auto& button : buttons)
		button.render(rc);

	const float STAGE_BUTTON_WIDTH = 380.0f;
	const float STAGE_BUTTON_HEIGHT = 280.0f;
	const float NUMBER_OFFSET_X = 35.0f;
	const float NUMBER_OFFSET_Y = 30.0f;

	for (int i = 0; i < STAGE_COUNT; ++i)
	{
		const DirectX::XMFLOAT2 center = GetStageCenter(i);
		const float numberX = center.x - STAGE_BUTTON_WIDTH * 0.5f + NUMBER_OFFSET_X;
		const float numberY = center.y - STAGE_BUTTON_HEIGHT * 0.5f + NUMBER_OFFSET_Y;

		RenderStageNumber(rc, i + 1, numberX, numberY);
	}

	if (num % 80 < 40)
		selectButtons[ButtonIndex].render(rc);

	for (int i = 0; i < STAGE_COUNT; ++i)
	{
		if (StageObjectManager::Instance().IsCleared(i))
		{
			const DirectX::XMFLOAT2 center = GetStageCenter(i);

			// Sprite は左上座標なので補正
			const float x = center.x - 60;
			const float y = center.y - 60;

			checkMarks[i]->Render(
				rc,
				x - 150, y - 150,
				0.0f,
				450, 450,
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

void SceneStageSelect::RenderStageNumber(const RenderContext& rc, int number, float x, float y)
{
	if (!stageNumberSprite)
	{
		return;
	}


	const float NUMBER_SRC_WIDTH = 48.0f;   // 元画像から切り抜く横幅。
	const float NUMBER_SRC_HEIGHT = 53.0f;  // 元画像から切り抜く縦幅。

	const float NUMBER_DRAW_WIDTH = 72.0f;   // 表示する横サイズ。大きさ調整用
	const float NUMBER_DRAW_HEIGHT = 80.0f;  // 表示する縦サイズ。大きさ調整用
	const float NUMBER_INTERVAL = 72.0f;     // 2桁以上のときの数字間隔、大体NUMBER_DRAW_WIDTHと同じでいい

	std::string text = std::to_string(number);

	// 数字画像は横に0～9が48px間隔で並んでいる。
	// ステージが10以上になっても、1桁ずつ48px間隔で描けばそのまま使える。
	float startX = x;

	for (size_t i = 0; i < text.length(); ++i)
	{
		int digit = text[i] - '0';
		float sx = NUMBER_SRC_WIDTH * static_cast<float>(digit);

		stageNumberSprite->Render(
			rc,
			startX + NUMBER_INTERVAL * static_cast<float>(i), y,
			0.0f,
			NUMBER_DRAW_WIDTH, NUMBER_DRAW_HEIGHT,
			sx, 0.0f,
			NUMBER_SRC_WIDTH, NUMBER_SRC_HEIGHT,
			0.0f,
			1, 1, 1, 1
		);
	}
}

#pragma once
#include "Scene.h"
#include "Sprite2D.h"
#include "Button.h"
#include "fade.h"
#include "System/Model.h"
#include "AnimationController.h"
#include <memory>

class SceneResult:public Scene
{
public:
	SceneResult();
	~SceneResult();

	//初期化
	void Initialize()override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

	//GUI描画
	void DrawGUI()override;

private:
	// リザルト画面の顔テクスチャを読み込む
	void LoadResultFaceTextures();

	// Goalモーションを更新する。ハートの見せ場だけ少し止める
	void UpdateResultPlayerAnimation(float elapsedTime);

	// Goalモーションの時間を見て、今の顔テクスチャを更新する
	void UpdateResultFaceTexture();

	// closeEye が true なら目閉じ、false なら通常顔にする
	void SetResultFaceTexture(bool closeEye);

	// 今が目閉じにしたい時間かどうか
	bool IsResultCloseEyeTime() const;

	Sprite2D sprites[2];
	Button nextSceneButton;
	Sprite2D back;
	Fade fade;
	std::unique_ptr<Model> resultPlayerModel = nullptr;
	AnimationController resultPlayerAnimation;

	// 通常顔のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resultNormalFaceTexture;

	// 目閉じ顔のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resultCloseEyeFaceTexture;

	// 今、目を閉じているか
	bool resultCloseEye = false;

	// ハートの見せ場で止めている残り時間
	float resultHeartStopTimer = 0.0f;

	// 今のGoalループで、もうハート停止をしたか
	bool resultHeartStopDone = false;

	bool changeScene;
	bool isClear;
};


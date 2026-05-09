#pragma once

#include<memory>
#include"System/ModelRenderer.h"
#include"System/AudioSource.h"
#include"Character.h"
#include"ProjectileManager.h"
#include"Effect.h"
#include"AnimationController.h"
#include"BoxCollider.h"	
#include"CylinderCollider.h"

#include"PlayerStates.h"


//プレイヤー
class Player :public Character
{
private:


public:
	Player() {}
	~Player()override {}

	//インスタンス取得
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	//初期化
	void Initialize(const char* modelPath);

	//終了化
	void Finalize();

	//更新処理
	void Update(float elapsedTime, bool canControl);

	//コライダー更新処理
	void UpdateCollider();

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//移動入力処理
	void InputMove(float elapsedTime);

	//ジャンプ入力処理
	bool InputJump();

	//弾丸入力処理
	void InputProjectile();

	//回転入力処理（ロボット専用）
	void InputRotate();

	//操作対象から外れたときの処理
	void StopControl();

	//プレイヤー同士の衝突処理
	void CollisionVsPlayer(Player& other, bool canRide);

	//肩車開始処理
	void StartRiding(Player& target);
	//肩車更新処理
	void UpdateRiding(float elapsedTime);
	//移動入力をリセット
	void ResetMove();
	//肩車可能か
	bool CanStartRiding() const { return rideTimer <= 0.0f; }
	//肩車解除処理
	void StopRiding();
	//肩車中か
	bool IsRiding() const { return isRiding; }
	//ロボットかどうかを設定する
	void SetIsRobot(bool v) { isRobot = v; }
	//ロボットかどうかを取得する
	bool GetIsRobot() const { return isRobot; }
	//肩車位置まで上がりきっているか
	bool IsRideReady() const;

	//ステート変更処理
	void ChangeState(std::unique_ptr<PlayerState> newState);

	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//ゲッター
	AnimationController& GetAnimation() { return animation; }

	void SetIsControlling(bool i) { isControlling = i; }

	bool GetIsControlling() { return isControlling; }

	//向き取得
	DirectX::XMFLOAT3 GetForward() const;


protected:
	//着地したときに呼ばれる
	void OnLanding()override;

private:
	std::unique_ptr<Model> model = nullptr;



	bool isControlling=false;

	float moveSpeed = 5.0f;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	//プレイヤーとエネミーの衝突処理
	void CollisionPlayerVsEnemies();

	//弾丸と敵の衝突処理	
	void CollisionProjectilesVsEnemies();

	//ステージとの衝突処理
	void CollisionPlayerVsStage();

	float JumpSpeed = 12.0f;

	int JumpCount = 0;
	int JumpLimit = 2;
	
	//乗っているか
	bool isRiding = false;
	//乗っている相手
	Player* ridingTarget = nullptr;
	//player1が降りるときの無効か時間
	float rideTimer = 0.0f;
	//ロボットかどうか（Player2はtrue）
	bool isRobot = false;
	//肩車時の高さオフセット
	float rideOffsetY = 0.5f;

	ProjectileManager projectileManager;
	//Effect* hitEffect = nullptr;
	AudioSource* hitSE = nullptr;
	AnimationController animation;
	std::unique_ptr<PlayerState> state = nullptr;
	//BoxCollider bodyCollider;
	CylinderCollider bodyCylinderCollider;
	DirectX::XMFLOAT3 bodyColliderOffset = { 0,0.55f,0 };
};
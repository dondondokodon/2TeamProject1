#pragma once

#include<memory>
#include"System/ModelRenderer.h"
#include"System/AudioSource.h"
#include"Character.h"
#include"ProjectileManager.h"
#include"Effect.h"
#include"AnimationController.h"
#include"BoxCollider.h"

#include"PlayerStates.h"


//プレイヤー
class Player :public Character
{
private:
	Player(){}
	~Player()override{}

public:
	//インスタンス取得
	static Player& Instance()
	{
		static Player instance;
		return instance;
	}

	//初期化
	void Initialize();

	//終了化
	void Finalize();

	//更新処理
	void Update(float elapsedTime);

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

	//ステート変更処理
	void ChangeState(std::unique_ptr<PlayerState> newState);

	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;

	//ゲッター
	AnimationController& GetAnimation() { return animation; }

	//向き取得
	DirectX::XMFLOAT3 GetForward() const;


protected:
	//着地したときに呼ばれる
	void OnLanding()override;

private:
	std::unique_ptr<Model> model = nullptr;




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
	
	ProjectileManager projectileManager;
	Effect* hitEffect = nullptr;
	AudioSource* hitSE = nullptr;
	AnimationController animation;
	std::unique_ptr<PlayerState> state = nullptr;
	BoxCollider bodyCollider;
	DirectX::XMFLOAT3 bodyColliderOffset = { 0,0.55f,0 };
};
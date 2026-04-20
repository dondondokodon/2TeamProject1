#pragma once

#include"System/Model.h"
#include"Enemy.h"
#include"ProjectileManager.h"
#include"Effect.h"
#include"System/AudioSource.h"


//スライム
class EnemySlime :public Enemy
{
public:
	EnemySlime();
	~EnemySlime()override;	//継承元の仮想関数をオーバーライドする場合はoverrideキーワードを付ける事

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override;

	//縄張り設定
	void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

private:
	//ターゲット位置をランダム設定
	void SetRandomTargetPosition();

	//目標地点へ移動
	void MoveToTarget(float elapsedTime, float moveSpeedRate, float turnSpeedRate);

	//徘徊ステートへ遷移
	void SetWanderState();

	//徘徊ステート更新処理
	void UpdateWanderState(float elapsedTime);

	//待機ステートへ遷移
	void SetIdleState();

	//待機ステート更新処理
	void UpdateIdleState(float elapsedTime);

	//プレイヤー索敵
	bool SearchPlayer();

	//攻撃ステートへ遷移
	void SetAttackState();

	//攻撃ステート更新処理
	void UpdateAttackState(float elapsedTime);

	//弾丸とプレイヤーの衝突処理
	void CollisionProjectilesVsPlayer();
private:
	//ステート
	enum class State
	{
		Wander,
		Idle,
		Attack
	};

protected:
	//死亡した時に呼ばれる
	void OnDead()override;
private:
	Model* model                        = nullptr;
	State state                         = State::Wander;
	DirectX::XMFLOAT3 targetPosition    = { 0,0,0 };
	DirectX::XMFLOAT3 territoryOrigin   = { 0,0,0 };
	float territoryRange                = 10.0f;
	float moveSpeed                     = 2.0f;
	float turnSpeed                     = DirectX::XMConvertToRadians(360);
	float stateTimer                    = 0.0f;
	float searchRange                   = 5.0f;
	ProjectileManager projectileManager;
	Effect* hitEffect  = nullptr;
	AudioSource* hitSE = nullptr;
};
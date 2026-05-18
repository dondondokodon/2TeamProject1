#pragma once
#include<memory>
#include "StageObject.h"
#include "Player.h"
#include "Effect.h"
class GoalObject :public StageObject
{
public:
	GoalObject();
	~GoalObject();

	//更新処理
	void Update(float elapsedTime)override;

	//当たり判定
	void CollisionVsPlayer(Player& p);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		renderer->RenderBox(rc, position, angle, scale, DirectX::XMFLOAT4(0, 0, 1, 1));
		DirectX::XMFLOAT3 aabbCenter;
		aabbCenter.x=(aabbMin.x + aabbMax.x)*0.5f;
		aabbCenter.y=(aabbMin.y + aabbMax.y)*0.5f;
		aabbCenter.z=(aabbMin.z + aabbMax.z)*0.5f;
		renderer->RenderBox(rc, aabbCenter, angle, halfSize, DirectX::XMFLOAT4(1, 1, 0, 1));
	}

	void DrawDebugGUI();

	//コライダー更新処理
	void UpdateCollider();

private:
	void PlayGoalEffect();

	DirectX::XMFLOAT3 aabbMin = { 0,0,0 };
	DirectX::XMFLOAT3 aabbMax = { 0,0,0 };
	DirectX::XMFLOAT3 halfSize = { 1.0f,0.5f,1.0f };
	bool isHit = false;
	bool wasHit = false;
	std::unique_ptr<Effect> goalEffect = nullptr;
};


#include "RideState.h"
#include "Player.h"

void RideState::Initialize(Player& player)
{
	//肩車状態に入った瞬間に移動入力を止める
	player.ResetMove();

	//最初は待機アニメーションにする
	isWalkAnimation = false;
	player.GetAnimation().PlayAnimation("Idle", true);
}

void RideState::Finalize(Player& player)
{
}

void RideState::Update(Player& player, float elapsedTime, bool canControl)
{
	//肩車位置まで上がりきっていない間は、操作せずゆっくり上がる
	if (!player.IsRideReady())
	{
		player.UpdateRiding(elapsedTime);
		return;
	}

	//操作中なら、肩車中でもPlayer1を動かす
	if (canControl)
	{
		player.InputMove(elapsedTime);
		player.InputJump();

		DirectX::XMFLOAT3 moveVec = player.GetMoveVec();
		bool isMoving = (fabsf(moveVec.x) > 0.0f || fabsf(moveVec.z) > 0.0f);

		//移動し始めた瞬間だけ歩きアニメーションに切り替える
		if (isMoving && !isWalkAnimation)
		{
			isWalkAnimation = true;
			player.GetAnimation().PlayAnimation("Running", true);
		}
		//止まった瞬間だけ待機アニメーションに切り替える
		else if (!isMoving && isWalkAnimation)
		{
			isWalkAnimation = false;
			player.GetAnimation().PlayAnimation("Idle", true);
		}

		return;
	}

	//操作中でないなら、乗っている相手についていく
	player.UpdateRiding(elapsedTime);
}

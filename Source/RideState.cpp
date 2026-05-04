#include "RideState.h"
#include "Player.h"

void RideState::Initialize(Player& player)
{
	//肩車状態に入った瞬間に移動入力を止める
	player.ResetMove();

	//歩きモーションが残らないように待機アニメーションへ戻す
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

	//操作中なら、肩車中でも自分で動ける
	if (canControl)
	{
		player.InputMove(elapsedTime);
		player.InputJump();

		//移動入力を取得する
		DirectX::XMFLOAT3 moveVec = player.GetMoveVec();

		//移動入力があるなら歩きアニメーションにする
		if (fabsf(moveVec.x) > 0.0f || fabsf(moveVec.z) > 0.0f)
		{
			player.GetAnimation().PlayAnimation("Walk", true);
		}
		//移動入力がないなら待機アニメーションにする
		else
		{
			player.GetAnimation().PlayAnimation("Idle", true);
		}
	}
	//操作中でないなら、乗っている相手についていく
	else
	{
		player.UpdateRiding(elapsedTime);

		//操作していないときは待機アニメーションにする
		player.GetAnimation().PlayAnimation("Idle", true);
	}
}

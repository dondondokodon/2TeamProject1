#include "WalkState.h"

#include"Player.h"

void WalkState::Initialize(Player& player)
{
	player.GetAnimation().PlayAnimation("Running", true);
}

void WalkState::Finalize(Player& player)
{

}

//更新処理
void WalkState::Update(Player& player, float elapsedTime, bool canControl)
{
	//移動入力処理
	player.InputMove(elapsedTime);

	if (fabsf(player.GetMoveVec().x) == 0.0f && fabsf(player.GetMoveVec().z) == 0.0f&&player.getIsGround())
	{
		player.ChangeState(std::make_unique<IdleState>());
	}

	//ジャンプ入力処理
	if (player.InputJump())
		player.ChangeState(std::make_unique<JumpState>());
}
#include "IdleState.h"

#include"Player.h"

void IdleState::Initialize(Player& player)
{
	player.GetAnimation().PlayAnimation("Idle", true);
}

void IdleState::Finalize(Player& player)
{

}

//更新処理
void IdleState::Update(Player& player, float elapsedTime)
{
	//移動入力処理
	player.InputMove(elapsedTime);

	if (fabsf(player.GetMoveVec().x) > 0.0f || fabsf(player.GetMoveVec().z) > 0.0f && player.getIsGround())
	{
		player.ChangeState(std::make_unique<WalkState>());
	}

	//ジャンプ入力処理
	if (player.InputJump())
		player.ChangeState(std::make_unique<JumpState>());
}
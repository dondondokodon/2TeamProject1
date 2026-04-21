#include "JumpState.h"

#include"Player.h"

void JumpState::Initialize(Player& player)
{
	player.GetAnimation().PlayAnimation("Jump", false);
}

void JumpState::Finalize(Player& player)
{

}

//更新処理
void JumpState::Update(Player& player, float elapsedTime)
{
	//移動入力処理
	player.InputMove(elapsedTime);

	if (player.getIsGround())
	{
		player.ChangeState(std::make_unique<IdleState>());
	}

	//ジャンプ入力処理
	if (player.InputJump())
		player.ChangeState(std::make_unique<JumpState>());
}
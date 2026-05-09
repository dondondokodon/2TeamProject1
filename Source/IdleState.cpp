#include"IdleState.h"

#include"Player.h"

void IdleState::Initialize(Player& player)
{
	//待機アニメーションを再生する
	player.GetAnimation().PlayAnimation("Idle", true);
}

void IdleState::Finalize(Player& player)
{

}

//更新処理
void IdleState::Update(Player& player, float elapsedTime, bool canControl)
{
    //回転入力処理（ロボット専用）
    player.InputRotate();

    //移動入力を受け取る
    player.InputMove(elapsedTime);
    //移動入力があり、地面にいるなら歩き状態へ切り替える
    if ((fabsf(player.GetMoveVec().x) > 0.0f || fabsf(player.GetMoveVec().z) > 0.0f) && player.getIsGround())
    {
        player.ChangeState(std::make_unique<WalkState>());
    }
    //ジャンプ入力があればジャンプ状態へ切り替える
    if (player.InputJump())
    {
        player.ChangeState(std::make_unique<JumpState>());
    }
}
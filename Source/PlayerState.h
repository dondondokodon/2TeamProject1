#pragma once
//#include "Player.h"

class Player;

class PlayerState
{
public:
	PlayerState() {}
	virtual ~PlayerState() {}

	virtual void Initialize(Player& player) = 0;

	virtual void Finalize(Player& player) = 0;

	//ステート更新処理
	//canControl は、このプレイヤーが現在操作中かどうか
	virtual void Update(Player& player, float elapsedTime, bool canControl) = 0;
};


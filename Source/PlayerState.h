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

	//XVˆ—
	virtual void Update(Player& player, float elapsedTime) = 0;
};


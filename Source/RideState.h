#pragma once

#include "PlayerState.h"

class Player;

//å®é‘èÛë‘
class RideState : public PlayerState
{
public:
	void Initialize(Player& player) override;
	void Finalize(Player& player) override;
	void Update(Player& player, float elapsedTime, bool canControl) override;
};

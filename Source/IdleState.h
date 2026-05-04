#pragma once
#include "PlayerState.h"
class IdleState :
    public PlayerState
{
public:
	IdleState() {}
	~IdleState() {}
	void Initialize(Player& player) override;
	void Finalize(Player& player) override;
	//XVˆ—
	void Update(Player& player, float elapsedTime, bool canControl) override;
};
#pragma once
#include "PlayerState.h"
class JumpState :
    public PlayerState
{
public:
	JumpState() {}
	~JumpState() {}
	void Initialize(Player& player) override;
	void Finalize(Player& player) override;
	//XVˆ—
	void Update(Player& player, float elapsedTime, bool canControl) override;
};


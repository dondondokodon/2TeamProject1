#pragma once
#include "PlayerState.h"
class WalkState :
    public PlayerState
{
public:
	WalkState() {}
	~WalkState() {}
	void Initialize(Player& player) override;
	void Finalize(Player& player) override;
	//XVˆ—
	void Update(Player& player, float elapsedTime) override;
};


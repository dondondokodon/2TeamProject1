#pragma once
#include "Sprite2D.h"
#include <vector>
#include <memory>

class Tutorial
{
public:
	Tutorial() {};
	~Tutorial() {};
	virtual void Initialize()=0;

	virtual void Finalize()=0;

	virtual void Update(float elapsedTime) = 0;
	virtual void Render()=0;

	protected:
		std::vector<std::unique_ptr<Sprite2D>> sprites;
		int index = 0;
		bool isEnd = false;
};


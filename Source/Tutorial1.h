#pragma once
#include "Tutorial.h"

class Tutorial1 :
    public Tutorial
{
public:
	Tutorial1() {};
	~Tutorial1() {};
	void Initialize() override;

	void Finalize()override;
	void Update(float elapsedTime) override;
	void Render() override;

private:
	unsigned int buttonState[2] = { 0,0 };
	unsigned int buttonDown = 0;
	bool isRender = true;
};


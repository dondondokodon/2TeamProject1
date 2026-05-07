#pragma once
#include"StageObject.h"

class IrradiationDevice:public StageObject
{
public:
	IrradiationDevice();
	~IrradiationDevice() = default;

	//XVˆ—
	void Update(float elapsedTime);

	//•`‰æˆ—
	void Render(const RenderContext& rc, ModelRenderer* renderer);
};


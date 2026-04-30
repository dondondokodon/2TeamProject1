#pragma once

#include"System/ModelRenderer.h"
#include "StageObject.h"

//ステージ
class Stage:public StageObject
{
public:
	Stage();
	~Stage();

	//更新処理
	void Update(float elapsedTime)override;

	//更新処理
	//void Render(const RenderContext& rc, ModelRenderer* renderer);
};
#pragma once
#include "System/ModelRenderer.h"
#include "memory"

class StageObject
{
public:
	StageObject() {}
	~StageObject() {}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:

	//行列更新処理
	void UpdateTransform();

	std::unique_ptr<Model> model = nullptr;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
};


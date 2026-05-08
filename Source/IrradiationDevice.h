#pragma once
#include"StageObject.h"

class IrradiationDevice:public StageObject
{
public:
	IrradiationDevice()
	{
		SetModel("Data/Model/Objects/IrradiationDevice/IrradiationDevice.mdl");
	}
	~IrradiationDevice() = default;

	//更新処理
	void Update(float elapsedTime);

	//ヒット通知
	void OnHit(bool hit) override
	{
		isHit = hit;
	}

	//デバッグ用GUI描画
	void DrawDebugGUI() override;

	//ヒットの詳細判定
	RayHitResult ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal)override;

	//デバッグプリミティブ描画
	/*void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override
	{
		DirectX::XMFLOAT3 position = this->position;
		position.y -= 1;
		renderer->RenderBox(rc, position, angle, scale, DirectX::XMFLOAT4(0, 0, 1, 1));
	}*/

private:
	bool isHit = false;
};


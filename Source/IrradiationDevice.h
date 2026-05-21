#pragma once
#include"StageObject.h"

class IrradiationDevice:public StageObject
{
public:
	IrradiationDevice():modelFilename("Data/Model/Objects/IrradiationDevice/IrradiationDevice2.mdl"),isActiveModelFilename("Data/Model/Objects/IrradiationDevice/IrradiationDeviceIsActive2.mdl")
	{
		SetModel(isActiveModelFilename);	//最初こっちにしたら　正しく動いた
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



	const DirectX::XMFLOAT3& GetAABBMin() const { return aabbMin; }
	const DirectX::XMFLOAT3& GetAABBMax() const { return aabbMax; }

private:
	const char* modelFilename;
	const char* isActiveModelFilename;
	bool isHit = false;
	bool oldHit = false;
	DirectX::XMFLOAT3 aabbMin;
	DirectX::XMFLOAT3 aabbMax;
};


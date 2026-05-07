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

private:
	bool isHit = false;
};


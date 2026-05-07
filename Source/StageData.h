#pragma once
#include<DirectXMath.h>
#include<vector>
#include"StageObject.h"
#include"Stage.h"

enum class ObjectType
{
	Box,
	Mirror,
	Goal,
	IrradiationDevice,
	Laser
};

struct ObjectData
{
	ObjectType type;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 direction;	//ほぼレーザー用

public:
	StageObject* CreateStageObject() const;
};

class StageData
{
public:
	StageData() {}
	~StageData() {}
	std::vector<ObjectData> objects;
	Stage* MyStage = nullptr;
};
#pragma once
#include<DirectXMath.h>
#include<vector>
#include<memory>
#include"StageObject.h"
#include"Stage.h"

enum class ObjectType
{
	Box,
	Mirror,
	Goal,
	IrradiationDevice,
	Laser,
	Stairs,
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
	StageData(const char* filename);
	virtual ~StageData() { for (auto obj : objects) { delete obj; } }
	std::vector<ObjectData*> objects;
	std::unique_ptr<Stage> MyStage = nullptr;
};
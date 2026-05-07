#pragma once
#include<DirectXMath.h>
#include<vector>

enum class ObjectType
{
	Box,
	Mirror,
	Goal,
	Stage,
	IrradiationDevice,
};

struct ObjectData
{
	ObjectType type;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT3 scale;
};

class StageData
{
public:
	StageData() {}
	~StageData() {}
	std::vector<ObjectData> objects;
};
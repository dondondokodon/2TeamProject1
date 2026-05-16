#pragma once
class StageObject;

enum class RayHitType
{
	reflection,
	Stop,
	None
};

struct RayHitResult
{
	bool hit;
	StageObject* object;
	RayHitType type;
	DirectX::XMFLOAT3 hitPos;
	DirectX::XMFLOAT3 hitNormal = {};

	DirectX::XMFLOAT3 reflectionDir;
};

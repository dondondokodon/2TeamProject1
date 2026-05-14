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
};

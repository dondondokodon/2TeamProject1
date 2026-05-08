#pragma once
class StageObject;

enum class RayHitType
{
	reflection,
	Stop
};

struct RayHitResult
{
	bool hit;
	StageObject* object;
	RayHitType type;
	DirectX::XMFLOAT3 hitPos;
};

#pragma once
#include <DirectXMath.h>
#include"CollisionResult.h"

class BoxCollider
{
public:
	BoxCollider() = default;
	~BoxCollider() {}
		
	void SetCenter(const DirectX::XMFLOAT3& c) { center = c; }
	void SetSize(const DirectX::XMFLOAT3& s) { size = s; }

	const DirectX::XMFLOAT3& GetCenter()const { return center; }
	const DirectX::XMFLOAT3& GetSize()const { return size; }

	//別コライダーとの判定
	CollisionResult Intersect(const BoxCollider& other) const;

private:
	DirectX::XMFLOAT3 center = { 0,0,0 };
	DirectX::XMFLOAT3 size = { 1,1,1 };
};


#pragma once
#include <DirectXMath.h>

struct CollisionResult
{
	bool hit = false;
	
	//押し戻し量
	DirectX::XMFLOAT3 pushOut = { 0,0,0 };

	//当たった方向
	DirectX::XMFLOAT3 normal = { 0,1,0 };
};

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


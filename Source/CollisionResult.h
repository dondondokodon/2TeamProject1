#pragma once
#include <DirectXMath.h>


struct CollisionResult
{
	bool hit = false;

	//‰Ÿ‚µ–ß‚µ—Ê
	DirectX::XMFLOAT3 pushOut = { 0,0,0 };

	//“–‚½‚Á‚½•ûŒü
	DirectX::XMFLOAT3 normal = { 0,1,0 };
};


#include"Collision.h"

//球と球の交差判定
bool Collision::IntersectSphereVsSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB
)
{
	//A→Bの単位ベクトルを算出		これ以降全部怪しい
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);	//ここ怪しい    上二つのベクトル
	DirectX::XMVECTOR LengthSq= DirectX::XMVector3LengthSq(Vec);				//ここ怪しい	ベクトルの長さの2乗
	float lengthSq;		//長さの2乗
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//距離判定		
	float range = radiusA + radiusB;
	if (range*range < lengthSq)
	{
		return false;
	}
	//AがBを押し出す
	float length = sqrtf(lengthSq);//長さ
	DirectX::XMVECTOR normal=Vec;	//正規化したベクトル
	if (length)//０除算防止
		normal = DirectX::XMVectorScale(Vec, 1.0f / length);
	
	//Bを押し出す
	DirectX::XMVECTOR push = DirectX::XMVectorScale(normal, range - length);	//めり込んだ量だけ押し出し方向のベクトルを大きくする
	//DirectX::XMVECTOR newPosB = DirectX::XMVectorAdd(PositionB, push);	//元の場所から動いた後の場所の変数

	DirectX::XMStoreFloat3(&outPositionB, push);

	return true;
}

//円柱と円柱の交差判定
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outPositonB)
{
	//Aの足元がBの頭より上なら当たっていない
	if (positionA.y > positionB.y + heightB)
	{
		return false;
	}

	//Aの頭がBの足元より下なら当たっていない
	if (positionA.y + heightA < positionB.y)
	{
		return false;
	}

	//xy平面での範囲チェック
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ= sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;

	//AがBを押し出す
	float push = range - distXZ;//めり込んだ量だけ押し返す
	outPositonB.x = positionB.x+vx*push;
	outPositonB.y = positionB.y;
	outPositonB.z = positionB.z+vz*push;

	return true;
}

bool Collision::IntersectSphereVsCylinder(
	const DirectX::XMFLOAT3& spherePosition,
	float sphereRadius,
	const DirectX::XMFLOAT3& cylinderPosition,
	float cylinderRadius,
	float cylinderHeight,
	DirectX::XMFLOAT3& outCylinderPosition
)
{
	//Aの足元がBの頭より上なら当たっていない
	if (spherePosition.y > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}

	//Aの頭がBの足元より下なら当たっていない
	if (spherePosition.y +sphereRadius*2 < cylinderPosition.y)
	{
		return false;
	}

	//xy平面での範囲チェック
	float vx = cylinderPosition.x - spherePosition.x;
	float vz = cylinderPosition.z - spherePosition.z;
	float range = sphereRadius + cylinderRadius;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)
	{
		return false;
	}

	//単位ベクトル化
	vx /= distXZ;
	vz /= distXZ;

	//AがBを押し出す
	float push = range - distXZ;//めり込んだ量だけ押し返す
	outCylinderPosition.x = cylinderPosition.x + vx * push;
	outCylinderPosition.y = cylinderPosition.y;
	outCylinderPosition.z = cylinderPosition.z + vz * push;

	return true;
}
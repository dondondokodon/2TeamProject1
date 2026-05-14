#include <DirectXCollision.h>
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

// Cylinder vs AABB

inline float Clamp(float v, float min, float max)
{
	return (v < min) ? min : (v > max ? max : v);
}

bool Collision::IntersectCylinderVsAABB(const DirectX::XMFLOAT3& cylPos, float cylRadius, float cylHeight, const DirectX::XMFLOAT3& boxMin, const DirectX::XMFLOAT3& boxMax, DirectX::XMFLOAT3& outPush)
{
	// 円柱の上下
	float cylBottom = cylPos.y;
	float cylTop = cylPos.y + cylHeight;

	// AABB の上下
	float boxBottom = boxMin.y;
	float boxTop = boxMax.y;

	// Y 軸の重なりチェック
	if (cylTop < boxBottom || cylBottom > boxTop)
		return false;

	// XZ 平面で最近接点を求める
	float closestX = Clamp(cylPos.x, boxMin.x, boxMax.x);
	float closestZ = Clamp(cylPos.z, boxMin.z, boxMax.z);

	float dx = cylPos.x - closestX;
	float dz = cylPos.z - closestZ;

	float distSq = dx * dx + dz * dz;
	float r = cylRadius;

	if (distSq > r * r)
		return false;

	// めり込み量（押し戻しベクトル）
	float dist = sqrtf(distSq);
	float penetration = r - dist;

	if (dist > 0.0001f)
	{
		outPush.x = (dx / dist) * penetration;
		outPush.z = (dz / dist) * penetration;
	}
	else
	{
		// 完全に中心が重なった場合は適当に押し出す
		outPush.x = r;
		outPush.z = 0;
	}

	outPush.y = 0; // Y は押し戻さない（地面判定は別でやっているため）

	return true;
}


//// レイキャスト
//bool Collision::RayCast(
//	const DirectX::XMFLOAT3& start,
//	const DirectX::XMFLOAT3& end,
//	const DirectX::XMFLOAT4X4& worldTransform,
//	const Model* model,
//	DirectX::XMFLOAT3& hitPosition,
//	DirectX::XMFLOAT3& hitNormal)
//{
//	bool hit = false;
//
//	// 始点と終点からレイのベクトルと長さを求める
//	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
//	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
//	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
//	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
//
//	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
//	if (nearestDist <= 0.0f) return false;
//
//	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);
//
//	// モデル内の全てのメッシュと交差判定を行う
//	const ModelResource* resource = model->GetResource();
//	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
//	{
//		// メッシュのワールド行列を求める
//		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
//		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
//		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);
//
//		// レイをメッシュのローカル空間に変換する
//		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
//		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
//		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
//		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
//		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));
//		float nearestLocalDist = distance;
//
//		// メッシュ内の全ての三角形と交差判定を行う
//		for (size_t i = 0; i < mesh.indices.size(); i += 3)
//		{
//			// 三角形の頂点座標を取得
//			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
//			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
//			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));
//
//			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
//			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
//			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
//
//			// 三角形の交差判定
//			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
//			{
//				// メッシュのローカル空間での交点を求める
//				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
//				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);
//
//				// メッシュのローカル空間での交点をワールド空間に変換する
//				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);
//
//				// ワールド空間でのレイの始点から交点までの距離を求める
//				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
//				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
//				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);
//
//				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
//				if (worldHitDist <= nearestDist)
//				{
//					// メッシュのローカル空間での三角形の法線ベクトルを算出
//					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
//					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
//					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);
//
//					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
//					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);
//
//					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
//					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
//					float dot = DirectX::XMVectorGetX(Dot);
//					if (dot < 0)
//{
//	DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
//	DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);
//	return true;
//}
//
//				}
//			}
//		}
//	}
//	return hit;
//}

bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	using namespace DirectX;

	bool hit = false;

	XMVECTOR WorldRayStart = XMLoadFloat3(&start);
	XMVECTOR WorldRayEnd = XMLoadFloat3(&end);
	XMVECTOR WorldRayVec = WorldRayEnd - WorldRayStart;

	float maxDistance = XMVectorGetX(XMVector3Length(WorldRayVec));
	if (maxDistance <= 0.0f) return false;

	float nearestDistT = 1.0f;

	const ModelResource* resource = model->GetResource();
	XMMATRIX ParentWorld = XMLoadFloat4x4(&worldTransform);

	for (const auto& mesh : resource->GetMeshes())
	{
		XMMATRIX World =
			XMLoadFloat4x4(&model->GetNodes().at(mesh.nodeIndex).globalTransform) *
			ParentWorld;

		XMMATRIX InvWorld = XMMatrixInverse(nullptr, World);

		XMVECTOR LocalStart = XMVector3Transform(WorldRayStart, InvWorld);
		XMVECTOR LocalEnd = XMVector3Transform(WorldRayEnd, InvWorld);
		XMVECTOR LocalVec = LocalEnd - LocalStart;

		float localMaxDist = XMVectorGetX(XMVector3Length(LocalVec));
		if (localMaxDist <= 0.0f) continue;

		XMVECTOR LocalDir = XMVectorScale(LocalVec, 1.0f / localMaxDist);

		DirectX::XMFLOAT3 boxCenter = {
			(mesh.boundsMin.x + mesh.boundsMax.x) * 0.5f,
			(mesh.boundsMin.y + mesh.boundsMax.y) * 0.5f,
			(mesh.boundsMin.z + mesh.boundsMax.z) * 0.5f
		};

		const float boundsMargin = 0.1f;

		DirectX::XMFLOAT3 boxExtents = {
			(mesh.boundsMax.x - mesh.boundsMin.x) * 0.5f + boundsMargin,
			(mesh.boundsMax.y - mesh.boundsMin.y) * 0.5f + boundsMargin,
			(mesh.boundsMax.z - mesh.boundsMin.z) * 0.5f + boundsMargin
		};

		DirectX::BoundingBox boundingBox(boxCenter, boxExtents);

		float boxDist = 0.0f;
		if (!boundingBox.Intersects(LocalStart, LocalDir, boxDist))
		{
			continue;
		}

		if (boxDist > localMaxDist)
		{
			continue;
		}

		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			XMVECTOR A = XMLoadFloat3(&mesh.vertices[mesh.indices[i]].position);
			XMVECTOR B = XMLoadFloat3(&mesh.vertices[mesh.indices[i + 1]].position);
			XMVECTOR C = XMLoadFloat3(&mesh.vertices[mesh.indices[i + 2]].position);

			float t;
			if (DirectX::TriangleTests::Intersects(LocalStart, LocalDir, A, B, C, t))
			{
				if (t >= 0.0f && t <= localMaxDist)
				{
					float worldT = t / localMaxDist;

					if (worldT < nearestDistT)
					{
						XMVECTOR Normal = XMVector3Cross(B - A, C - B);

						if (XMVectorGetX(XMVector3Dot(LocalDir, Normal)) < 0)
						{
							nearestDistT = worldT;

							XMVECTOR HitPos = LocalStart + LocalDir * t;

							XMStoreFloat3(
								&hitPosition,
								XMVector3Transform(HitPos, World)
							);

							XMStoreFloat3(
								&hitNormal,
								XMVector3Normalize(XMVector3TransformNormal(Normal, World))
							);

							hit = true;
						}
					}
				}
			}
		}
	}

	return hit;
}

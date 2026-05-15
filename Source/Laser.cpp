#include "Laser.h"
#include"Collision.h"
#include <algorithm>
#include<imgui.h>
#include"StageObjectManager.h"
#include"math.h"
#include"EffectManager.h"
#include "Player.h"

using namespace DirectX;

void LaserBeam::SetMirrorPlayers(Player** players, int count)
{
	mirrorPlayers = players;
	mirrorPlayerCount = count;
}


void LaserBeam::Update(float elapsedTime) 
{
	segments.clear();

	DirectX::XMFLOAT3 start = origin;
	DirectX::XMFLOAT3 dir = direction;

	for (int i = 0; i < maxReflection; i++)
	{
		DirectX::XMFLOAT3 end =
		{
			start.x + dir.x * maxLength,
			start.y + dir.y * maxLength,
			start.z + dir.z * maxLength
		};

		DirectX::XMFLOAT3 hitPos, hitNormal;

		//    StageObjectManager  ?  C L   X g  ?     
		RayHitResult hit = StageObjectManager::Instance().RayCast(start, end, hitPos, hitNormal);

		// 今回のレーザー線分がロボットの仮想鏡に当たったかを確認するための変数
		Player* hitMirrorPlayer = nullptr;

		// ステージオブジェクトに当たった距離を記録、後でロボットの鏡にも当たった場合より手前にある方を優先するから
		float stageHitDistSq = FLT_MAX;
		if (hit.hit)
		{
			float dx = hitPos.x - start.x;
			float dy = hitPos.y - start.y;
			float dz = hitPos.z - start.z;
			stageHitDistSq = dx * dx + dy * dy + dz * dz;
		}

		// Player2はStageObjectではないので、StageObjectManagerのRayCastには含まれない→レーザー側でプレイヤー配列を見て、ロボットの仮想鏡面との当たり判定えお
		for (int playerIndex = 0; playerIndex < mirrorPlayerCount; ++playerIndex)
		{
			Player* player = mirrorPlayers[playerIndex];

			// プレイヤーが存在しない、または鏡を持っていない場合は判定しない
			if (!player || !player->IsLaserMirror()) continue;

			DirectX::XMFLOAT3 playerHitPos;
			DirectX::XMFLOAT3 playerHitNormal;

			// ロボット正面の仮想鏡面に、このレーザー線分が当たるか調べる
			if (!player->RayCastLaserMirror(start, end, playerHitPos, playerHitNormal)) continue;

			// レーザー開始位置から、ロボット鏡面のヒット位置までの距離を求める
			float dx = playerHitPos.x - start.x;
			float dy = playerHitPos.y - start.y;
			float dz = playerHitPos.z - start.z;
			float playerHitDistSq = dx * dx + dy * dy + dz * dz;

			// ステージオブジェクトより手前でロボットの鏡に当たった場合だけ、
			// 今回のヒット結果をロボット鏡面の反射として扱う
			if (playerHitDistSq < stageHitDistSq)
			{
				hit.hit = true;

				// ロボットはStageObjectではないので object は nullptr にする
				hit.object = nullptr;

				// レーザーの通常反射処理に流すため、reflection として扱う
				hit.type = RayHitType::reflection;
				hit.hitPos = playerHitPos;

				hitPos = playerHitPos;
				hitNormal = playerHitNormal;
				stageHitDistSq = playerHitDistSq;
			}
		}

		//反射
		if (hit.object)
		{
			//それぞれのヒット条件と見比べる
			hit = hit.object->ReallyHit(dir, hitPos, hitNormal);
			if (hit.hit)
			{
				hit.object->OnHit(true); //ヒット通知
			}
		}
		else if (!hit.hit)
		{
			 segments.push_back({ start, end,hitNormal,false });
			 break;
		}


		if (hit.type == RayHitType::reflection)
		{
			// 反射点までのレーザー線分を登録
			segments.push_back({ start, hitPos ,hitNormal,true});

		  //反射方向
			DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&dir);
			//法線
			//XZ平面にする
			hitNormal.y = 0.0f;
			DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&hitNormal);
			//反射
			DirectX::XMVECTOR r = DirectX::XMVector3Reflect(d, n);
			//正規化
			DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(r));

			// 次のレーザーの開始位置を、反射点から少しだけ進める
			// 同じ位置から再スタートすると、同じ鏡にもう一度当たってしまうことがあるから
			start = {
				hitPos.x + dir.x * 0.02f,
				hitPos.y + dir.y * 0.02f,
				hitPos.z + dir.z * 0.02f
			};
		}
		else if (hit.type == RayHitType::Stop)
		{
			// 反射しない物体に当たった場合は当たった一でレーザーを止める
			segments.push_back({ start, hitPos,hitNormal,true });
			break;
		}
		else if (hit.type == RayHitType::None)
		{
			//貫通するものに当たった場合はそこまでのlaserを作って貫通後のレーザーを計算する
			//segments.push_back({ start, hitPos,hitNormal,true });
			//start = hitPos;
			continue;
		}

	}
}

void LaserBeam::Render()
{
	// 1. 回転中は何もしない
	if (isRotating) {
		if (isEffectPlaying) {
			//StopEffect();
			//isEffectPlaying = false;
			StopBackEffect();
		}
		//return;
	}
	
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	if (!isEffectPlaying)
	{
		BackEffectHandle = laserBackEffect->Play(origin, 0.5f);

		// 1. 方向ベクトルから角度を計算
		float yaw = atan2f(direction.x, direction.z);
		float xzLen = sqrtf(direction.x * direction.x + direction.z * direction.z);
		float pitch = -atan2f(direction.y, xzLen);

		// 2. 素材が元々逆を向いているなら、180度（XM_PI）回転させて相殺する
		// yaw に対して XM_PI を加算する
		effekseerManager->SetRotation(BackEffectHandle, pitch, yaw + DirectX::XM_PI, 0.0f);
	}
	
	//セグメント数とエフェクト数の同期
	//反射が減った場合：余分なエフェクトを止める

	while (activeEffects.size() > segments.size()) {
		effekseerManager->StopEffect(activeEffects.back());
		activeEffects.pop_back();
	}

	for (size_t i = 0; i < segments.size(); ++i) {
		auto& seg = segments[i];
	
		DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
		DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
		float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e, s));
	
		Effekseer::Handle handle;
	
		//足りない場合のみ新しくPlayする
		if (i >= activeEffects.size()) {
			handle = laserEffect.get()->Play(seg.start, 1.0f);
			activeEffects.push_back(handle);
		}
		else {
			handle = activeEffects[i];
		}
		
	
		//行列計算
		dir = DirectX::XMVectorNegate(dir);
		DirectX::XMVECTOR worldUp = DirectX::XMVectorSet(0, 1, 0, 0);
		if (fabsf(DirectX::XMVectorGetY(dir)) > 0.999f) {
			worldUp = DirectX::XMVectorSet(0, 0, 1, 0);
		}
		DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, dir));
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(dir, right);
	
		const float baseEffectSize = 28.0f;
		DirectX::XMMATRIX mat = DirectX::XMMATRIX(
			DirectX::XMVectorScale(right, 1.0f),
			DirectX::XMVectorScale(up, 1.0f),
			DirectX::XMVectorScale(dir, length / baseEffectSize),
			DirectX::XMVectorSetW(s, 1.0f)
		);
	
		Effekseer::Matrix43 effekMat;
		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m, mat);
		effekMat.Value[0][0] = m._11; effekMat.Value[0][1] = m._12; effekMat.Value[0][2] = m._13;
		effekMat.Value[1][0] = m._21; effekMat.Value[1][1] = m._22; effekMat.Value[1][2] = m._23;
		effekMat.Value[2][0] = m._31; effekMat.Value[2][1] = m._32; effekMat.Value[2][2] = m._33;
		effekMat.Value[3][0] = m._41; effekMat.Value[3][1] = m._42; effekMat.Value[3][2] = m._43;
	
		// 毎フレーム、最新のセグメント行列をセット
		effekseerManager->SetMatrix(handle, effekMat);
	}

	//反射エフェクト
	//セグメント数とエフェクト数の同期
	int refrectionCount = segments.size();
	if (refrectionCount > 0 && segments[refrectionCount-1].hit == false)
		refrectionCount--;

	while(activeReflectEffects.size() > refrectionCount) {
		effekseerManager->StopEffect(activeReflectEffects.back());
		activeReflectEffects.pop_back();
	}

	for (size_t i = 0; i < refrectionCount; ++i) {
		auto& seg = segments[i];
		if (!seg.hit)break;

		DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
		DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
		float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e, s));
		DirectX::XMFLOAT3 Direction;
		DirectX::XMStoreFloat3(&Direction, dir);

		Effekseer::Handle handle;

		//足りない場合のみ新しくPlayする
		if (i >= activeReflectEffects.size()) {
			handle = beamReflectEffect.get()->Play(seg.end, 1.0f);
			activeReflectEffects.push_back(handle);
		}
		else {
			handle = activeReflectEffects[i];
		}

		//角度計算
		// 1. 方向ベクトルから角度を計算
		float yaw = atan2f(seg.normal.x, seg.normal.z);
		float xzLen = sqrtf(seg.normal.x * seg.normal.x + seg.normal.z * seg.normal.z);
		float pitch = -atan2f(seg.normal.y, xzLen);

		// 座標を最新のヒット地点(seg.end)に更新する
		effekseerManager->SetLocation(handle, seg.end.x, seg.end.y, seg.end.z);

		// 2. 素材が元々逆を向いているなら、180度（XM_PI）回転させて相殺する
		// yaw に対して XM_PI を加算する
		effekseerManager->SetRotation(activeReflectEffects[i], pitch, yaw + DirectX::XM_PI, 0.0f);
	}

	
	isEffectPlaying = true;
}

//?f?o?b?O?pGUI?`??
void LaserBeam::DrawDebugGUI()
{
	if (ImGui::Begin("Beam", nullptr, ImGuiWindowFlags_None))
	{
		//?g?????X?t?H?[??
		if (ImGui::CollapsingHeader("Item", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//??u
			ImGui::InputFloat3("dir", &direction.x);

			//????
			ImGui::InputFloat("maxLength", &maxLength);

			//????
			ImGui::InputFloat("radius", &radius);
		}
	}
	ImGui::End();
}

LaserHit LaserBeam::CheckHitAABB(const BoxCollider& box) const
{
	//if (isRotating)
	//	return LaserHit(); // ????????

	LaserHit result;
	//float bestDist = FLT_MAX;


	// AABB ?? min/max
	DirectX::XMFLOAT3 bmin =
	{
		box.GetCenter().x - box.GetSize().x * 0.5f,
		box.GetCenter().y - box.GetSize().y * 0.5f,
		box.GetCenter().z - box.GetSize().z * 0.5f
	};
	DirectX::XMFLOAT3 bmax =
	{
		box.GetCenter().x + box.GetSize().x * 0.5f,
		box.GetCenter().y + box.GetSize().y * 0.5f,
		box.GetCenter().z + box.GetSize().z * 0.5f
	};

	// ?S???????????????
	for (const auto& seg : segments)
	{
		DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
		DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
		DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e,s));

		float segLen = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));

		// AABB ???S
		DirectX::XMVECTOR boxCenter = DirectX::XMLoadFloat3(&box.GetCenter());
		DirectX::XMVECTOR v = DirectX::XMVectorSubtract(boxCenter, s);

		// ??????????_
		float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, dir));
		t = std::clamp(t, 0.0f, segLen);

		DirectX::XMVECTOR closestOnRay = DirectX::XMVectorAdd(s, DirectX::XMVectorScale(dir, t));

		// AABB ??????_
		DirectX::XMFLOAT3 rayPoint;
		DirectX::XMStoreFloat3(&rayPoint, closestOnRay);

		DirectX::XMFLOAT3 closestOnAABB =
		{
			std::clamp(rayPoint.x, bmin.x, bmax.x),
			std::clamp(rayPoint.y, bmin.y, bmax.y),
			std::clamp(rayPoint.z, bmin.z, bmax.z)
		};

		DirectX::XMVECTOR aabbP = DirectX::XMLoadFloat3(&closestOnAABB);

		// ????
		float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(aabbP , closestOnRay)));


		float skin = 0.01f;

		if (dist <= radius - skin)
		{
			float hitDist = t;  //・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾌ具ｿｽ・ｽ・ｽ
			result.hit = true;

			float depth = radius - dist;

			// 押しすぎ防止
			float push = depth * 0.7f;

			// 最小保証
			push = max(push, 0.01f);

			result.penetration = push;

			// 法線
			DirectX::XMFLOAT3 dirOut =
			{
				box.GetCenter().x - rayPoint.x,
				box.GetCenter().y - rayPoint.y,
				box.GetCenter().z - rayPoint.z
			};

			DirectX::XMVECTOR n = DirectX::XMVector3Normalize(
				DirectX::XMLoadFloat3(&dirOut)
			);

			DirectX::XMStoreFloat3(&result.normal, n);

			return result;
		}
	  
	}

	return result;
	//return bestHit;
}

//・ｽ~・ｽ・ｽ・ｽﾆの費ｿｽ・ｽ・ｽ
LaserHit LaserBeam::CheckHitCylinder(const CylinderCollider& cylinder) const
{
	//if (isRotating)
	//	return LaserHit();

	LaserHit result;

	float halfH = cylinder.GetHeight() * 0.5f;
	float cylR = cylinder.GetRadius();
	DirectX::XMFLOAT3 center = cylinder.GetCenter();

	for (const auto& seg : segments)
	{
		// ざっくり距離チェック。
// 円柱中心がレーザー線分の範囲から明らかに遠い場合は、細かい判定をしない。
		float minX = min(seg.start.x, seg.end.x) - (cylR + radius);
		float maxX = max(seg.start.x, seg.end.x) + (cylR + radius);
		float minY = min(seg.start.y, seg.end.y) - (halfH + radius);
		float maxY = max(seg.start.y, seg.end.y) + (halfH + radius);
		float minZ = min(seg.start.z, seg.end.z) - (cylR + radius);
		float maxZ = max(seg.start.z, seg.end.z) + (cylR + radius);

		if (center.x < minX || center.x > maxX ||
			center.y < minY || center.y > maxY ||
			center.z < minZ || center.z > maxZ)
		{
			continue;
		}

		DirectX::XMVECTOR s = XMLoadFloat3(&seg.start);
		DirectX::XMVECTOR e = XMLoadFloat3(&seg.end);

		DirectX::XMVECTOR segVec = XMVectorSubtract(e, s);
		float segLen = XMVectorGetX(XMVector3Length(segVec));
		if (segLen <= 0.0001f) continue;

		DirectX::XMVECTOR dir = XMVectorScale(segVec, 1.0f / segLen);

		// レーザー上の最近接点 p
		DirectX::XMVECTOR c = XMLoadFloat3(&center);
		float t = XMVectorGetX(XMVector3Dot(XMVectorSubtract(c, s), dir));
		t = std::clamp(t, 0.0f, segLen);

		DirectX::XMVECTOR pVec = XMVectorAdd(s, XMVectorScale(dir, t));

		DirectX::XMFLOAT3 p;
		DirectX::XMStoreFloat3(&p, pVec);

		// Cylinder 上の最近接点 q
		DirectX::XMFLOAT3 q;

		// Y clamp
		q.y = std::clamp(p.y, center.y - halfH, center.y + halfH);

		// XZ 円
		float dx = p.x - center.x;
		float dz = p.z - center.z;
		float len = sqrtf(dx * dx + dz * dz);

		if (len > cylR)
		{
			q.x = center.x + dx / len * cylR;
			q.z = center.z + dz / len * cylR;
		}
		else
		{
			q.x = p.x;
			q.z = p.z;
		}

		DirectX::XMVECTOR qVec = XMLoadFloat3(&q);

		// 距離
		DirectX::XMVECTOR v = XMVectorSubtract(qVec, pVec);
		float dist = XMVectorGetX(XMVector3Length(v));

		float skin = 0.01f;

		if (dist <= radius - skin)
		{
			result.hit = true;

			float depth = radius - dist;

			// AABB と同じ押し量
			float push = depth * 0.7f;
			push = max(push, 0.01f);

			result.penetration = push;

			// ? 法線（AABB と同じ向きに統一）
			DirectX::XMVECTOR n;

			if (dist > 0.0001f)
			{
				// AABB と同じ向き：center - p
				DirectX::XMFLOAT3 dirOut =
				{
					center.x - p.x,
					center.y - p.y,
					center.z - p.z
				};
				n = XMVector3Normalize(XMLoadFloat3(&dirOut));
			}
			else
			{
				// fallback：XZ 方向優先
				DirectX::XMFLOAT3 fallback =
				{
					center.x - p.x,
					0.0f,
					center.z - p.z
				};

				DirectX::XMVECTOR fb = XMLoadFloat3(&fallback);

				if (XMVector3Length(fb).m128_f32[0] < 0.0001f)
					fb = XMVectorSet(1, 0, 0, 0);

				n = XMVector3Normalize(fb);
			}

			DirectX::XMStoreFloat3(&result.normal, n);
			result.point = q;

			return result;
		}
	}

	return result;


}

void Laser::UpdateTransformByAngle(const DirectX::XMFLOAT3& center, float totalAngleY)
{
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(totalAngleY);
	DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&center);

	// 1. ポジション：常に baseStartPos から計算
	DirectX::XMVECTOR basePos = DirectX::XMLoadFloat3(&baseStartPos);
	DirectX::XMVECTOR local = DirectX::XMVectorSubtract(basePos, c);
	local = DirectX::XMVector3Transform(local, rot);
	DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(local, c);
	DirectX::XMStoreFloat3(&startPos, newPos);

	// 2. 方向：常に baseDirection から計算
	DirectX::XMVECTOR baseDir = DirectX::XMLoadFloat3(&baseDirection);
	DirectX::XMVECTOR newDir = DirectX::XMVector3TransformNormal(baseDir, rot);
	DirectX::XMStoreFloat3(&direction, newDir);


	currentAngleY = totalAngleY;
	// 3. Beam(LaserBeam) に最新の値を伝える
	beam.origin = startPos;
	beam.direction = direction;
}

void Laser::RotateAroundCenter(const DirectX::XMFLOAT3& center, float angleY)
{
	// origin を中心基準に移動
	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&startPos);
	DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR local = DirectX::XMVectorSubtract(pos, c);

	// Y軸回転
	DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(angleY);
	local = DirectX::XMVector3Transform(local, rot);

	// 中心に戻す
	DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(local, c);
	DirectX::XMStoreFloat3(&startPos, newPos);

	// direction も回転
	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
	dir = DirectX::XMVector3TransformNormal(dir, rot);
	DirectX::XMStoreFloat3(&direction, dir);
}

void Laser::Initialize(
	const DirectX::XMFLOAT3& emitterPos,
	const DirectX::XMFLOAT3& dir,
	float maxLen)
{
   model = std::make_unique<Model>("Data/Model/Objects/Laser/Laser.mdl");

   //beam.setEffect("Data/Effect/laserOre.efkefc");
   beam.setEffect("Data/Effect/reizar.efkefc","Data/Effect/laser_back.efkefc","Data/Effect/hansya.efkefc");

   // 最初に dir を正規化したものを baseDirection に入れる
   XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&dir));
   XMStoreFloat3(&baseDirection, v); // ここで保存！

	baseStartPos = emitterPos;

	startPos = emitterPos;
	direction = baseDirection;
	maxLength = maxLen;
  
	scale = { 0.5f,0.5f,0.5f };

	
	//DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
	//v = DirectX::XMVector3Normalize(v);
	//DirectX::XMStoreFloat3(&direction, v);


	// LaserBeam
	beam.origin = startPos;
	beam.direction = direction;
	beam.maxLength = maxLength;
	beam.maxReflection = 5;
	//beam.radius = 0.3f;

   // OutputDebugStringA("laserやってる\n");
}

void Laser::Update(float elapsedTime)
{
	beam.isRotating = isRotating;


	if (!isActive) return;

  /*  Shoot();

	beam.SetPoints(startPos, endPos);
	beam.SetAngle(angle);
	beam.setDirection(direction);
	beam.Update(elapsedTime);*/

	position = startPos;

	//position.x -= direction.x*0.5f;
	angle.y = atan2f(direction.x, direction.z);

	UpdateTransform();

	// レーザーの位置
	beam.origin = startPos;
	beam.direction = direction;
	// LaserBeam がレーザーを撃つ（反射含む）
	beam.Update(elapsedTime);
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	if (!isActive) return;

	beam.Render();

	StageObject::Render(rc, renderer);
}

void Laser::SetRotating(bool flag)
{
	isRotating = flag;
}

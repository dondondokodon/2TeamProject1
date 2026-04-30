#pragma once
#include<memory>
#include"StageObject.h"
#include"StageObjectManager.h"
#include"BoxCollider.h"

//レーザーのビーム
//class LaserBeam
//{
//public:
//	LaserBeam() {};
//	~LaserBeam() {}
//
//	//始点と終点をセット
//	void SetPoints(
//		const DirectX::XMFLOAT3& start,
//		const DirectX::XMFLOAT3& end
//	) {
//		startPos = start;
//		endPos = end;
//		UpdateTransform();
//	}
//
//	void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }
//	void setDirection(const DirectX::XMFLOAT3& d) { direction = d; }
//
//	//太さ
//	void setWidth(float w) {
//		width = w;
//		UpdateTransform();
//	}
//
//	//更新
//	void Update(float elapsedTime);
//
//	//描画
//	void Render(const RenderContext& rc, ModelRenderer* renderer);
//
//	//デバッグプリミティブ描画
//	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
//	{
//		renderer->RenderBox(rc, startPos, { 0,0,0 }, { width, width, 0.1f }, DirectX::XMFLOAT4(1, 1, 0, 1));
//		//コライダーのデバッグ描画
//		renderer->RenderBox(rc, topCollider.GetCenter(), { 0,0,0 }, topCollider.GetSize(), DirectX::XMFLOAT4(1, 0, 0, 1));
//		renderer->RenderBox(rc, sideCollider.GetCenter(),{0,0,0}, sideCollider.GetSize(), DirectX::XMFLOAT4(0, 1, 0, 1));
//	}
//
//	//ゲッター
//	BoxCollider& GetTopCollider() { return topCollider; }
//	BoxCollider& GetSideCollider() { return sideCollider; }
//
//	//Collieder更新
//	void UpdateColliders();
//
//private:
//	DirectX::XMFLOAT3 startPos = { 0,0,0 };
//	DirectX::XMFLOAT3 endPos = { 0,0,0 };
//	DirectX::XMFLOAT3 angle = { 0,0,0 };
//	DirectX::XMFLOAT3 direction = { 1,0,0 };
//
//	//上に乗れる
//	BoxCollider topCollider;
//	BoxCollider sideCollider;
//
//
//
//	float width = 0.1f;
//
//	//板ポリモデル
//
//	//行列
//	DirectX::XMFLOAT4X4 transform = {
//		1,0,0,0,
//		0,1,0,0,
//		0,0,1,0,
//		0,0,0,1
//	};
//
//	void UpdateTransform();
//};

struct LaserSegment
{
	DirectX::XMFLOAT3 start;
	DirectX::XMFLOAT3 end;
};

struct LaserHit
{
	bool hit = false;
	DirectX::XMFLOAT3 point;
	DirectX::XMFLOAT3 normal;
	float penetration = 0.0f;//当たり判定の貫通量（距離）
};

class LaserBeam
{
public:
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;
	float maxLength = 50.0f;
	float radius = 0.5f;
	int maxReflection = 3;

	std::vector<LaserSegment> segments;

	void Update();
	LaserHit CheckHitAABB(const BoxCollider& box) const;

	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		// LaserBeam の線分を描画
		for (auto& seg : segments)
		{
			// start と end のベクトル
			DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
			DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);

			// 中心位置
			DirectX::XMVECTOR c = DirectX::XMVectorScale(DirectX::XMVectorAdd(s , e) , 0.5f);
			DirectX::XMFLOAT3 center;
			DirectX::XMStoreFloat3(&center, c);

			// 長さ
			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e , s)));
			length *= 0.5f; // Boxモデルが「中心から前後0.5ずつ」なら、長さを半分にする必要があります。

			// 方向ベクトル
			DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e , s));

			// Z軸をレーザー方向に合わせる回転
			DirectX::XMVECTOR z = dir;
			DirectX::XMVECTOR y = DirectX::XMVectorSet(0, 1, 0, 0);
			DirectX::XMVECTOR x = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(y, z));
			y = DirectX::XMVector3Cross(z, x);

			DirectX::XMFLOAT4X4 rot;
			DirectX::XMStoreFloat4x4(&rot, DirectX::XMMatrixSet(
				DirectX::XMVectorGetX(x), DirectX::XMVectorGetY(x), DirectX::XMVectorGetZ(x), 0,
				DirectX::XMVectorGetX(y), DirectX::XMVectorGetY(y), DirectX::XMVectorGetZ(y), 0,
				DirectX::XMVectorGetX(z), DirectX::XMVectorGetY(z), DirectX::XMVectorGetZ(z), 0,
				0, 0, 0, 1
			));

			// サイズ（太さ 0.1、長さ length）
			DirectX::XMFLOAT3 size = { radius, radius, length };

			//角度(デバッグ用)
			DirectX::XMFLOAT3 angle;

			// yaw（Y回転）
			angle.y = atan2f(dir.m128_f32[0], dir.m128_f32[2]);

			// pitch（X回転）
			angle.x = -asinf(dir.m128_f32[1]);

			angle.z = 0.0f;


			// 描画
			renderer->RenderBox(rc, center, angle, size, DirectX::XMFLOAT4(1, 1, 0, 1));
		}
	}

	//デバッグ用GUI描画
	void DrawDebugGUI();

private:
	/*bool RayCastMesh(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		DirectX::XMFLOAT3& hitPos,
		DirectX::XMFLOAT3& hitNormal
	) const;*/
};

//レーザーの本体
class Laser :public StageObject
{
	public:
	//	Laser() {};
	//	~Laser() {}
	//
	//	//初期化
	//	void Initialize(const DirectX::XMFLOAT3& emitterPos, const DirectX::XMFLOAT3& direction, float maxLength);
	//
	//	//更新処理
	//	void Update(float elapsedTime)override;
	//
	//	//描画処理
	//	void Render(const RenderContext& rc, ModelRenderer* renderer)override;
	//
		//デバッグプリミティブ描画
		void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override
		{
			StageObject::RenderDebugPrimitive(rc, renderer);
			//ビームのデバッグ描画
			beam.RenderDebugPrimitive(rc, renderer);
			renderer->RenderBox(rc, startPos, { 0,0,0 }, { 1, 1, 0.1f }, DirectX::XMFLOAT4(1, 0, 0, 1));
		}

		//デバッグ用GUI描画
		void DrawDebugGUI()
		{
			beam.DrawDebugGUI();
		}
	
	//	//回転 仮
	//	void RotateY(float angle)
	//	{
	//		DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(angle);
	//		DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
	//		dir = DirectX::XMVector3TransformNormal(dir, rot);
	//		DirectX::XMStoreFloat3(&direction, dir);
	//	}
	//
	//	//ON OFF
	//	void SetActive(bool flag) { isActive = flag; }
	//	bool IsActive() const { return isActive; }
	//
	//	//ステージオブジェクトマネージャーセット
	//	void setManager(StageObjectManager* mgr) { manager = mgr; }
	//
	//	//BoxCollider& GetTopCollider() { return beam.GetTopCollider(); }
	//	//BoxCollider& GetSideCollider() { return beam.GetSideCollider(); }
	//
	//private:
	//	//ビーム
	//	LaserBeam beam;
	//
	//
	//	//レーザー情報
	//	DirectX::XMFLOAT3 startPos = { 0,0,0 };
	//	DirectX::XMFLOAT3 direction = { 1,0,0 };
	//	DirectX::XMFLOAT3 endPos = { 0,0,0 };
	//
	//	float maxLength = 20.0f;
	//	bool isActive = true;
	//
	//private:
	//	//レーザーを飛ばす
	//	void Shoot();
	//
	//	//反射ベクトル
	//	DirectX::XMFLOAT3 Reflect(
	//		const DirectX::XMFLOAT3& inDir,
	//		const DirectX::XMFLOAT3& normal
	//	);
	//
	//	//プレイヤー押し戻し
	//	void ResolvePlayerCollision();
	//
	//	//ステージオブジェクトマネージャー
	//	StageObjectManager* manager = nullptr;

public:
	Laser() {}
	~Laser() {}

	void Initialize(
		const DirectX::XMFLOAT3& emitterPos,
		const DirectX::XMFLOAT3& direction,
		float maxLength);

	void Update(float elapsedTime) override;
	void Render(const RenderContext& rc, ModelRenderer* renderer) override;

	void SetActive(bool flag) { isActive = flag; }
	bool IsActive() const { return isActive; }

	void setManager(StageObjectManager* mgr) { manager = mgr; }

	LaserBeam& GetBeam() { return beam; }

private:
	LaserBeam beam;  // ← これがレーザーの本体

	DirectX::XMFLOAT3 startPos;
	DirectX::XMFLOAT3 direction;
	float maxLength = 20.0f;

	bool isActive = true;

	void ResolvePlayerCollision();
	StageObjectManager* manager = nullptr;
};
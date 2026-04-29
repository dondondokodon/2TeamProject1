#pragma once
#include<memory>
#include"StageObject.h"
#include"StageObjectManager.h"
#include"BoxCollider.h"

//レーザーのビーム
class LaserBeam
{
public:
	LaserBeam() {};
	~LaserBeam() {}

	//始点と終点をセット
	void SetPoints(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end
	) {
		startPos = start;
		endPos = end;
		UpdateTransform();
	}

	void SetAngle(const DirectX::XMFLOAT3& a) { angle = a; }
	void setDirection(const DirectX::XMFLOAT3& d) { direction = d; }

	//太さ
	void setWidth(float w) {
		width = w;
		UpdateTransform();
	}

	//更新
	void Update(float elapsedTime);

	//描画
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		renderer->RenderBox(rc, startPos, { 0,0,0 }, { width, width, 0.1f }, DirectX::XMFLOAT4(1, 1, 0, 1));
		//コライダーのデバッグ描画
		renderer->RenderBox(rc, topCollider.GetCenter(), { 0,0,0 }, topCollider.GetSize(), DirectX::XMFLOAT4(1, 0, 0, 1));
		renderer->RenderBox(rc, sideCollider.GetCenter(),{0,0,0}, sideCollider.GetSize(), DirectX::XMFLOAT4(0, 1, 0, 1));
	}

	//ゲッター
	BoxCollider& GetTopCollider() { return topCollider; }
	BoxCollider& GetSideCollider() { return sideCollider; }

	//Collieder更新
	void UpdateColliders();

private:
	DirectX::XMFLOAT3 startPos = { 0,0,0 };
	DirectX::XMFLOAT3 endPos = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 1,0,0 };

	//上に乗れる
	BoxCollider topCollider;
	BoxCollider sideCollider;



	float width = 0.1f;

	//板ポリモデル

	//行列
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	void UpdateTransform();
};

//レーザーの本体
class Laser :public StageObject
{
public:
	Laser() {};
	~Laser() {}

	//初期化
	void Initialize(const DirectX::XMFLOAT3& emitterPos, const DirectX::XMFLOAT3& direction, float maxLength);

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)override
	{
		StageObject::RenderDebugPrimitive(rc, renderer);
		//ビームのデバッグ描画
		beam.RenderDebugPrimitive(rc, renderer);
		renderer->RenderBox(rc, startPos, { 0,0,0 }, { 1, 1, 0.1f }, DirectX::XMFLOAT4(1, 0, 0, 1));
	}

	//回転 仮
	void RotateY(float angle)
	{
		DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(angle);
		DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
		dir = DirectX::XMVector3TransformNormal(dir, rot);
		DirectX::XMStoreFloat3(&direction, dir);
	}

	//ON OFF
	void SetActive(bool flag) { isActive = flag; }
	bool IsActive() const { return isActive; }

	//ステージオブジェクトマネージャーセット
	void setManager(StageObjectManager* mgr) { manager = mgr; }

	BoxCollider& GetTopCollider() { return beam.GetTopCollider(); }
	BoxCollider& GetSideCollider() { return beam.GetSideCollider(); }

private:
	//ビーム
	LaserBeam beam;


	//レーザー情報
	DirectX::XMFLOAT3 startPos = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 1,0,0 };
	DirectX::XMFLOAT3 endPos = { 0,0,0 };

	float maxLength = 20.0f;
	bool isActive = true;

private:
	//レーザーを飛ばす
	void Shoot();

	//反射ベクトル
	DirectX::XMFLOAT3 Reflect(
		const DirectX::XMFLOAT3& inDir,
		const DirectX::XMFLOAT3& normal
	);

	//プレイヤー押し戻し
	void ResolvePlayerCollision();

	//ステージオブジェクトマネージャー
	StageObjectManager* manager = nullptr;
};
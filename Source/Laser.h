#pragma once
#include<memory>
#include"StageObject.h"
#include"StageObjectManager.h"
#include"BoxCollider.h"

//レーザーのビーム
class LaserBeam
{
public:
	LaserBeam();
	~LaserBeam() {}

	//始点と終点をセット
	void SetPoints(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end
	) {
		startPos = start;
		endPos = end;
		updateTransform();
	}

	//太さ
	void setWidth(float w) {
		width = w;
		updateTransform();
	}

	//更新
	void Update(float elapsedTime);

	//描画
	void Render(const RenderContext& rc, ModelRenderer* renderer);

private:
	DirectX::XMFLOAT3 startPos = { 0,0,0 };
	DirectX::XMFLOAT3 endPos = { 0,0,0 };

	float width = 0.1f;

	//板ポリモデル

	//行列
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	void updateTransform();
};

//レーザーの本体
class Laser :public StageObject
{
public:
	Laser();
	~Laser() {}

	//初期化
	void Initialize(const DirectX::XMFLOAT3& emitterPos, const DirectX::XMFLOAT3& direction, float maxLength);

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer)override;

	//回転
	void RotateY(float angle);

	//ON OFF
	void SetActive(bool flag);
	bool IsActive() const;

	void setManager(StageObjectManager* mgr) { manager = mgr; }

private:
	//ビーム
	LaserBeam beam;

	//上に乗れる
	BoxCollider topCollider;

	BoxCollider sideCollider;

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

	//Collieder更新
	void UpdateColliders();

	//プレイヤー押し戻し
	void ResolvePlayerCollision();

	//ステージオブジェクトマネージャー
	StageObjectManager* manager = nullptr;
};
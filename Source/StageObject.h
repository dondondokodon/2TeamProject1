#pragma once
#include "System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include "memory"
#include "RayHitType.h"

class StageObject
{
public:
	StageObject() {}
	~StageObject() {}

	//更新処理
	virtual void Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグプリミティブ描画
	virtual void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
	{
		renderer->RenderBox(rc, position,angle, scale, DirectX::XMFLOAT4(0, 0, 1,1));
	}

	//デバッグ用GUI描画
	virtual void DrawDebugGUI()
	{

	}

	//トランスフォーム取得
	DirectX::XMFLOAT4X4 GetTransform() const { return transform; }

	//モデル取得
	const Model* GetModel() const { return model.get(); }

	//レイヒットタイプ取得
	RayHitType GetRayHitType() const { return type; }

	//ヒット通知のフック
	virtual void OnHit(bool hit) {}

	//セッター
	void SetPosition(const DirectX::XMFLOAT3& pos)
	{
		position = pos;
		UpdateTransform();
	}

	virtual void SetAngle(const DirectX::XMFLOAT3& ang)
	{
		angle = ang;
		UpdateTransform();
	}

	void SetScale(const DirectX::XMFLOAT3& sca)
	{
		scale = sca;
		UpdateTransform();
	}

	void SetModel(const char* modelPath)
	{
		model = std::make_unique<Model>(modelPath);
	}
	
	//ヒットがきちんとヒットしてるか
	virtual RayHitResult ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal)
	{
		RayHitResult result{ true,this,type,hitPos };
		return result;	//基底クラスはとりあえずtrue
	}

protected:

	//行列更新処理
	void UpdateTransform();

	std::unique_ptr<Model> model = nullptr;
	std::unique_ptr<Model> modelBase = nullptr;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	RayHitType type = RayHitType::Stop;
};


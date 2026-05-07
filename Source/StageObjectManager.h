#pragma once
#include<vector>
#include<set>
#include"System/ShapeRenderer.h"
#include "StageObject.h"
//#include"LaserManager.h"

class LaserManager;

class StageObjectManager
{
public:
	~StageObjectManager();

	//シングルトンにしてみる
	static StageObjectManager& Instance()
	{
		static StageObjectManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);
	
	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//ステージオブジェクト登録
	void Register(StageObject* stageObject);

	//ステージオブジェクト全削除
	void Clear();

	//ステージオブジェクト削除
	void Remove(StageObject* stageObject);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//レイキャスト
	bool StageObjectManager::RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		DirectX::XMFLOAT3& hitPos,
		DirectX::XMFLOAT3& normal);

	//ステージオブジェクト取得
	StageObject* GetStageObject(int index) { return stageObjects.at(index).get(); }

	//レーザーマネージャー取得
	LaserManager* GetLaserManager();

	//セッター
	void setLaserManager(LaserManager* mgr);

private:
	StageObjectManager() {}

	std::vector <std::unique_ptr<StageObject>> stageObjects;
	std::set<StageObject*> removes;
	LaserManager* laserManager;	//循環するから前方宣言で使えるようにポインタにする
};


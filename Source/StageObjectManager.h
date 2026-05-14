#pragma once
#include<vector>
#include<set>
#include<memory>
#include"System/ShapeRenderer.h"
#include "StageObject.h"
#include"RayHitType.h"
#include"StageData.h"
#include "Mirror.h"
//#include"LaserManager.h"

class StageGrid;
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

	//ステージデータロード
	void LoadStageData(StageData* data);	//直接もらうほう

	void LoadStageData(int stageNum);	//持ってるほうからロード

	//次のステージに移る処理 true：最後のステージ
	bool NextStage();

	//ステージオブジェクト登録
	void Register(StageObject* stageObject);

	//ステージオブジェクト全削除
	void Clear();

	//リセット
	void Reset();

	//ステージオブジェクト削除
	void Remove(StageObject* stageObject);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//レイキャスト
	RayHitResult StageObjectManager::RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		DirectX::XMFLOAT3& hitPos,
		DirectX::XMFLOAT3& normal);

	//複数同時レイキャスト（軽くするため）
	RayHitResult RayCastAny(
		const DirectX::XMFLOAT3* starts,
		const DirectX::XMFLOAT3* ends,
		int rayCount,
		StageObject* ignoreObject,
		DirectX::XMFLOAT3& hitPos,
		DirectX::XMFLOAT3& normal);

	//ステージの範囲設定
	void SetStageBounds(float minX, float maxX, float minZ, float maxZ)
	{
		stageMinX = minX;
		stageMaxX = maxX;
		stageMinZ = minZ;
		stageMaxZ = maxZ;
	}
	//ステージの範囲取得
	float GetStageMinX() const { return stageMinX; }
	float GetStageMaxX() const { return stageMaxX; }
	float GetStageMinZ() const { return stageMinZ; }
	float GetStageMaxZ() const { return stageMaxZ; }


	//ステージオブジェクト取得
	StageObject* GetStageObject(int index) { return stageObjects.at(index).get(); }

	//ステージオブジェクトサイズ
	int GetStageObjectSize() { return static_cast<int>(stageObjects.size()); };

	//レーザーマネージャー取得
	LaserManager* GetLaserManager();

	//セッター
	void setLaserManager(LaserManager* mgr);

	

private:
	StageObjectManager();

	//ステージの範囲（レーザーが届く範囲）　初期値は適当
	float stageMinX = -25.0f;
	float stageMaxX = 25.0f;
	float stageMinZ = -25.0f;
	float stageMaxZ = 25.0f;

	std::vector <std::unique_ptr<StageObject>> stageObjects;
	std::set<StageObject*> removes;
	LaserManager* laserManager;	//循環するから前方宣言で使えるようにポインタにする
	std::vector<std::unique_ptr<StageData>> stageDatas;
	int nextStageIndex = 0;
	std::vector<StageGrid*> grids;
	std::vector<Mirror*> mirrors;
};


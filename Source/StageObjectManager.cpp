#include "StageObjectManager.h"
#include "Collision.h"
#include "StageGrid.h"

#include"LaserManager.h"
#include"StageData1.h"
#include"StageData2.h"

StageObjectManager::StageObjectManager():laserManager(nullptr) 
{
	stageDatas.push_back(std::make_unique<StageData1>());
	stageDatas.push_back(std::make_unique<StageData2>());
}

StageObjectManager::~StageObjectManager() 
{
	Clear();
	laserManager->Clear();
	delete laserManager; 
}

//リセット
void StageObjectManager::Reset()
{
	nextStageIndex = 0;
	Clear();
	stageDatas.clear();
	stageDatas.push_back(std::make_unique<StageData1>());
	stageDatas.push_back(std::make_unique<StageData2>());
}


//更新処理
void StageObjectManager::Update(float elapsedTime)
{
	// ---------------------------
	// 木箱同士判定
	// ---------------------------
	for (StageGrid* grid : grids)
	{
		grid->CollisionVsStage(*this);

		grid->CollisionVsGrid(grids);

		grid->CollisionVsMirror(mirrors);
	}

	for (auto& stageObject : stageObjects)
	{
		stageObject->Update(elapsedTime);
		// ---------------------------
		// リスト初期化
		// ---------------------------
		grids.clear();
		mirrors.clear();
	}

	// ---------------------------
	// 木箱,鏡収集
	// ---------------------------
	for (auto& stageObject : stageObjects)
	{
		// 木箱
		if (StageGrid* grid =
			dynamic_cast<StageGrid*>(stageObject.get()))
		{
			grids.push_back(grid);
		}

		// 鏡
		if (Mirror* mirror =
			dynamic_cast<Mirror*>(stageObject.get()))
		{
			mirrors.push_back(mirror);
		}
	}


	//破棄処理
	for (auto& stageObject : removes)
	{
		auto it = std::find_if(stageObjects.begin(),
			stageObjects.end(),
			[&](const std::unique_ptr<StageObject>& obj)
			{
				return obj.get() == stageObject;
			});

		if (it != stageObjects.end())
		{
			stageObjects.erase(it);
		}
	}
	removes.clear();

	if (laserManager)
		laserManager->Update(elapsedTime);
}


//描画処理
void StageObjectManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{


	for (auto& stageObject : stageObjects)
	{
		stageObject->Render(rc, renderer);
	}
	
	if (laserManager)
	laserManager->Render(rc, renderer);
}

//ステージデータロード
void StageObjectManager::LoadStageData(StageData* data)
{
	Clear();
	laserManager->Clear();

	for (auto& objData : data->objects)
	{
		StageObject* obj = objData->CreateStageObject();
		if (objData->type == ObjectType::Laser)
		{
			Laser* laser = dynamic_cast<Laser*>(obj);
			laser->setManager(this);
			laserManager->Register(laser);
		}
		else
		Register(obj);
	}

	if (data->MyStage) {
		Register(data->MyStage.release());
	}
}

//ステージデータロード
void StageObjectManager::LoadStageData(int stageNum)
{
	Clear();
	laserManager->Clear();

	for (auto& objData : stageDatas[stageNum]->objects)
	{
		StageObject* obj = objData->CreateStageObject();
		if (objData->type == ObjectType::Laser)
		{
			Laser* laser = dynamic_cast<Laser*>(obj);
			laser->setManager(this);
			laserManager->Register(laser);
		}
		else
			Register(obj);
	}

	Register(stageDatas[stageNum]->MyStage.release());
}


//次のステージに移る処理 true：最後のステージ
bool StageObjectManager::NextStage()
{
	if (nextStageIndex >= stageDatas.size())
	{
		return true;
	}
	LoadStageData(nextStageIndex);
	
	// ステージごとの外に出られない範囲
	if (nextStageIndex == 0)
	{
		// StageData1用
		SetStageBounds(-17.5f, 17.5f, -17.5f, 17.5f);
	}
	else if (nextStageIndex == 1)
	{
		// StageData2用
		SetStageBounds(-17.5f, 17.5f, -17.5f, 17.5f);
	}


	nextStageIndex++;

	return false;
}

//ステージオブジェクト登録
void StageObjectManager::Register(StageObject* stageObject)
{
	stageObjects.emplace_back(stageObject);
}

//ステージオブジェクト全削除
void StageObjectManager::Clear()
{
	laserManager->Clear();
	stageObjects.clear();
}

//ステージオブジェクト削除
void StageObjectManager::Remove(StageObject* stageObject)
{
	//破棄リストに追加
	removes.insert(stageObject);
}

//デバッグプリミティブ描画
void StageObjectManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (auto& stageObject : stageObjects)
	{
		stageObject->RenderDebugPrimitive(rc, renderer);
	}

	if (laserManager)
	laserManager->RenderDebugPrimitive(rc, renderer);
}

//デバッグ用GUI描画
void StageObjectManager::DrawDebugGUI()
{
	for (auto& stageObject : stageObjects)
	{
		stageObject->DrawDebugGUI();
	}

	if (laserManager)
	laserManager->DrawDebugGUI();
}

//レイキャスト
RayHitResult StageObjectManager::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	DirectX::XMFLOAT3& hitPos,
	DirectX::XMFLOAT3& normal)
{
	RayHitResult result =
	{
		false,
		nullptr,
		RayHitType::Stop,
		{0,0,0}
	};

	// 一番近い距離
	float nearestDistSq = FLT_MAX;

	for (auto& obj : stageObjects)
	{
		DirectX::XMFLOAT3 tempHitPos;
		DirectX::XMFLOAT3 tempNormal;

		if (Collision::RayCast(
			start,
			end,
			obj->GetTransform(),
			obj->GetModel(),
			tempHitPos,
			tempNormal))
		{
		
			// start → hitPos の距離
			float dx = tempHitPos.x - start.x;
			float dy = tempHitPos.y - start.y;
			float dz = tempHitPos.z - start.z;

			float distSq =
				dx * dx +
				dy * dy +
				dz * dz;

			// より近い物だけ保存
			if (distSq < nearestDistSq)
			{
				nearestDistSq = distSq;

				result.hit = true;
				result.object = obj.get();
				result.type = obj->GetRayHitType();
				result.hitPos = tempHitPos;

				// 出力用
				hitPos = tempHitPos;
				normal = tempNormal;
			}
		}
	}

	return result;
}

//レイキャスト複数
RayHitResult StageObjectManager::RayCastAny(
	const DirectX::XMFLOAT3* starts,
	const DirectX::XMFLOAT3* ends,
	int rayCount,
	StageObject* ignoreObject,
	DirectX::XMFLOAT3& hitPos,
	DirectX::XMFLOAT3& normal)
{
	RayHitResult result = { false, nullptr, RayHitType::Stop,{0,0,0} };

	for (auto& obj : stageObjects)
	{
		if (obj.get() == ignoreObject) continue;

		for (int i = 0; i < rayCount; ++i)
		{
			if (Collision::RayCast(
				starts[i],
				ends[i],
				obj->GetTransform(),
				obj->GetModel(),
				hitPos,
				normal))
			{
				result.hit = true;
				result.object = obj.get();
				result.type = obj->GetRayHitType();
				result.hitPos = hitPos;
				return result;
			}
		}
	}

	return result;
}


LaserManager* StageObjectManager::GetLaserManager() { return laserManager; }

void StageObjectManager::setLaserManager(LaserManager* mgr)
{
	if (laserManager)
	{
		//laserManager->Clear();
		delete laserManager;
		laserManager = nullptr;
	}
	laserManager = mgr;
}
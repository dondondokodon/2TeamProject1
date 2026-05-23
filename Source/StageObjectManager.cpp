#include "StageObjectManager.h"
#include "Collision.h"
#include "StageGrid.h"

#include"LaserManager.h"
#include"StageData1.h"
#include"StageData2.h"

#include "Flag.h"

StageObjectManager::StageObjectManager()
{
	stageDatas.push_back(std::make_unique<StageData1>());
	stageDatas.push_back(std::make_unique<StageData2>());
	stageDatas.push_back(std::make_unique<StageData3>());
	stageDatas.push_back(std::make_unique<StageData4>());
}

StageObjectManager::~StageObjectManager() 
{
	Clear();
}

//リセット
void StageObjectManager::Reset()
{
	//nextStageIndex = 0;
	Clear();
	stageDatas.clear();
	stageDatas.push_back(std::make_unique<StageData1>());
	stageDatas.push_back(std::make_unique<StageData2>());
	stageDatas.push_back(std::make_unique<StageData3>());
	stageDatas.push_back(std::make_unique<StageData4>());
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

		grid->CollisionVsFloor(*this);

		grid->CollisionVsStairs(stairsList);
	}

	for (auto& stageObject : stageObjects)
	{
		stageObject->Update(elapsedTime);
		
	}
	
	// ---------------------------
	// リスト初期化
	// ---------------------------
	grids.clear();
	mirrors.clear();
	stairsList.clear();

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

		if (Stairs* stairs =
			dynamic_cast<Stairs*>(stageObject.get()))
		{
			stairsList.push_back(stairs);
		}
    }

	// ---------------------------
	// 木箱同士判定
	// ---------------------------
	for (StageGrid* grid : grids)
	{
		grid->CollisionVsStage(*this);

		grid->CollisionVsGrid(grids);

		grid->CollisionVsMirror(mirrors);

		grid->CollisionVsFloor(*this);

		grid->CollisionVsStairs(stairsList);
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
	stageFloor->Render(rc, renderer);
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
		if (obj == nullptr)
			return;

		if (objData->type == ObjectType::Laser)
		{
			Laser* laser = dynamic_cast<Laser*>(obj);
			laser->setManager(this);
			laserManager->Register(laser);
		}
		else {
			Register(obj);
		}
	}

	Register(stageDatas[stageNum]->MyStage.release());
	stageFloor = std::move(stageDatas[stageNum]->StageFloor);
}


//次のステージに移る処理 true：最後のステージ
bool StageObjectManager::NextStage()
{
	if (nextStageIndex >= stageDatas.size())
	{
		return true;
	}

	LoadStageData(nextStageIndex);

	// 各ステージの場外判定の広さ。
	// ステージを増やした時は、ここに数値を1つ足すだけで調整できる。
	const float stageBounds[] =
	{
		20.5f, // チュートリアル
		17.5f, // ステージ1
		20.5f, // ステージ2
		20.5f, // ステージ3
	};

	int boundsIndex = nextStageIndex;
	if (boundsIndex >= _countof(stageBounds))
	{
		boundsIndex = _countof(stageBounds) - 1;
	}

	float halfSize = stageBounds[boundsIndex];

	SetStageBounds(-halfSize, halfSize, -halfSize, halfSize);

	nextStageIndex++;

	return false;
}

//ステージオブジェクト登録
void StageObjectManager::Register(StageObject* stageObject)
{
	std::unique_ptr<StageObject> p(stageObject);

	stageObjects.emplace_back(std::move(p));
}

//ステージオブジェクト全削除
void StageObjectManager::Clear()
{
	stageObjects.clear();
	mirrors.clear();
	grids.clear();
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


	if (ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_None))
	{
			//位置
			ImGui::InputInt("stageindex", &nextStageIndex);
	}
	ImGui::End();
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
		if(obj->GetRayHitType() == RayHitType::None)continue;
		if (obj->GetModel() == nullptr)continue;

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
				result.hitNormal = tempNormal;

				// 出力用
				hitPos = tempHitPos;
				normal = tempNormal;
			}
		}
	}

	return result;
}

//レイキャスト
RayHitResult StageObjectManager::RayCastFloor(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end
	)
{
	RayHitResult result =
	{
		false,
		nullptr,
		RayHitType::Stop,
		{0,0,0}
	};


	DirectX::XMFLOAT3 tempHitPos;
	DirectX::XMFLOAT3 tempNormal;

	// 一番近い距離
	float nearestDistSq = FLT_MAX;

	if (Collision::RayCast(
		start,
		end,
		stageFloor->GetTransform(),
		stageFloor->GetModel(),
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
			result.object = stageFloor.get();
			result.type = stageFloor->GetRayHitType();
			result.hitPos = tempHitPos;
			result.hitNormal = tempNormal;
		}

		
	}

	for (auto& obj : stageObjects)
	{
		DirectX::XMFLOAT3 tempHitPos;
		DirectX::XMFLOAT3 tempNormal;
		if (obj->GetRayHitType() == RayHitType::None)continue;
		if (obj->GetModel() == nullptr)continue;

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
				result.hitNormal = tempNormal;
			}
		}
	}
	return result;
}

//ステージ床だけとのレイキャスト
//木箱の落下チェック用。stageObjects は見ず、ステージ床だけを見る。
//箱・鏡・壁との衝突は別の処理で見ているので、ここでは床の有無だけ確認する。
RayHitResult StageObjectManager::RayCastStageFloorOnly(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end
)
{
	RayHitResult result =
	{
		false,
		nullptr,
		RayHitType::Stop,
		{0,0,0}
	};

	if (!stageFloor || !stageFloor->GetModel()) return result;

	DirectX::XMFLOAT3 tempHitPos;
	DirectX::XMFLOAT3 tempNormal;

	// stageFloor だけに RayCast する。
	// RayCastFloor のように stageObjects 全体は回さないので、木箱移動時の負荷を減らせる。
	if (Collision::RayCast(
		start,
		end,
		stageFloor->GetTransform(),
		stageFloor->GetModel(),
		tempHitPos,
		tempNormal))
	{
		result.hit = true;
		result.object = stageFloor.get();
		result.type = stageFloor->GetRayHitType();
		result.hitPos = tempHitPos;
		result.hitNormal = tempNormal;
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
		if (obj->GetModel() == nullptr) continue;

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


LaserManager* StageObjectManager::GetLaserManager()
{
	return laserManager.get();
}

void StageObjectManager::setLaserManager(std::unique_ptr<LaserManager> mgr)
{
    laserManager = std::move(mgr);
}

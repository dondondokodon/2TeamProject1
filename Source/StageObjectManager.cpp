#include "StageObjectManager.h"
#include "Collision.h"

#include"LaserManager.h"

StageObjectManager::~StageObjectManager() { Clear();delete laserManager; }

//更新処理
void StageObjectManager::Update(float elapsedTime)
{
	for (auto& stageObject : stageObjects)
	{
		stageObject->Update(elapsedTime);
	}

	//破棄処理
	for (auto& stageObject : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		auto it = std::find_if(stageObjects.begin(),
			stageObjects.end(),
			[&](const std::unique_ptr<StageObject>& obj) { return obj.get() == stageObject; });
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
		StageObject* obj = objData.CreateStageObject();
		if (objData.type == ObjectType::Laser)
		{
			Laser* laser = dynamic_cast<Laser*>(obj);
			laser->setManager(this);
			laserManager->Register(laser);
		}
		else
		Register(obj);
	}

	Register(data->MyStage);
}

//ステージオブジェクト登録
void StageObjectManager::Register(StageObject* stageObject)
{
	stageObjects.emplace_back(stageObject);
}

//ステージオブジェクト全削除
void StageObjectManager::Clear()
{
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
	RayHitResult result = { false, nullptr, RayHitType::Stop,{0,0,0} };
	for (auto& obj : stageObjects)
	{
		if (Collision::RayCast(
			start,
			end,
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

	return result;
}

LaserManager* StageObjectManager::GetLaserManager() { return laserManager; }

void StageObjectManager::setLaserManager(LaserManager* mgr)
{
	if (laserManager)
	{
		delete laserManager;
		laserManager = nullptr;
	}
	laserManager = mgr;
}
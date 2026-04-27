#include "StageObjectManager.h"
#include "Collision.h"

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

	laserManager.Update(elapsedTime);
}

//描画処理
void StageObjectManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& stageObject : stageObjects)
	{
		stageObject->Render(rc, renderer);
	}
	
	laserManager.Render(rc, renderer);
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

	laserManager.RenderDebugPrimitive(rc, renderer);
}

//レイキャスト
bool StageObjectManager::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	DirectX::XMFLOAT3& hitPos,
	DirectX::XMFLOAT3& normal)
{
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
			return true;
		}
	}

	return false;
}
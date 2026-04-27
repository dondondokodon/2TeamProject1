#include "LaserManager.h"

//更新処理
void LaserManager::Update(float elapsedTime)
{
	for (auto& Laser : Lasers)
	{
		Laser->Update(elapsedTime);
	}

	//破棄処理
	for (auto& Laser : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		auto it = std::find_if(Lasers.begin(),
			Lasers.end(),
			[&](const std::unique_ptr<StageObject>& obj) { return obj.get() == Laser; });
		if (it != Lasers.end())
		{
			Lasers.erase(it);
		}
	}
	removes.clear();
}

//描画処理
void LaserManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& stageObject : Lasers)
	{
		stageObject->Render(rc, renderer);
	}
}

//ステージオブジェクト登録
void LaserManager::Register(Laser* stageObject)
{
	Lasers.emplace_back(stageObject);
}

//ステージオブジェクト全削除
void LaserManager::Clear()
{
	Lasers.clear();
}

//ステージオブジェクト削除
void LaserManager::Remove(Laser* stageObject)
{
	//破棄リストに追加
	removes.insert(stageObject);
}

//デバッグプリミティブ描画
void LaserManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (auto& stageObject : Lasers)
	{
		stageObject->RenderDebugPrimitive(rc, renderer);
	}
}
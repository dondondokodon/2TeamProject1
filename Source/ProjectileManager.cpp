#include"ProjectileManager.h"

//コンストラクタ
ProjectileManager::ProjectileManager()
{
}

//デストラクタ
ProjectileManager::~ProjectileManager()
{
	Clear();
}

//弾丸削除
void ProjectileManager::Remove(Projectile* projectile)
{
	//破棄リストに追加
	removes.insert(projectile);
}


//更新処理
void ProjectileManager::Update(float elapsedTime)
{
	//更新処理
	for (Projectile* p : projectiles)
	{
		p->Update(elapsedTime);
	}

	//破棄処理
	//※projectilesの範囲for文の中でerase()すると不具合が発生してしまうため、
	//　更新処理が終わった後に破棄リストに積まれたオブジェクトを削除。
	for (Projectile* projectile : removes)
	{
		//std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(),
			projectiles.end(), projectile);
		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}
		delete projectile;
	}
	removes.clear();
}

//描画処理
void ProjectileManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (Projectile* p : projectiles)
	{
		p->Render(rc,renderer);
	}
}

//デバッグプリミティブ描画
void ProjectileManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (Projectile* p : projectiles)
	{
		p->RenderDebugPrimitive(rc, renderer);
	}
}

//弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}

//弾丸全削除
void ProjectileManager::Clear()
{
	for (Projectile* p : projectiles)
	{
		delete p;
	}
	projectiles.clear();
}
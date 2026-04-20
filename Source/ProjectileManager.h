#pragma once

#include<vector>
#include"Projectile.h"
#include<set>

//弾丸マネージャー
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//弾丸登録
	void Register(Projectile* projectile);

	//弾丸全削除
	void Clear();

	//弾丸数取得
	int GetProjectileCount()const { return static_cast<int>(projectiles.size()); }

	//弾丸取得
	Projectile* GetProjectile(int index) { return projectiles.at(index); }

	//弾丸削除
	void Remove(Projectile* projectile);
private:
	std::vector<Projectile*>	projectiles;

	std::set<Projectile*>	removes;
};
#pragma once
#include"Laser.h"
#include<vector>
class LaserManager
{
public:
	LaserManager() {}
	~LaserManager() { Clear(); }

	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(const RenderContext& rc, ModelRenderer* renderer);

	//レーザー登録
	void Register(Laser* laser);

	//レーザー全削除
	void Clear();

	//レーザー削除
	void Remove(Laser* laser);

	//デバッグプリミティブ描画
	void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

	//レーザー取得
	Laser* GetLaser(int index) { return Lasers.at(index).get(); }

	//レーザー数取得
	int GetLaserCount() { return static_cast<int>(Lasers.size()); }

private:
	std::vector <std::unique_ptr<Laser>> Lasers;
	std::set<Laser*> removes;
};


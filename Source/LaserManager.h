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

	//デバッグ用GUI描画
	void DrawDebugGUI();

	//レーザー取得
	Laser* GetLaser(int index) { return Lasers.at(index).get(); }

	//レーザー数取得
	int GetLaserCount() { return static_cast<int>(Lasers.size()); }

	//回転しているか取得
	bool GetIsRotating() { return isRotating; }

private:
	std::vector <std::unique_ptr<Laser>> Lasers;
	std::set<Laser*> removes;

	float targetAngleY = 0.0f;
	float currentAngleY = 0.0f;
	bool isRotating = false;

};


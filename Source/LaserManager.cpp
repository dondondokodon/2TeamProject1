#include "LaserManager.h"
#include "System/GamePad.h"
#include "System/Input.h"

//更新処理
void LaserManager::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	// --- 回転入力処理 ---
	float step = DirectX::XM_PI / 4.0f;

	if (!isRotating)
	{
		if (gamePad.GetButtonDown() & GamePad::BTN_LEFT_SHOULDER)
		{
			targetAngleY -= step;
			isRotating = true;
		}
		if (gamePad.GetButtonDown() & GamePad::BTN_RIGHT_SHOULDER)
		{
			targetAngleY += step;
			isRotating = true;
		}
	}

	float diff = targetAngleY - currentAngleY;

	if (fabs(diff) < 0.001f)
	{
		float snap = DirectX::XM_PI / 4.0f;
		int index = (int)round(targetAngleY / snap);

		// 隗貞ｺｦ繧・0 ・・2PI 縺ｮ遽・峇縺ｫ豁｣隕丞喧縺吶ｋ
		// 8繧ｹ繝・ャ繝励〒荳蜻ｨ(2PI)縺ｪ縺ｮ縺ｧ縲・縺ｮ蜑ｰ菴吶ｒ蜿悶ｋ
		// index 縺後・繧､繝翫せ縺ｫ縺ｪ縺｣縺ｦ繧よｭ｣縺ｮ謨ｰ縺ｫ縺ｪ繧九ｈ縺・↓險育ｮ・
		index = (index % 8 + 8) % 8;

		// 豁｣縺励￥ 0, 0.5PI, 1.0PI... 縺ｨ繧ｫ繝√ャ縺ｨ縺励◆隗貞ｺｦ繧貞・逕滓・
		targetAngleY = (float)index * snap;
		currentAngleY = targetAngleY;
		isRotating = false;

		DirectX::XMFLOAT3 center = { 0,0,0 };
		for (auto& laser : Lasers) {
			laser->UpdateTransformByAngle(center, currentAngleY);

			// 蠢ｵ縺ｮ縺溘ａ縲∝ｺｧ讓呵・菴薙ｒ蟆乗焚轤ｹ隨ｬ3菴阪〒荳ｸ繧√※縺励∪縺・
			auto& pos = laser->GetStartPos();
			pos.x = round(pos.x * 1000.0f) / 1000.0f;
			pos.y = round(pos.y * 1000.0f) / 1000.0f;
			pos.z = round(pos.z * 1000.0f) / 1000.0f;

			// 蜷代″(beam.direction)繧よ峩譁ｰ縺輔ｌ繧九ｈ縺・↓蜿肴丐
			laser->GetBeam().origin = pos;
		}

	}
	else
	{
		float dir = (diff > 0.0f) ? 1.0f : -1.0f;
		float delta = (DirectX::XM_PI / 2.0f) * elapsedTime * dir;

		if (fabs(delta) > fabs(diff))
			delta = diff;

		currentAngleY += delta;

		// --- 全レーザーを回転 ---
		DirectX::XMFLOAT3 center = { 0,0,0 };
		for (auto& laser : Lasers)
		{
			laser->UpdateTransformByAngle(center, currentAngleY);
			laser->SetRotating(true);
		}
	}

	if (!isRotating)
	{
		for (auto& laser : Lasers)
			laser->SetRotating(false);
	}

	// --- 各レーザーの更新 ---
	for (auto& laser : Lasers)
	{
		laser->Update(elapsedTime);
	}

	// --- 破棄処理 ---
	for (auto& laser : removes)
	{
		auto it = std::find_if(Lasers.begin(), Lasers.end(),
			[&](const std::unique_ptr<Laser>& ptr) { return ptr.get() == laser; });
		if (it != Lasers.end())
			Lasers.erase(it);
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
	targetAngleY = 0.0f;
	currentAngleY = 0.0f;
	isRotating = false;
	for (auto& laser : Lasers)
	{
		laser->GetBeam().StopEffect();
	}
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

//デバッグ用GUI描画
void LaserManager::DrawDebugGUI()
{
	for (auto& laser : Lasers)
	{
		laser->DrawDebugGUI();
	}
}
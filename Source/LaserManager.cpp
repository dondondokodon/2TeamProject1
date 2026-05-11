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
        diff = 0.0f;
        currentAngleY = targetAngleY;
        isRotating = false;
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
            laser->RotateAroundCenter(center, delta);
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
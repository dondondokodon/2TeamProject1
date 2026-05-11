#include "LaserManager.h"
#include "System/GamePad.h"
#include "System/Input.h"

//XVˆ—
void LaserManager::Update(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    // --- ‰ñ“]“ü—Íˆ— ---
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

        // è§’åº¦ã‚’ 0 ï½ 2PI ã®ç¯„å›²ã«æ­£è¦åŒ–ã™ã‚‹
        // 8ã‚¹ãƒ†ãƒƒãƒ—ã§ä¸€å‘¨(2PI)ãªã®ã§ã€8ã®å‰°ä½™ã‚’å–ã‚‹
        // index ãŒãƒã‚¤ãƒŠã‚¹ã«ãªã£ã¦ã‚‚æ­£ã®æ•°ã«ãªã‚‹ã‚ˆã†ã«è¨ˆç®—
        index = (index % 8 + 8) % 8;

        // æ­£ã—ã 0, 0.5PI, 1.0PI... ã¨ã‚«ãƒãƒƒã¨ã—ãŸè§’åº¦ã‚’å†ç”Ÿæˆ
        targetAngleY = (float)index * snap;
        currentAngleY = targetAngleY;
        isRotating = false;

        DirectX::XMFLOAT3 center = { 0,0,0 };
        for (auto& laser : Lasers) {
            laser->UpdateTransformByAngle(center, currentAngleY);

            // å¿µã®ãŸã‚ã€åº§æ¨™è‡ªä½“ã‚’å°æ•°ç‚¹ç¬¬3ä½ã§ä¸¸ã‚ã¦ã—ã¾ã†
            auto& pos = laser->GetStartPos();
            pos.x = round(pos.x * 1000.0f) / 1000.0f;
            pos.y = round(pos.y * 1000.0f) / 1000.0f;
            pos.z = round(pos.z * 1000.0f) / 1000.0f;

            // å‘ã(beam.direction)ã‚‚æ›´æ–°ã•ã‚Œã‚‹ã‚ˆã†ã«åæ˜ 
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

        // --- ‘SƒŒ[ƒU[‚ğ‰ñ“] ---
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

    // --- ŠeƒŒ[ƒU[‚ÌXV ---
    for (auto& laser : Lasers)
    {
        laser->Update(elapsedTime);
    }

    // --- ”jŠüˆ— ---
    for (auto& laser : removes)
    {
        auto it = std::find_if(Lasers.begin(), Lasers.end(),
            [&](const std::unique_ptr<Laser>& ptr) { return ptr.get() == laser; });
        if (it != Lasers.end())
            Lasers.erase(it);
    }
    removes.clear();
}


//•`‰æˆ—
void LaserManager::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	for (auto& stageObject : Lasers)
	{
		stageObject->Render(rc, renderer);
	}
}

//ƒXƒe[ƒWƒIƒuƒWƒFƒNƒg“o˜^
void LaserManager::Register(Laser* stageObject)
{
	Lasers.emplace_back(stageObject);
}

//ƒXƒe[ƒWƒIƒuƒWƒFƒNƒg‘Síœ
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

//ƒXƒe[ƒWƒIƒuƒWƒFƒNƒgíœ
void LaserManager::Remove(Laser* stageObject)
{
	//”jŠüƒŠƒXƒg‚É’Ç‰Á
	removes.insert(stageObject);
}

//ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
void LaserManager::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
	for (auto& stageObject : Lasers)
	{
		stageObject->RenderDebugPrimitive(rc, renderer);
	}
}

//ƒfƒoƒbƒO—pGUI•`‰æ
void LaserManager::DrawDebugGUI()
{
	for (auto& laser : Lasers)
	{
		laser->DrawDebugGUI();
	}
}
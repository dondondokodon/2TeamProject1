#pragma once
#include "StageObject.h"
#include "Player.h"
#include "Collision.h"

class Mirror : public StageObject
{
public:
    Mirror();
    ~Mirror();

    void Update(float elapsedTime) override;
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;
    void CollisionVsPlayer(Player& p);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;
    void DrawDebugGUI();
    RayHitResult ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal)override;

    void SetAngle(const DirectX::XMFLOAT3& ang)override
    {
        angle = ang;
        targetAngleY = ang.y;
        UpdateTransform();
    }

    
private:
    DirectX::XMFLOAT3 aabbMin;
    DirectX::XMFLOAT3 aabbMax;

    bool isTouchingPlayer = false;
    bool prevU = false;
    bool prevO = false;
    float targetAngleY = 0.0f;
    bool isRotating = false;


};

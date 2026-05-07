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


private:
    DirectX::XMFLOAT3 aabbMin;
    DirectX::XMFLOAT3 aabbMax;

    bool isTouchingPlayer = false;
    bool prevU = false;
    bool prevO = false;
};

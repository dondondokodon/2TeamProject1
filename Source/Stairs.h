#pragma once
#include "StageObject.h"
#include "Player.h"
#include "imgui.h"
#include "Collision.h"

class Stairs : public StageObject
{
public:
    Stairs();
    ~Stairs();

    void Update(float elapsedTime) override;
    void Render(const RenderContext& rc, ModelRenderer* renderer) override;
    void CollisionVsPlayer(Player& p);
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer) override;
    void DrawDebugGUI() override;

private:
    Model* model = nullptr;




};


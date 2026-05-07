#include "Mirror.h"

Mirror::Mirror()
{
   
    model = new Model("Data/Model/Objects/Mirror/mirror.mdl");

    // 初期トランスフォーム
    position = { 0.0f, 1.2f, 0.0f };
    angle = { 0.0f, 0.0f, 0.0f };
    scale = { 1.0f, 1.0f, 1.0f };

    UpdateTransform();
}

Mirror::~Mirror()
{
    delete model;
}

void Mirror::Update(float elapsedTime)
{
    const float step = DirectX::XM_PI / 4.0f;

    bool nowU = (GetAsyncKeyState('U') & 0x8000);
    bool nowO = (GetAsyncKeyState('O') & 0x8000);

    bool trgU = (nowU && !prevU);
    bool trgO = (nowO && !prevO);

    if (trgU) angle.y += step;
    if (trgO) angle.y -= step;

    UpdateTransform();

    // ★★★ AABB 更新（これが無いとクラッシュする） ★★★
    aabbMin = {
        position.x - 0.6f * scale.x,
        position.y - 1.5f * scale.y,
        position.z - 0.2f * scale.z
    };

    aabbMax = {
        position.x + 0.6f * scale.x,
        position.y + 1.5f * scale.y,
        position.z + 0.3f * scale.z
    };

    prevU = nowU;
    prevO = nowO;

    isTouchingPlayer = false;
}

void Mirror::CollisionVsPlayer(Player& p)
{
    DirectX::XMFLOAT3 push;

    if (Collision::IntersectCylinderVsAABB(
        p.GetPosition(),
        p.GetRadius(),
        p.GetHeight(),
        aabbMin,
        aabbMax,
        push))
    {
        auto pos = p.GetPosition();
        pos.x += push.x;
        pos.z += push.z;
        p.SetPosition(pos);

        isTouchingPlayer = true;
    }
}

void Mirror::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    // スケール・回転・平行移動
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
        angle.x,
        angle.y,
        angle.z
    );
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMMATRIX M = S * R * T;

    DirectX::XMFLOAT4X4 transform;
    DirectX::XMStoreFloat4x4(&transform, M);

    renderer->Render(rc, transform, model, ShaderId::Lambert);
}

void Mirror::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    // AABB の中心
    DirectX::XMFLOAT3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.7f,
        (aabbMin.z + aabbMax.z) * 0.1f
    };

    // AABB のサイズ
    DirectX::XMFLOAT3 size = {
        0.6,
        1.5,
        0.2
    };

    renderer->RenderBox(
        rc,
        center,   // ★ AABB の中心
        angle,
        size,     // ★ AABB の大きさ
        { 1, 0, 0, 1 }
    );
}


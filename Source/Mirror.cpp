#include "Mirror.h"
#include<imgui.h>

Mirror::Mirror()
{
   
    model = std::make_unique<Model>("Data/Model/Objects/Mirror/mirror.mdl");
    modelBase = std::make_unique<Model>("Data/Model/Objects/Mirror/mirror_base.mdl");

    // 初期トランスフォーム
    position = { 0.0f, 1.2f, 0.0f };
    angle = { 0.0f, 0.0f, 0.0f };
    scale = { 1.0f, 1.0f, 1.0f };

    type = RayHitType::reflection;

    UpdateTransform();
}

Mirror::~Mirror()
{
    
}

void Mirror::Update(float elapsedTime)
{
    const float step = DirectX::XM_PI / 4.0f;
    const float rotateSpeed = 6.0f;

    bool nowU = (GetAsyncKeyState('U') & 0x8000);
    bool nowO = (GetAsyncKeyState('O') & 0x8000);

    bool trgU = (nowU && !prevU);
    bool trgO = (nowO && !prevO);

    if (trgU)
    {
        targetAngleY += step;
        isRotating = true;
    }

    if (trgO)
    {
        targetAngleY -= step;
        isRotating = true;
    }

    // -----------------------------
    // -π ～ π に正規化（target）
    // -----------------------------
    while (targetAngleY > DirectX::XM_PI)
        targetAngleY -= DirectX::XM_2PI;

    while (targetAngleY < -DirectX::XM_PI)
        targetAngleY += DirectX::XM_2PI;

    // -----------------------------
    // 最短回転補間
    // -----------------------------
    float diff = targetAngleY - angle.y;

    while (diff > DirectX::XM_PI)
        diff -= DirectX::XM_2PI;

    while (diff < -DirectX::XM_PI)
        diff += DirectX::XM_2PI;

    angle.y += diff * rotateSpeed * elapsedTime;

    // -----------------------------
    // 到達判定
    // -----------------------------
    if (fabs(diff) < 0.01f)
    {
        angle.y = targetAngleY;
        isRotating = false;
    }

    // -----------------------------
    // angleも正規化
    // -----------------------------
    while (angle.y > DirectX::XM_PI)
        angle.y -= DirectX::XM_2PI;

    while (angle.y < -DirectX::XM_PI)
        angle.y += DirectX::XM_2PI;

    UpdateTransform();

    const float halfX = 0.6f * scale.x;
    const float halfY = 1.5f * scale.y;
    const float halfZ = 0.2f * scale.z;

    aabbMin = {
        position.x - halfX,
        position.y - halfY,
        position.z - halfZ
    };

    aabbMax = {
        position.x + halfX,
        position.y + halfY,
        position.z + halfZ
    };

    prevU = nowU;
    prevO = nowO;
    isTouchingPlayer = false;

    // 0° を 180° と同じ扱いにする
    float deg = DirectX::XMConvertToDegrees(angle.y);

    // 0° ±1° の範囲なら 180° にする
    if (fabs(deg) < 1.0f)
    {
        angle.y = DirectX::XMConvertToRadians(180.0f);
        targetAngleY = angle.y;
        UpdateTransform();
    }


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

    renderer->Render(rc, transform, model.get(), ShaderId::Lambert);

    renderer->Render(rc, transform, modelBase.get(), ShaderId::Lambert);


}

void Mirror::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    if (!renderer) return;

    DirectX::XMFLOAT3 size = {
        0.6f ,
        1.5f ,
        0.2f
    };

    // AABB
    renderer->RenderBox(
        rc,
        position,
        angle,
        size,
        { 1, 0, 0, 1 }
    );

    // -------------------------
    // 法線の可視化（球で描く）
    // -------------------------
    DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&transform);

    // モデルのローカル +Z を法線として描画
    DirectX::XMVECTOR N = DirectX::XMVectorSet(0, 0, 1, 0);
    N = DirectX::XMVector3TransformNormal(N, mat);
    N = DirectX::XMVector3Normalize(N);

    DirectX::XMFLOAT3 n3;
    DirectX::XMStoreFloat3(&n3, N);

    DirectX::XMFLOAT3 endPos = {
        position.x + n3.x,
        position.y + n3.y,
        position.z + n3.z
    };

    // 法線の先端に小さい球を描く
    renderer->RenderSphere(
        rc,
        endPos,
        0.1f,
        { 0, 1, 0, 1 }
    );
}


RayHitResult Mirror::ReallyHit(
    DirectX::XMFLOAT3 dir,
    DirectX::XMFLOAT3 hitPos,
    DirectX::XMFLOAT3 hitNormal)
{
    if (isRotating)
    {
        return RayHitResult{ true, this, RayHitType::Stop, hitPos };
    }

    // -----------------------------------
    // 鏡の法線（transform から取得）
    // ここでは「モデルのローカル +Z を鏡の表面法線」と仮定
    // -----------------------------------
    DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&transform);

    DirectX::XMVECTOR N = DirectX::XMVectorSet(0, 0, 1, 0); // ← 必ず定義する
    N = DirectX::XMVector3TransformNormal(N, mat);
    N = DirectX::XMVector3Normalize(N);

    DirectX::XMVECTOR D = DirectX::XMLoadFloat3(&dir);
    D = DirectX::XMVector3Normalize(D);

    float dot = 0.0f;
    DirectX::XMStoreFloat(
        &dot,
        DirectX::XMVector3Dot(
            DirectX::XMVectorNegate(D),
            N));

    RayHitResult result{ true, this, type, hitPos };

    // 鏡に対してほぼ平行なら反射させず停止
    if (fabs(dot) < 0.2f)
    {
        result.type = RayHitType::None;
    }

    return result;
}


void Mirror::DrawDebugGUI()
{
    if (ImGui::Begin("mirror"))
    {
        if (ImGui::CollapsingHeader("mirror", ImGuiTreeNodeFlags_DefaultOpen))
        {
            float angleDeg[3] =
            {
                DirectX::XMConvertToDegrees(angle.x),
                DirectX::XMConvertToDegrees(angle.y),
                DirectX::XMConvertToDegrees(angle.z)
            };

            if (ImGui::InputFloat3("angle", angleDeg))
            {
                angle.x = DirectX::XMConvertToRadians(angleDeg[0]);
                angle.y = DirectX::XMConvertToRadians(angleDeg[1]);
                angle.z = DirectX::XMConvertToRadians(angleDeg[2]);

                targetAngleY = angle.y; // ★重要
            }
        }
    }
    ImGui::End();
}
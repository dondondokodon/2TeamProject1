#include "Mirror.h"
#include<imgui.h>

Mirror::Mirror()
{
   
    SetModel("Data/Model/Objects/Mirror/mirror.mdl");

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

    bool nowU = (GetAsyncKeyState('U') & 0x8000);
    bool nowO = (GetAsyncKeyState('O') & 0x8000);

    bool trgU = (nowU && !prevU);
    bool trgO = (nowO && !prevO);

    if (trgU) angle.y += step;
    if (trgO) angle.y -= step;

    // AABB の半径（モデルに合わせて調整）
    const float halfX = 0.6f * scale.x;
    const float halfY = 1.5f * scale.y;
    const float halfZ = 0.2f * scale.z;

    // AABB を position を中心に生成
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

    UpdateTransform();
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

    renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

void Mirror::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    DirectX::XMFLOAT3 size = {
        0.6f ,
        1.5f ,
        0.2f 
    };

    renderer->RenderBox(
        rc,
        position,   // ← モデルと同じ位置
        angle,      // ← モデルと同じ回転
        size,
        { 1, 0, 0, 1 }
    );
}

RayHitResult Mirror::ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal)
{
    // 角度による判定 (内積)
    // 鏡の正面ベクトル (forward) と レイの方向 (dir) が向かい合っているか
    DirectX::XMFLOAT3 forward = { transform._31,transform._32,transform._33 };
    DirectX::XMVECTOR Forward = DirectX::XMLoadFloat3(&forward);
    DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&dir);
    DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hitNormal);

    // dot < 0 なら向かい合っている（正面から当たっている）
    float dot = 0.0f;
    DirectX::XMStoreFloat(&dot, DirectX::XMVector3Dot(DirectX::XMVectorNegate(dirVec), Forward));

    RayHitResult result{ true,this,type,hitPos };
    
    //前と後ろからはOK
    if (dot < 0.1f&&dot>-0.1f)
    {
        result.type = RayHitType::Stop;
    }

    
    return result;
}


void Mirror::DrawDebugGUI()
{
    if (ImGui::Begin("mirror", nullptr, ImGuiWindowFlags_None))
    {
      
        if (ImGui::CollapsingHeader("mirror", ImGuiTreeNodeFlags_DefaultOpen))
        {
           
            ImGui::InputFloat3("pos", &position.x);

        }
    }
    ImGui::End();
}
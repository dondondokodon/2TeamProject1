#include "Stairs.h"

Stairs::Stairs()
{
    SetModel("Data/Model/Objects/kaidan/kaidan.mdl");

    position = { 0.0f, 1.2f, 0.0f };
    angle = { 0.0f, 0.0f, 0.0f };
    scale = { 1.0f, 1.0f, 1.0f };

    

    
    
}

Stairs::~Stairs()
{

}

void Stairs::Update(float elapsedTime)
{
    float halfX = 2.0f * scale.x;
    float halfZ = 1.0f * scale.z;
    float height = 1.2f * scale.y; // 階段の高さ
    float offsetZ = 0.5f;

    aabbMin = { position.x - halfX, position.y, position.z - halfZ + offsetZ };
    aabbMax = { position.x + halfX, position.y + height, position.z + halfZ + offsetZ };
}



void Stairs::Render(const RenderContext& rc, ModelRenderer* renderer)
{
   StageObject::Render(rc, renderer);
}

void Stairs::CollisionVsPlayer(Player& p)
{
   
}

void Stairs::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    // AABB の中心
    DirectX::XMFLOAT3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    // AABB のサイズ
    DirectX::XMFLOAT3 size = {
        (aabbMax.x - aabbMin.x),
        (aabbMax.y - aabbMin.y),
        (aabbMax.z - aabbMin.z)
    };

    DirectX::XMFLOAT3 angle = { 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 1, 0, 1 }; // 緑色

    renderer->RenderBox(rc, center, angle, size, color);
}


void Stairs::DrawDebugGUI()
{
  

}

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

}


void Stairs::DrawDebugGUI()
{
  
    ImGui::DragFloat3("angle", &angle.x, 0.1f);
}

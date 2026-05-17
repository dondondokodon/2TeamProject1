#include "GoalObject.h"
#include "Flag.h"
#include "Collision.h"
#include<imgui.h>

GoalObject::GoalObject()
{
	SetModel("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
	goalEffect = std::make_unique<Effect>("Data/Effect/hansya.efkefc");
    UpdateCollider();
}

GoalObject::~GoalObject()
{

}

void GoalObject::Update(float elapsedTime)
{
    UpdateCollider();

	//照射装置に当たっているとき
	if (Flag::Instance().getFlag(Flag::eventName::openGoal))
	{
        Flag::Instance().SetFlag(Flag::IsGoal, isHit);
	}

	if (!isHit)
	{
		wasHit = false;
	}

    isHit = false;
	UpdateTransform();
}

//コライダー更新処理
void GoalObject::UpdateCollider()
{
    aabbMin.x = position.x - halfSize.x;
    aabbMin.y = position.y;
    aabbMin.z = position.z - halfSize.z;

    aabbMax.x = position.x + halfSize.x;
    aabbMax.y = position.y + halfSize.y;
    aabbMax.z = position.z + halfSize.z;
}

void GoalObject::CollisionVsPlayer(Player& p)
{
    DirectX::XMFLOAT3 push; //使わない

    if (Collision::IntersectCylinderVsAABB(
        p.GetPosition(),
        p.GetRadius(),
        p.GetHeight(),
        aabbMin,
        aabbMax,
        push))
    {
        isHit = true;

		// ゴール専用素材がまだ無いので、反射エフェクトを仮でゴール用に使う
		if (!wasHit && Flag::Instance().getFlag(Flag::eventName::openGoal) && goalEffect)
		{
			PlayGoalEffect();
		}

		wasHit = true;
    }
}

void GoalObject::PlayGoalEffect()
{
	if (!goalEffect) return;

	DirectX::XMFLOAT3 effectPos = position;

	// エフェクトの高さ
	// 値を大きくすると上に出る
	const float effectHeight = 0.5f;

	// エフェクトの大きさ
	// 値を大きくすると大きく表示される
	const float effectScale = 0.5f;

	effectPos.y += effectHeight;
	goalEffect->Play(effectPos, effectScale);
}

void GoalObject::DrawDebugGUI()
{
    if (ImGui::Begin("Goal", nullptr, ImGuiWindowFlags_None))
    {

        if (ImGui::CollapsingHeader("Goal", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat3("pos", &position.x);
            //位置
            ImGui::CheckboxFlags("isHit", (unsigned int*)&isHit, true);

			// デバッグ確認用
			// ゴール条件を満たさなくても、このボタンだけでエフェクトを確認できる
			if (ImGui::Button("Play Goal Effect"))
			{
				PlayGoalEffect();
			}
        }
        if (ImGui::CollapsingHeader("AABB", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat3("posMin", &aabbMin.x);
            ImGui::InputFloat3("posMax", &aabbMax.x);
        }
    }
    ImGui::End();
}

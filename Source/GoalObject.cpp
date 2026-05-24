#include "GoalObject.h"
#include "Flag.h"
#include "Collision.h"
#include<imgui.h>

GoalObject::GoalObject()
{

	goalEffect = std::make_unique<Effect>("Data/Effect/goru.efkefc");
    UpdateCollider();
}

GoalObject::~GoalObject()
{
	StopGoalEffect();
}

void GoalObject::Update(float elapsedTime)
{
    UpdateCollider();

	//照射装置に当たっているとき、またはデバッグでゴール判定を有効にしているとき
	bool goalJudgeEnabled = Flag::Instance().getFlag(Flag::eventName::openGoal) || debugGoalJudgeEnabled;
	if (goalJudgeEnabled)
	{
        Flag::Instance().SetFlag(Flag::IsGoal, isHit);
	}

	// ゴールエフェクトは、照射装置にレーザーが当たってゴール判定が有効な間だけ出す
	// 実際にゴールしたかどうかは、上の IsGoal に isHit を入れる処理で判定する
	if (goalJudgeEnabled)
	{
		UpdateGoalHitEffect(elapsedTime);
	}
	else
	{
		StopGoalEffect();
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
		wasHit = true;
    }
}

void GoalObject::PlayGoalEffect()
{
	if (!goalEffect) return;

	DirectX::XMFLOAT3 effectPos = position;

	effectPos.y += goalEffectHeight;
	goalEffectHandle = goalEffect->Play(effectPos, goalEffectScale);
	goalEffectPlaying = true;
}
//ゴールエフェクトの停止
void GoalObject::StopGoalEffect()
{
	if (!goalEffect || !goalEffectPlaying) return;

	goalEffect->Stop(goalEffectHandle);
	goalEffectPlaying = false;
	goalEffectHandle = -1;
	goalHitEffectTimer = 0.0f;
}
//ゴールエフェクトの更新
void GoalObject::UpdateGoalHitEffect(float elapsedTime)
{
	if (!goalEffect) return;

	// goru.efkefcは1回完結の素材なので、ゴール判定が有効な間だけ一定間隔で再再生する
	// 完全なループ素材ではないため、開始と終了のつなぎ目が見える可能性はある
	goalHitEffectTimer -= elapsedTime;
	if (goalEffectPlaying && goalHitEffectTimer > 0.0f) return;

	StopGoalEffect();
	PlayGoalEffect();
	goalHitEffectTimer = goalHitEffectInterval;
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
			ImGui::Checkbox("Debug Goal Judge Enabled", &debugGoalJudgeEnabled);
			if (ImGui::Button("Force Goal"))
			{
				isHit = true;
				wasHit = true;
				Flag::Instance().SetFlag(Flag::eventName::IsGoal, true);
				PlayGoalEffect();
			}
			ImGui::InputFloat("Effect Height", &goalEffectHeight);
			ImGui::InputFloat("Effect Scale", &goalEffectScale);
			ImGui::InputFloat("Hit Effect Interval", &goalHitEffectInterval);

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

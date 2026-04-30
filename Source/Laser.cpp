#include "Laser.h"
#include"Collision.h"
#include <algorithm>
#include<imgui.h>
#include"StageObjectManager.h"

void LaserBeam::Update()
{
    segments.clear();

    DirectX::XMFLOAT3 start = origin;
    DirectX::XMFLOAT3 dir = direction;

    for (int i = 0; i < maxReflection; i++)
    {
        DirectX::XMFLOAT3 end =
        {
            start.x + dir.x * maxLength,
            start.y + dir.y * maxLength,
            start.z + dir.z * maxLength
        };

        DirectX::XMFLOAT3 hitPos, hitNormal;

        // ★ StageObjectManager にレイキャストを依頼する
        bool hit = StageObjectManager::Instance().RayCast(start, end, hitPos, hitNormal);

        if (hit)
        {
            segments.push_back({ start, hitPos });

            // 反射
            DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&dir);
            DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&hitNormal);
            DirectX::XMVECTOR r = DirectX::XMVector3Reflect(d, n);
            DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(r));

            start = hitPos;
        }
        else
        {
            segments.push_back({ start, end });
            break;
        }
    }
}

//デバッグ用GUI描画
void LaserBeam::DrawDebugGUI()
{
    if (ImGui::Begin("Beam", nullptr, ImGuiWindowFlags_None))
    {
        //トランスフォーム
        if (ImGui::CollapsingHeader("Item", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //位置
            ImGui::InputFloat3("dir", &direction.x);

            //長さ
			ImGui::InputFloat("maxLength", &maxLength);

            //太さ
			ImGui::InputFloat("radius", &radius);
        }
    }
    ImGui::End();
}

LaserHit LaserBeam::CheckHitAABB(const BoxCollider& box) const
{
    LaserHit result;

    // AABB の min/max
    DirectX::XMFLOAT3 bmin =
    {
        box.GetCenter().x - box.GetSize().x * 0.5f,
        box.GetCenter().y - box.GetSize().y * 0.5f,
        box.GetCenter().z - box.GetSize().z * 0.5f
    };
    DirectX::XMFLOAT3 bmax =
    {
        box.GetCenter().x + box.GetSize().x * 0.5f,
        box.GetCenter().y + box.GetSize().y * 0.5f,
        box.GetCenter().z + box.GetSize().z * 0.5f
    };

    // 全ての線分に対して判定
    for (const auto& seg : segments)
    {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
        DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
        DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e,s));

        float segLen = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));

        // AABB 中心
        DirectX::XMVECTOR boxCenter = DirectX::XMLoadFloat3(&box.GetCenter());
        DirectX::XMVECTOR v = DirectX::XMVectorSubtract(boxCenter, s);

        // 線分上の最近接点
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, dir));
        t = std::clamp(t, 0.0f, segLen);

        DirectX::XMVECTOR closestOnRay = DirectX::XMVectorAdd(s, DirectX::XMVectorScale(dir, t));

        // AABB 上の最近接点
        DirectX::XMFLOAT3 rayPoint;
        DirectX::XMStoreFloat3(&rayPoint, closestOnRay);

        DirectX::XMFLOAT3 closestOnAABB =
        {
            std::clamp(rayPoint.x, bmin.x, bmax.x),
            std::clamp(rayPoint.y, bmin.y, bmax.y),
            std::clamp(rayPoint.z, bmin.z, bmax.z)
        };

        DirectX::XMVECTOR aabbP = DirectX::XMLoadFloat3(&closestOnAABB);

        // 距離
        float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(aabbP , closestOnRay)));

        if (dist <= radius)
        {
            result.hit = true;
            result.penetration = radius - dist;

            // normal
            DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(aabbP , closestOnRay));
            DirectX::XMStoreFloat3(&result.normal, n);

         
            result.point = closestOnAABB;
            return result;
        }
    }

    return result;
}

//レーザー本体
void Laser::Initialize(
    const DirectX::XMFLOAT3& emitterPos,
    const DirectX::XMFLOAT3& dir,
    float maxLen)
{
    model = std::make_unique<Model>("Data/Model/Objects/Box/Box.mdl");

    startPos = emitterPos;
    direction = dir;
    maxLength = maxLen;

    // 正規化
    DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
    v = DirectX::XMVector3Normalize(v);
    DirectX::XMStoreFloat3(&direction, v);

    // LaserBeam に初期値を渡す
    beam.origin = startPos;
    beam.direction = direction;
    beam.maxLength = maxLength;
    beam.maxReflection = 5;
    //beam.radius = 0.3f;
}

void Laser::Update(float elapsedTime)
{
    if (!isActive) return;

    // LaserBeam がレーザーを撃つ（反射含む）
    beam.Update();

	position = startPos;
	direction = beam.direction;
	position.x -= direction.x*0.5f;

	UpdateTransform();
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    //beam.Render(rc, renderer);

    StageObject::Render(rc, renderer);
}
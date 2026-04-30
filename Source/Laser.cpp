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

            //// 反射
            //DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&dir);
            //DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&hitNormal);
            //DirectX::XMVECTOR r = DirectX::XMVector3Reflect(d, n);
            //DirectX::XMStoreFloat3(&dir, DirectX::XMVector3Normalize(r));

            //start = hitPos;
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

            //// ★ AABBの面ベースで法線を求める
            //DirectX::XMFLOAT3 normal = { 0,0,0 };

            //// AABB中心と接触点との差
            //float dx = rayPoint.x - box.GetCenter().x;
            //float dy = rayPoint.y - box.GetCenter().y;
            //float dz = rayPoint.z - box.GetCenter().z;

            //// 各軸の「残り距離」
            //float px = (box.GetSize().x * 0.5f) - fabsf(dx);
            //float py = (box.GetSize().y * 0.5f) - fabsf(dy);
            //float pz = (box.GetSize().z * 0.5f) - fabsf(dz);

            //// 一番めり込みが小さい軸＝当たった面
            //if (px < py && px < pz)
            //{
            //    normal = { (dx > 0) ? 1.0f : -1.0f, 0, 0 };
            //}
            //else if (py < pz)
            //{
            //    normal = { 0, (dy > 0) ? 1.0f : -1.0f, 0 };
            //}
            //else
            //{
            //    normal = { 0, 0, (dz > 0) ? 1.0f : -1.0f };
            //}

            //result.normal = normal;

            result.point = closestOnAABB;
            return result;
        }
    }

    return result;
}

//レーザービーム
//void LaserBeam::UpdateTransform()
//{
//    //// ① 方向ベクトル
//    //DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&startPos);
//    //DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&endPos);
//
//    //DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, S);
//
//    //// ② 長さ
//    //float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(V));
//
//    //// ③ 正規化（前方向）
//    //DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(V);
//
//    //// ④ 上方向（とりあえずワールドUP）
//    //DirectX::XMVECTOR up = DirectX::g_XMIdentityR1; // (0,1,0)
//
//    //// ⑤ 右方向
//    //DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, dir);
//
//    //// 上方向再計算（直交化）
//    //up = DirectX::XMVector3Cross(dir, right);
//
//    //right = DirectX::XMVector3Normalize(right);
//    //up = DirectX::XMVector3Normalize(up);
//
//    //// ⑥ 中心位置
//    //DirectX::XMVECTOR center = DirectX::XMVectorLerp(S, E, 0.5f);
//
//    //// ⑦ 行列作成
//    //DirectX::XMMATRIX mat;
//
//    //mat.r[0] = right;
//    //mat.r[1] = up;
//    //mat.r[2] = dir;
//    //mat.r[3] = DirectX::XMVectorSet(
//    //    DirectX::XMVectorGetX(center),
//    //    DirectX::XMVectorGetY(center),
//    //    DirectX::XMVectorGetZ(center),
//    //    1.0f
//    //);
//
//    //// ⑧ スケール（長さと太さ）
//    //DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, width, length);
//
//    //mat = scale * mat;
//
//    //// ⑨ 保存
//    //DirectX::XMStoreFloat4x4(&transform, mat);
//
//    // 1. 方向と長さの計算
//    DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&startPos);
//    DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&endPos);
//    DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, S);
//    float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(V));
//
//    if (length < 0.0001f) return; // 長さがほぼゼロなら処理しない
//
//    // 2. 正規化した方向（前方向 Z）
//    DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(V);
//
//    // 3. 右・上方向の算出（ビルボード的な計算）
//    DirectX::XMVECTOR worldUp = DirectX::g_XMIdentityR1; // (0,1,0)
//    // 方向が真上を向いている場合の回避
//    if (fabsf(DirectX::XMVectorGetY(dir)) > 0.999f) worldUp = DirectX::g_XMIdentityR2;
//
//    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, dir));
//    DirectX::XMVECTOR up = DirectX::XMVector3Cross(dir, right);
//
//    // 4. スケール行列（Z方向にのみ長さを伸ばす）
//    // ※もしBoxモデルが「中心から前後0.5ずつ」なら、Zスケールをlengthにする
//    DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(width, width, length);
//
//    // 5. 回転・並進（移動）行列
//    DirectX::XMMATRIX matRotTrans;
//    matRotTrans.r[0] = right;
//    matRotTrans.r[1] = up;
//    matRotTrans.r[2] = dir;
//    // ここがポイント：配置場所を「中心(center)」ではなく「始点(startPos)」にする
//    // ※ただしモデルが「中心原点」なら、centerにする必要があります。
//    DirectX::XMVECTOR center = DirectX::XMVectorLerp(S, E, 0.5f);
//    matRotTrans.r[3] = DirectX::XMVectorSetW(center, 1.0f);
//
//    // 6. 合成
//    DirectX::XMStoreFloat4x4(&transform, matScale * matRotTrans);
//
//
//    // center が XMVECTOR の場合
//    DirectX::XMMATRIX debugMat = DirectX::XMMatrixIdentity();
//    // w成分を1.0にするために XMVectorSetW を通すのが安全です
//    debugMat.r[3] = DirectX::XMVectorSetW(center, 1.0f);
//    DirectX::XMStoreFloat4x4(&transform, debugMat);
//
//}
//
////仮
//void LaserBeam::Update(float elapsedTime)
//{
//	// ビームのエフェクト更新（例: アニメーションUV）
//	UpdateTransform();
//    UpdateColliders();
//}
//
////仮
//void LaserBeam::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//	// ビームの描画
//	//renderer->DrawModel(rc, model, transform);
//}
//
//void LaserBeam::UpdateColliders()
//{
//    //// 足場は start → end の中点
//    //DirectX::XMFLOAT3 center =
//    //{
//    //    (startPos.x + endPos.x) * 0.5f,
//    //    (startPos.y + endPos.y) * 0.5f,
//    //    (startPos.z + endPos.z) * 0.5f
//    //};
//
//    //float length =
//    //    sqrtf(
//    //        (endPos.x - startPos.x) * (endPos.x - startPos.x) +
//    //        (endPos.y - startPos.y) * (endPos.y - startPos.y) +
//    //        (endPos.z - startPos.z) * (endPos.z - startPos.z)
//    //    );
//
//    //// Z 軸をレーザー方向に向ける回転行列
//    //DirectX::XMMATRIX rotMat =
//    //    DirectX::XMMatrixLookToRH(
//    //        DirectX::XMVectorZero(),
//    //        direction,
//    //        DirectX::XMVectorSet(0, 1, 0, 0)
//    //    );
//
//    //// 足場コライダー（薄い箱）
//    //topCollider.SetCenter({ center.x,center.y + 0.4f,center.z });
//    //topCollider.SetSize({ 1.0f, 0.1f, length });
//    //
//
//    //// 側面も同様に薄く作る
//    //sideCollider.SetCenter({ center.x,center.y - 0.8f,center.z });
//    //sideCollider.SetSize({ 1.2f, 0.4f, length });
//    {
//        // 中点
//        DirectX::XMFLOAT3 center =
//        {
//            (startPos.x + endPos.x) * 0.5f,
//            (startPos.y + endPos.y) * 0.5f,
//            (startPos.z + endPos.z) * 0.5f
//        };
//
//        // 方向ベクトル
//        DirectX::XMVECTOR dir = DirectX::XMVectorSet(
//            endPos.x - startPos.x,
//            endPos.y - startPos.y,
//            endPos.z - startPos.z,
//            0.0f
//        );
//
//        // 正規化
//        dir = DirectX::XMVector3Normalize(dir);
//
//        // 長さ
//        float length = DirectX::XMVectorGetX(
//            DirectX::XMVector3Length(
//                DirectX::XMVectorSubtract(
//                    DirectX::XMLoadFloat3(&endPos),
//                    DirectX::XMLoadFloat3(&startPos)
//                )
//            )
//        );
//
//        // Z 軸をレーザー方向に向ける回転行列
//        DirectX::XMMATRIX rotMat =
//            DirectX::XMMatrixLookToRH(
//                DirectX::XMVectorZero(),
//                dir,
//                DirectX::XMVectorSet(0, 1, 0, 0)
//            );
//
//        // 足場コライダー
//        topCollider.SetCenter({ center.x, center.y + 0.4f, center.z });
//        topCollider.SetSize({ 1.0f, 0.1f, length });
//        topCollider.SetRotationMatrix(rotMat);  // ← これだけでOK
//
//        // 側面コライダー
//        sideCollider.SetCenter({ center.x, center.y - 0.8f, center.z });
//        sideCollider.SetSize({ 1.2f, 0.4f, length });
//        sideCollider.SetRotationMatrix(rotMat);
//}

//レーザー本体
void Laser::Initialize(
    const DirectX::XMFLOAT3& emitterPos,
    const DirectX::XMFLOAT3& dir,
    float maxLen)
{
    model = std::make_unique<Model>("Data/Model/Objects/Box/Box.mdl");
    //startPos = emitterPos;
    //direction = dir;

    //position = startPos; 

    //// 正規化
    //DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
    //v = DirectX::XMVector3Normalize(v);
    //DirectX::XMStoreFloat3(&direction, v);

    //maxLength = maxLen;

    //Shoot();

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
    beam.radius = 0.3f;
}

void Laser::Update(float elapsedTime)
{
    if (!isActive) return;

  /*  Shoot();

    beam.SetPoints(startPos, endPos);
    beam.SetAngle(angle);
    beam.setDirection(direction);
    beam.Update(elapsedTime);*/

    // LaserBeam がレーザーを撃つ（反射含む）
    beam.Update();

	UpdateTransform();
    //ResolvePlayerCollision();
}

//void Laser::Shoot()
//{
//    DirectX::XMFLOAT3 pos = startPos;
//    DirectX::XMFLOAT3 dir = direction;
//
//    endPos = startPos;
//
//    float remaining = maxLength;
//
//    for (int i = 0; i < 5; i++) // 反射回数制限
//    {
//        DirectX::XMFLOAT3 hitPos;
//        DirectX::XMFLOAT3 normal;
//
//        bool hit = manager->RayCast(
//            pos,
//            DirectX::XMFLOAT3(
//                pos.x + dir.x * remaining,
//                pos.y + dir.y * remaining,
//                pos.z + dir.z * remaining
//            ),
//            hitPos,
//            normal
//        );
//
//        if (!hit)
//        {
//            endPos = {
//                pos.x + dir.x * remaining,
//                pos.y + dir.y * remaining,
//                pos.z + dir.z * remaining
//            };
//            break;
//        }
//
//        endPos = hitPos;
//
//        auto Distance = [](const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
//            {
//                float dx = a.x - b.x;
//                float dy = a.y - b.y;
//                float dz = a.z - b.z;
//                return sqrtf(dx * dx + dy * dy + dz * dz);
//            };
//
//        remaining -= Distance(pos, hitPos);
//
//        pos = hitPos;
//
//        // 反射
//        dir = Reflect(dir, normal);
//    }
//}
//
//DirectX::XMFLOAT3 Laser::Reflect(
//    const DirectX::XMFLOAT3& inDir,
//    const DirectX::XMFLOAT3& normal)
//{
//    DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&inDir);
//    DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normal);
//
//    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, n));
//
//    DirectX::XMVECTOR result =
//        DirectX::XMVectorSubtract(
//            d,
//            DirectX::XMVectorScale(n, 2.0f * dot)
//        );
//
//    DirectX::XMFLOAT3 out;
//    DirectX::XMStoreFloat3(&out, DirectX::XMVector3Normalize(result));
//
//    return out;
//}
//
//
//void Laser::ResolvePlayerCollision()
//{
//    
//}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    //beam.Render(rc, renderer);

    //StageObject::Render(rc, renderer);
}
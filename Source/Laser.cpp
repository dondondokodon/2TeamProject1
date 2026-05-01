#include "Laser.h"
#include"Collision.h"
#include <algorithm>
#include<imgui.h>
#include"StageObjectManager.h"
#include"math.h"

using namespace DirectX;

void LaserBeam::Update(float elapsedTime) 
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

        // ?? StageObjectManager ????C?L???X?g????????
        bool hit = StageObjectManager::Instance().RayCast(start, end, hitPos, hitNormal);

        if (hit)
        {
            segments.push_back({ start, hitPos });

            // ????
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

//?f?o?b?O?pGUI?`??
void LaserBeam::DrawDebugGUI()
{
    if (ImGui::Begin("Beam", nullptr, ImGuiWindowFlags_None))
    {
        //?g?????X?t?H?[??
        if (ImGui::CollapsingHeader("Item", ImGuiTreeNodeFlags_DefaultOpen))
        {
            //??u
            ImGui::InputFloat3("dir", &direction.x);

            //????
			ImGui::InputFloat("maxLength", &maxLength);

            //????
			ImGui::InputFloat("radius", &radius);
        }
    }
    ImGui::End();
}

LaserHit LaserBeam::CheckHitAABB(const BoxCollider& box) const
{
    if (isRotating)
        return LaserHit(); // ????????

    LaserHit result;
    //float bestDist = FLT_MAX;


    // AABB ?? min/max
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

    // ?S???????????????
    for (const auto& seg : segments)
    {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
        DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
        DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e,s));

        float segLen = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));

        // AABB ???S
        DirectX::XMVECTOR boxCenter = DirectX::XMLoadFloat3(&box.GetCenter());
        DirectX::XMVECTOR v = DirectX::XMVectorSubtract(boxCenter, s);

        // ??????????_
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v, dir));
        t = std::clamp(t, 0.0f, segLen);

        DirectX::XMVECTOR closestOnRay = DirectX::XMVectorAdd(s, DirectX::XMVectorScale(dir, t));

        // AABB ??????_
        DirectX::XMFLOAT3 rayPoint;
        DirectX::XMStoreFloat3(&rayPoint, closestOnRay);

        DirectX::XMFLOAT3 closestOnAABB =
        {
            std::clamp(rayPoint.x, bmin.x, bmax.x),
            std::clamp(rayPoint.y, bmin.y, bmax.y),
            std::clamp(rayPoint.z, bmin.z, bmax.z)
        };

        DirectX::XMVECTOR aabbP = DirectX::XMLoadFloat3(&closestOnAABB);

        // ????
        float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(aabbP , closestOnRay)));


        float skin = 0.01f;

        if (dist <= radius - skin)
        {
            float hitDist = t;  //・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾌ具ｿｽ・ｽ・ｽ
            result.hit = true;

            float depth = radius - dist;

            // 押しすぎ防止
            float push = depth * 0.7f;

            // 最小保証
            push = max(push, 0.01f);

            result.penetration = push;

            // 法線
            DirectX::XMFLOAT3 dirOut =
            {
                box.GetCenter().x - rayPoint.x,
                box.GetCenter().y - rayPoint.y,
                box.GetCenter().z - rayPoint.z
            };

            DirectX::XMVECTOR n = DirectX::XMVector3Normalize(
                DirectX::XMLoadFloat3(&dirOut)
            );

            DirectX::XMStoreFloat3(&result.normal, n);

            return result;
        }
      
    }

    return result;
    //return bestHit;
}

//・ｽ~・ｽ・ｽ・ｽﾆの費ｿｽ・ｽ・ｽ
LaserHit LaserBeam::CheckHitCylinder(const CylinderCollider& cylinder) const
{
    if (isRotating)
        return LaserHit();

    LaserHit result;

    float halfH = cylinder.GetHeight() * 0.5f;
    float cylR = cylinder.GetRadius();
    DirectX::XMFLOAT3 center = cylinder.GetCenter();

    for (const auto& seg : segments)
    {
        DirectX::XMVECTOR s = XMLoadFloat3(&seg.start);
        DirectX::XMVECTOR e = XMLoadFloat3(&seg.end);

        DirectX::XMVECTOR dir = XMVector3Normalize(XMVectorSubtract(e, s));
        float segLen = XMVectorGetX(XMVector3Length(XMVectorSubtract(e, s)));

        // レーザー上の最近接点 p
        DirectX::XMVECTOR c = XMLoadFloat3(&center);
        float t = XMVectorGetX(XMVector3Dot(XMVectorSubtract(c, s), dir));
        t = std::clamp(t, 0.0f, segLen);

        DirectX::XMVECTOR pVec = XMVectorAdd(s, XMVectorScale(dir, t));

        DirectX::XMFLOAT3 p;
        XMStoreFloat3(&p, pVec);

        // Cylinder 上の最近接点 q
        DirectX::XMFLOAT3 q;

        // Y clamp
        q.y = std::clamp(p.y, center.y - halfH, center.y + halfH);

        // XZ 円
        float dx = p.x - center.x;
        float dz = p.z - center.z;
        float len = sqrtf(dx * dx + dz * dz);

        if (len > cylR)
        {
            q.x = center.x + dx / len * cylR;
            q.z = center.z + dz / len * cylR;
        }
        else
        {
            q.x = p.x;
            q.z = p.z;
        }

        DirectX::XMVECTOR qVec = XMLoadFloat3(&q);

        // 距離
        DirectX::XMVECTOR v = XMVectorSubtract(qVec, pVec);
        float dist = XMVectorGetX(XMVector3Length(v));

        float skin = 0.01f;

        if (dist <= radius - skin)
        {
            result.hit = true;

            float depth = radius - dist;

            // AABB と同じ押し量
            float push = depth * 0.7f;
            push = max(push, 0.01f);

            result.penetration = push;

            // ? 法線（AABB と同じ向きに統一）
            DirectX::XMVECTOR n;

            if (dist > 0.0001f)
            {
                // AABB と同じ向き：center - p
                DirectX::XMFLOAT3 dirOut =
                {
                    center.x - p.x,
                    center.y - p.y,
                    center.z - p.z
                };
                n = XMVector3Normalize(XMLoadFloat3(&dirOut));
            }
            else
            {
                // fallback：XZ 方向優先
                DirectX::XMFLOAT3 fallback =
                {
                    center.x - p.x,
                    0.0f,
                    center.z - p.z
                };

                DirectX::XMVECTOR fb = XMLoadFloat3(&fallback);

                if (XMVector3Length(fb).m128_f32[0] < 0.0001f)
                    fb = XMVectorSet(1, 0, 0, 0);

                n = XMVector3Normalize(fb);
            }

            XMStoreFloat3(&result.normal, n);
            result.point = q;

            return result;
        }
    }

    return result;
}



void Laser::RotateAroundCenter(const DirectX::XMFLOAT3& center, float angleY)
{
    // origin を中心基準に移動
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&startPos);
    DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&center);
    DirectX::XMVECTOR local = DirectX::XMVectorSubtract(pos, c);

    // Y軸回転
    DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(angleY);
    local = DirectX::XMVector3Transform(local, rot);

    // 中心に戻す
    DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(local, c);
    DirectX::XMStoreFloat3(&startPos, newPos);

    // direction も回転
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
    dir = DirectX::XMVector3TransformNormal(dir, rot);
    DirectX::XMStoreFloat3(&direction, dir);
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


void Laser::Initialize(
    const DirectX::XMFLOAT3& emitterPos,
    const DirectX::XMFLOAT3& dir,
    float maxLen)
{
    model = std::make_unique<Model>("Data/Model/Objects/Laser/Laser.mdl");

    startPos = emitterPos;
    direction = dir;
    maxLength = maxLen;
  
    scale = { 0.5f,0.5f,0.5f };

    
    DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
    v = DirectX::XMVector3Normalize(v);
    DirectX::XMStoreFloat3(&direction, v);

    // LaserBeam
    beam.origin = startPos;
    beam.direction = direction;
    beam.maxLength = maxLength;
    beam.maxReflection = 5;
    //beam.radius = 0.3f;


}

void Laser::Update(float elapsedTime)
{
    beam.isRotating = isRotating;


    if (!isActive) return;

  /*  Shoot();

    beam.SetPoints(startPos, endPos);
    beam.SetAngle(angle);
    beam.setDirection(direction);
    beam.Update(elapsedTime);*/

	position = startPos;
	direction = beam.direction;
	position.x -= direction.x*0.5f;
	angle.y = atan2f(direction.x, direction.z);
    
    

	UpdateTransform();
    //ResolvePlayerCollision();

    if (!isActive) return;

    // ステージ中心（必要なら変更）
    DirectX::XMFLOAT3 center = { 0, 0, 0 };

    // Q/E で回転
    float step = DirectX::XM_PI / 4.0f;

    if (!isRotating)
    {

        if (GetAsyncKeyState('Q') & 0x0001)
        {
            targetAngleY -= step;
            isRotating = true;   // ← 回転開始
        }
        if (GetAsyncKeyState('E') & 0x0001)
        {
            targetAngleY += step;
            isRotating = true;   // ← 回転開始
        }
    }

    float diff = targetAngleY - currentAngleY;

    if (fabs(diff) < 0.001f)
    {
        diff = 0.0f;
        currentAngleY = targetAngleY;
        isRotating = false;
    }
    else
    {
        float dir = (diff > 0.0f) ? 1.0f : -1.0f;

        // 等速回転（例：1秒で90°）
        float delta = (DirectX::XM_PI / 2.0f) * elapsedTime * dir;

        if (fabs(delta) > fabs(diff))
            delta = diff;

        currentAngleY += delta;

        RotateAroundCenter(center, delta);
    }


    // レーザーの反射計算
    beam.origin = startPos;
    beam.direction = direction;
    // LaserBeam がレーザーを撃つ（反射含む）
    beam.Update(elapsedTime);
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    //beam.Render(rc, renderer);

    StageObject::Render(rc, renderer);
}
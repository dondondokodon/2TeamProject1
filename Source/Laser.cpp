#include "Laser.h"
#include"Collision.h"
#include <algorithm>
#include<imgui.h>
#include"StageObjectManager.h"
#include"math.h"

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
            float hitDist = t;  //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ì‹ï¿½ï¿½ï¿½
            result.hit = true;

            float depth = radius - dist;

            // ‰Ÿ‚µ‚·‚¬–h~
            float push = depth * 0.7f;

            // Å¬•ÛØ
            push = max(push, 0.01f);

            result.penetration = push;

            // –@ü
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

//ï¿½~ï¿½ï¿½ï¿½Æ‚Ì”ï¿½ï¿½ï¿½
LaserHit LaserBeam::CheckHitCylinder(const CylinderCollider& cylinder)const
{

    LaserHit result;

    float cylinderHalfHeight = cylinder.GetHeight() * 0.5f;
    float cylinderRadius = cylinder.GetRadius();
    DirectX::XMFLOAT3 center = cylinder.GetCenter();

    for (const auto& seg : segments) {
        DirectX::XMVECTOR s = DirectX::XMLoadFloat3(&seg.start);
        DirectX::XMVECTOR e = DirectX::XMLoadFloat3(&seg.end);
        DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(e, s));
        float segLen = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(e, s)));

        // 1. ï¿½ï¿½ï¿½[ï¿½Uï¿½[ï¿½iï¿½ï¿½ï¿½ï¿½ï¿½jï¿½ï¿½ÌÅ‹ßÚ“_ p ï¿½ï¿½ï¿½ï¿½ï¿½ß‚ï¿½
        DirectX::XMVECTOR cylinderCenter = DirectX::XMLoadFloat3(&center);
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorSubtract(cylinderCenter, s), dir));
        t = std::clamp(t, 0.0f, segLen);
        DirectX::XMVECTOR pVec = DirectX::XMVectorAdd(s, DirectX::XMVectorScale(dir, t));
        DirectX::XMFLOAT3 p;
        DirectX::XMStoreFloat3(&p, pVec);

        // 2. ï¿½~ï¿½ï¿½ï¿½ï¿½ï¿½ÌÅ‹ßÚ“_ q ï¿½ï¿½ï¿½ï¿½ï¿½ß‚ï¿½
        DirectX::XMFLOAT3 q;
        // ï¿½ï¿½ï¿½ï¿½(Y)ï¿½Í’Pï¿½ï¿½ï¿½ÉƒNï¿½ï¿½ï¿½ï¿½ï¿½v
        q.y = std::clamp(p.y, center.y - cylinderHalfHeight, center.y + cylinderHalfHeight);

        // ï¿½ï¿½ï¿½ï¿½(XZ)ï¿½Í‰~ï¿½Ì”ÍˆÍ“ï¿½ï¿½ÉƒNï¿½ï¿½ï¿½ï¿½ï¿½v
        float dx = p.x - center.x;
        float dz = p.z - center.z;
        float dXZ = sqrtf(dx * dx + dz * dz);
        if (dXZ > cylinderRadius) {
            q.x = center.x + (dx / dXZ) * cylinderRadius;
            q.z = center.z + (dz / dXZ) * cylinderRadius;
        }
        else {
            q.x = p.x;
            q.z = p.z;
        }

        // 3. ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
        DirectX::XMVECTOR qVec = DirectX::XMLoadFloat3(&q);
        DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(qVec, pVec);
        float dist = DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));

        if (dist <= this->radius) {
            result.hit = true;
            result.penetration = this->radius - dist;

            // ï¿½@ï¿½ï¿½
            if (dist > 0.0001f) {
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(diff));
            }
            else {
                // ï¿½^ï¿½ñ’†‚ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½^ï¿½ï¿½É‚ï¿½ï¿½Ä‚ï¿½ï¿½ï¿½
                result.normal = { 0, 1, 0 };
            }

            result.point = q;
            return result;
        }
    }
    return result;
}

void Laser::RotateAroundCenter(const DirectX::XMFLOAT3& center, float angleY)
{
    // origin ‚ğ’†SŠî€‚ÉˆÚ“®
    DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&startPos);
    DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&center);
    DirectX::XMVECTOR local = DirectX::XMVectorSubtract(pos, c);

    // Y²‰ñ“]
    DirectX::XMMATRIX rot = DirectX::XMMatrixRotationY(angleY);
    local = DirectX::XMVector3Transform(local, rot);

    // ’†S‚É–ß‚·
    DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(local, c);
    DirectX::XMStoreFloat3(&startPos, newPos);

    // direction ‚à‰ñ“]
    DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
    dir = DirectX::XMVector3TransformNormal(dir, rot);
    DirectX::XMStoreFloat3(&direction, dir);
}


//ƒŒ[ƒU[ƒr[ƒ€
//void LaserBeam::UpdateTransform()
//{
//    //// ‡@ •ûŒüƒxƒNƒgƒ‹
//    //DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&startPos);
//    //DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&endPos);
//
//    //DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, S);
//
//    //// ‡A ’·‚³
//    //float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(V));
//
//    //// ‡B ³‹K‰»i‘O•ûŒüj
//    //DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(V);
//
//    //// ‡C ã•ûŒüi‚Æ‚è‚ ‚¦‚¸ƒ[ƒ‹ƒhUPj
//    //DirectX::XMVECTOR up = DirectX::g_XMIdentityR1; // (0,1,0)
//
//    //// ‡D ‰E•ûŒü
//    //DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, dir);
//
//    //// ã•ûŒüÄŒvZi’¼Œğ‰»j
//    //up = DirectX::XMVector3Cross(dir, right);
//
//    //right = DirectX::XMVector3Normalize(right);
//    //up = DirectX::XMVector3Normalize(up);
//
//    //// ‡E ’†SˆÊ’u
//    //DirectX::XMVECTOR center = DirectX::XMVectorLerp(S, E, 0.5f);
//
//    //// ‡F s—ñì¬
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
//    //// ‡G ƒXƒP[ƒ‹i’·‚³‚Æ‘¾‚³j
//    //DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, width, length);
//
//    //mat = scale * mat;
//
//    //// ‡H •Û‘¶
//    //DirectX::XMStoreFloat4x4(&transform, mat);
//
//    // 1. •ûŒü‚Æ’·‚³‚ÌŒvZ
//    DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&startPos);
//    DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&endPos);
//    DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, S);
//    float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(V));
//
//    if (length < 0.0001f) return; // ’·‚³‚ª‚Ù‚Úƒ[ƒ‚È‚çˆ—‚µ‚È‚¢
//
//    // 2. ³‹K‰»‚µ‚½•ûŒüi‘O•ûŒü Zj
//    DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(V);
//
//    // 3. ‰EEã•ûŒü‚ÌZoiƒrƒ‹ƒ{[ƒh“I‚ÈŒvZj
//    DirectX::XMVECTOR worldUp = DirectX::g_XMIdentityR1; // (0,1,0)
//    // •ûŒü‚ª^ã‚ğŒü‚¢‚Ä‚¢‚éê‡‚Ì‰ñ”ğ
//    if (fabsf(DirectX::XMVectorGetY(dir)) > 0.999f) worldUp = DirectX::g_XMIdentityR2;
//
//    DirectX::XMVECTOR right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(worldUp, dir));
//    DirectX::XMVECTOR up = DirectX::XMVector3Cross(dir, right);
//
//    // 4. ƒXƒP[ƒ‹s—ñiZ•ûŒü‚É‚Ì‚İ’·‚³‚ğL‚Î‚·j
//    // ¦‚à‚µBoxƒ‚ƒfƒ‹‚ªu’†S‚©‚ç‘OŒã0.5‚¸‚Âv‚È‚çAZƒXƒP[ƒ‹‚ğlength‚É‚·‚é
//    DirectX::XMMATRIX matScale = DirectX::XMMatrixScaling(width, width, length);
//
//    // 5. ‰ñ“]E•ÀiiˆÚ“®js—ñ
//    DirectX::XMMATRIX matRotTrans;
//    matRotTrans.r[0] = right;
//    matRotTrans.r[1] = up;
//    matRotTrans.r[2] = dir;
//    // ‚±‚±‚ªƒ|ƒCƒ“ƒgF”z’uêŠ‚ğu’†S(center)v‚Å‚Í‚È‚­un“_(startPos)v‚É‚·‚é
//    // ¦‚½‚¾‚µƒ‚ƒfƒ‹‚ªu’†SŒ´“_v‚È‚çAcenter‚É‚·‚é•K—v‚ª‚ ‚è‚Ü‚·B
//    DirectX::XMVECTOR center = DirectX::XMVectorLerp(S, E, 0.5f);
//    matRotTrans.r[3] = DirectX::XMVectorSetW(center, 1.0f);
//
//    // 6. ‡¬
//    DirectX::XMStoreFloat4x4(&transform, matScale * matRotTrans);
//
//
//    // center ‚ª XMVECTOR ‚Ìê‡
//    DirectX::XMMATRIX debugMat = DirectX::XMMatrixIdentity();
//    // w¬•ª‚ğ1.0‚É‚·‚é‚½‚ß‚É XMVectorSetW ‚ğ’Ê‚·‚Ì‚ªˆÀ‘S‚Å‚·
//    debugMat.r[3] = DirectX::XMVectorSetW(center, 1.0f);
//    DirectX::XMStoreFloat4x4(&transform, debugMat);
//
//}
//
////‰¼
//void LaserBeam::Update(float elapsedTime)
//{
//	// ƒr[ƒ€‚ÌƒGƒtƒFƒNƒgXVi—á: ƒAƒjƒ[ƒVƒ‡ƒ“UVj
//	UpdateTransform();
//    UpdateColliders();
//}
//
////‰¼
//void LaserBeam::Render(const RenderContext& rc, ModelRenderer* renderer)
//{
//	// ƒr[ƒ€‚Ì•`‰æ
//	//renderer->DrawModel(rc, model, transform);
//}
//
//void LaserBeam::UpdateColliders()
//{
//    //// ‘«ê‚Í start ¨ end ‚Ì’†“_
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
//    //// Z ²‚ğƒŒ[ƒU[•ûŒü‚ÉŒü‚¯‚é‰ñ“]s—ñ
//    //DirectX::XMMATRIX rotMat =
//    //    DirectX::XMMatrixLookToRH(
//    //        DirectX::XMVectorZero(),
//    //        direction,
//    //        DirectX::XMVectorSet(0, 1, 0, 0)
//    //    );
//
//    //// ‘«êƒRƒ‰ƒCƒ_[i”–‚¢” j
//    //topCollider.SetCenter({ center.x,center.y + 0.4f,center.z });
//    //topCollider.SetSize({ 1.0f, 0.1f, length });
//    //
//
//    //// ‘¤–Ê‚à“¯—l‚É”–‚­ì‚é
//    //sideCollider.SetCenter({ center.x,center.y - 0.8f,center.z });
//    //sideCollider.SetSize({ 1.2f, 0.4f, length });
//    {
//        // ’†“_
//        DirectX::XMFLOAT3 center =
//        {
//            (startPos.x + endPos.x) * 0.5f,
//            (startPos.y + endPos.y) * 0.5f,
//            (startPos.z + endPos.z) * 0.5f
//        };
//
//        // •ûŒüƒxƒNƒgƒ‹
//        DirectX::XMVECTOR dir = DirectX::XMVectorSet(
//            endPos.x - startPos.x,
//            endPos.y - startPos.y,
//            endPos.z - startPos.z,
//            0.0f
//        );
//
//        // ³‹K‰»
//        dir = DirectX::XMVector3Normalize(dir);
//
//        // ’·‚³
//        float length = DirectX::XMVectorGetX(
//            DirectX::XMVector3Length(
//                DirectX::XMVectorSubtract(
//                    DirectX::XMLoadFloat3(&endPos),
//                    DirectX::XMLoadFloat3(&startPos)
//                )
//            )
//        );
//
//        // Z ²‚ğƒŒ[ƒU[•ûŒü‚ÉŒü‚¯‚é‰ñ“]s—ñ
//        DirectX::XMMATRIX rotMat =
//            DirectX::XMMatrixLookToRH(
//                DirectX::XMVectorZero(),
//                dir,
//                DirectX::XMVectorSet(0, 1, 0, 0)
//            );
//
//        // ‘«êƒRƒ‰ƒCƒ_[
//        topCollider.SetCenter({ center.x, center.y + 0.4f, center.z });
//        topCollider.SetSize({ 1.0f, 0.1f, length });
//        topCollider.SetRotationMatrix(rotMat);  // © ‚±‚ê‚¾‚¯‚ÅOK
//
//        // ‘¤–ÊƒRƒ‰ƒCƒ_[
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

    // ƒXƒe[ƒW’†Si•K—v‚È‚ç•ÏXj
    DirectX::XMFLOAT3 center = { 0, 0, 0 };

    // Q/E ‚Å‰ñ“]
    float step = DirectX::XM_PI / 4.0f;

    if (!isRotating)
    {

        if (GetAsyncKeyState('Q') & 0x0001)
        {
            targetAngleY -= step;
            isRotating = true;   // © ‰ñ“]ŠJn
        }
        if (GetAsyncKeyState('E') & 0x0001)
        {
            targetAngleY += step;
            isRotating = true;   // © ‰ñ“]ŠJn
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

        // “™‘¬‰ñ“]i—áF1•b‚Å90‹j
        float delta = (DirectX::XM_PI / 2.0f) * elapsedTime * dir;

        if (fabs(delta) > fabs(diff))
            delta = diff;

        currentAngleY += delta;

        RotateAroundCenter(center, delta);
    }


    // ƒŒ[ƒU[‚Ì”½ËŒvZ
    beam.origin = startPos;
    beam.direction = direction;
    // LaserBeam ‚ªƒŒ[ƒU[‚ğŒ‚‚Âi”½ËŠÜ‚Şj
    beam.Update(elapsedTime);
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    //beam.Render(rc, renderer);

    StageObject::Render(rc, renderer);
}
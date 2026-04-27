#include "Laser.h"
#include"Collision.h"

void Laser::Initialize(
    const DirectX::XMFLOAT3& emitterPos,
    const DirectX::XMFLOAT3& dir,
    float maxLen)
{
    startPos = emitterPos;
    direction = dir;

    // 正規化
    DirectX::XMVECTOR v = DirectX::XMLoadFloat3(&direction);
    v = DirectX::XMVector3Normalize(v);
    DirectX::XMStoreFloat3(&direction, v);

    maxLength = maxLen;

    Shoot();
}

void Laser::Update(float elapsedTime)
{
    if (!isActive) return;

    Shoot();

    beam.SetPoints(startPos, endPos);
    beam.Update(elapsedTime);

    UpdateColliders();
    ResolvePlayerCollision();
}

void Laser::Shoot()
{
    DirectX::XMFLOAT3 pos = startPos;
    DirectX::XMFLOAT3 dir = direction;

    endPos = startPos;

    float remaining = maxLength;

    for (int i = 0; i < 5; i++) // 反射回数制限
    {
        DirectX::XMFLOAT3 hitPos;
        DirectX::XMFLOAT3 normal;

        ////次ここ触る　コメントの引数のところにステージの全てのオブジェクトを持っているマネージャーのようなクラスを作ってそのクラスの情報を渡すかそのクラスでレイキャスト関数を書いてそのメンバ関数を呼び出せばいい
        //bool hit = Collision::RayCast(
        //    pos,
        //    DirectX::XMFLOAT3(
        //        pos.x + dir.x * remaining,
        //        pos.y + dir.y * remaining,
        //        pos.z + dir.z * remaining
        //    ),
        //    /* world matrix */,
        //    /* mirror or stage model */,
        //    hitPos,
        //    normal
        //);

        bool hit = manager->RayCast(
            pos,
            DirectX::XMFLOAT3(
                pos.x + dir.x * remaining,
                pos.y + dir.y * remaining,
                pos.z + dir.z * remaining
            ),
            hitPos,
            normal
        );

        if (!hit)
        {
            endPos = {
                pos.x + dir.x * remaining,
                pos.y + dir.y * remaining,
                pos.z + dir.z * remaining
            };
            break;
        }

        endPos = hitPos;

        auto Distance = [](const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
            {
                float dx = a.x - b.x;
                float dy = a.y - b.y;
                float dz = a.z - b.z;
                return sqrtf(dx * dx + dy * dy + dz * dz);
            };

        remaining -= Distance(pos, hitPos);

        pos = hitPos;

        // 反射
        dir = Reflect(dir, normal);
    }
}

DirectX::XMFLOAT3 Laser::Reflect(
    const DirectX::XMFLOAT3& inDir,
    const DirectX::XMFLOAT3& normal)
{
    DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&inDir);
    DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normal);

    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, n));

    DirectX::XMVECTOR result =
        DirectX::XMVectorSubtract(
            d,
            DirectX::XMVectorScale(n, 2.0f * dot)
        );

    DirectX::XMFLOAT3 out;
    DirectX::XMStoreFloat3(&out, DirectX::XMVector3Normalize(result));

    return out;
}

void Laser::UpdateColliders()
{
    // 足場は start → end の中点
    DirectX::XMFLOAT3 center =
    {
        (startPos.x + endPos.x) * 0.5f,
        (startPos.y + endPos.y) * 0.5f,
        (startPos.z + endPos.z) * 0.5f
    };

    float length =
        sqrtf(
            (endPos.x - startPos.x) * (endPos.x - startPos.x) +
            (endPos.y - startPos.y) * (endPos.y - startPos.y) +
            (endPos.z - startPos.z) * (endPos.z - startPos.z)
        );

    // 足場コライダー（薄い箱）
    topCollider.SetCenter(center);
    topCollider.SetSize({ 0.5f, 0.1f, length });

    // 側面も同様に薄く作る
    sideCollider.SetCenter(center);
    sideCollider.SetSize({ 0.1f, 0.5f, length });
}

void Laser::ResolvePlayerCollision()
{
    Player& player = GetPlayer();

    CollisionResult result =
        player.collider.Intersect(topCollider);

    if (result.hit)
    {
        // 上から乗った場合
        if (result.normal.y > 0.5f)
        {
            player.position.y += result.pushOut.y;
            player.isGrounded = true;
        }
    }

    // 側面
    result = player.collider.Intersect(sideCollider);

    if (result.hit)
    {
        player.position.x += result.pushOut.x;
        player.position.z += result.pushOut.z;
    }
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    beam.Render(rc, renderer);

    StageObject::Render(rc, renderer);
}
#include "Laser.h"
#include"Collision.h"

//レーザービーム
void LaserBeam::UpdateTransform()
{
    // ① 方向ベクトル
    DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&startPos);
    DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&endPos);

    DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, S);

    // ② 長さ
    float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(V));

    // ③ 正規化（前方向）
    DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(V);

    // ④ 上方向（とりあえずワールドUP）
    DirectX::XMVECTOR up = DirectX::g_XMIdentityR1; // (0,1,0)

    // ⑤ 右方向
    DirectX::XMVECTOR right = DirectX::XMVector3Cross(up, dir);

    // 上方向再計算（直交化）
    up = DirectX::XMVector3Cross(dir, right);

    right = DirectX::XMVector3Normalize(right);
    up = DirectX::XMVector3Normalize(up);

    // ⑥ 中心位置
    DirectX::XMVECTOR center = DirectX::XMVectorLerp(S, E, 0.5f);

    // ⑦ 行列作成
    DirectX::XMMATRIX mat;

    mat.r[0] = right;
    mat.r[1] = up;
    mat.r[2] = dir;
    mat.r[3] = DirectX::XMVectorSet(
        DirectX::XMVectorGetX(center),
        DirectX::XMVectorGetY(center),
        DirectX::XMVectorGetZ(center),
        1.0f
    );

    // ⑧ スケール（長さと太さ）
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(width, width, length);

    mat = scale * mat;

    // ⑨ 保存
    DirectX::XMStoreFloat4x4(&transform, mat);
}

//仮
void LaserBeam::Update(float elapsedTime)
{
	// ビームのエフェクト更新（例: アニメーションUV）
	UpdateTransform();
}

//仮
void LaserBeam::Render(const RenderContext& rc, ModelRenderer* renderer)
{
	// ビームの描画
	//renderer->DrawModel(rc, model, transform);
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
    
}

void Laser::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    if (!isActive) return;

    beam.Render(rc, renderer);

    StageObject::Render(rc, renderer);
}
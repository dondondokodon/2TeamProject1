#include "StageGrid.h"
#include"Collision.h"
#include"Player.h"

StageGrid::StageGrid()  
{
    // ステージモデル（木箱）を読み込み
    model = new Model("Data/Model/Objects/Box/Box_1cm.mdl");

    aabbMin = { 0.0f, 0.0f, 0.0f };
    aabbMax = { 0.0f, 0.0f, 0.0f };
    scale = { 1.0f,1.0f,1.0f };
    pos = { 0.5f, 0.5f, -5.5f };
}

StageGrid::~StageGrid()
{
    // モデル破棄
    delete model;
}

void StageGrid::Update(float elapsedTime,Player& p)
{
  

    float moveSpeed = 2.0f;
    float moveAmount = 1.0f;

    bool nowP = (GetAsyncKeyState('P') & 0x8000);
    bool trgP = (nowP && !prevP);

    // ---------------------------------------------------------
    // ★ プレイヤーが木箱の正面を向いているか判定（毎フレーム）
    // ---------------------------------------------------------
    auto playerPos = p.GetPosition();
    auto playerForward = p.GetForward();

    DirectX::XMFLOAT3 toBox = {
        pos.x - playerPos.x,
        0,
        pos.z - playerPos.z
    };

    float len = sqrtf(toBox.x * toBox.x + toBox.z * toBox.z);
    if (len > 0.0001f) {
        toBox.x /= len;
        toBox.z /= len;
    }

    float dot = playerForward.x * toBox.x + playerForward.z * toBox.z;
    bool isFacingBox = (dot > 0.7f);

    // ★ 触れていて、正面を向いていて、P を押した瞬間に移動開始 ↓player1のみ
    if (isTouchingPlayer && isFacingBox && trgP && !isMoving && !p.GetIsRobot())
    {
        float dx = playerPos.x - pos.x;
        float dz = playerPos.z - pos.z;

        if (fabs(dx) > fabs(dz))
        {
            moveDir = (dx > 0) ? DirectX::XMFLOAT3{ -1,0,0 } : DirectX::XMFLOAT3{ 1,0,0 };
        }
        else
        {
            moveDir = (dz > 0) ? DirectX::XMFLOAT3{ 0,0,-1 } : DirectX::XMFLOAT3{ 0,0,1 };
        }

        isMoving = true;
        moveRemain = moveAmount;
    }



    // ---------------------------------------------------------
    // ★ 移動中：ゆっくり moveSpeed で動く
    // ---------------------------------------------------------
    if (isMoving)
    {
        float delta = moveSpeed * elapsedTime;  // 今フレームで動く量

        // 残り距離を超えないように調整
        if (delta > moveRemain)
            delta = moveRemain;

        // 実際に移動
        pos.x += moveDir.x * delta;
        pos.z += moveDir.z * delta;

        // 残り移動量を減らす
        moveRemain -= delta;

        // 規定量動いたら停止
        if (moveRemain <= 0.0f)
        {
            isMoving = false;
        }
    }

    // ---------------------------------------------------------
    // ★ AABB（当たり判定）の更新
    // ---------------------------------------------------------
    aabbMin = {
        pos.x - 0.5f * scale.x,
        pos.y - 0.5f * scale.y,
        pos.z - 0.5f * scale.z
    };

    aabbMax = {
        pos.x + 0.5f * scale.x,
        pos.y + 0.5f * scale.y,
        pos.z + 0.5f * scale.z
    };

    // Pキーの状態を保存
    prevP = nowP;
}



//描画処理
void StageGrid::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    // スケールと位置を行列に変換
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

    DirectX::XMMATRIX M = S * T;

    DirectX::XMFLOAT4X4 transform;
    DirectX::XMStoreFloat4x4(&transform, M);

    // モデル描画
    renderer->Render(rc, transform, model, ShaderId::Lambert);
}

//デバッグプリミティブ描画
void StageGrid::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
    float debugScale = 0.55f; // デバッグ用に少し小さく描く

    // AABB の中心
    DirectX::XMFLOAT3 center = {
        (aabbMin.x + aabbMax.x) * 0.5f,
        (aabbMin.y + aabbMax.y) * 0.5f,
        (aabbMin.z + aabbMax.z) * 0.5f
    };

    // AABB のサイズ（縮小版）
    DirectX::XMFLOAT3 size = {
        (aabbMax.x - aabbMin.x) * debugScale,
        (aabbMax.y - aabbMin.y) * debugScale,
        (aabbMax.z - aabbMin.z) * debugScale
    };

    DirectX::XMFLOAT3 angle = { 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 1, 0, 0, 1 };

    renderer->RenderBox(rc, center, angle, size, color);
}


void StageGrid::CollisionVsPlayer(Player& p)
{
    DirectX::XMFLOAT3 push;

    // ---------------------------------------------------------
    // ★ プレイヤー（円柱） vs 木箱（AABB）の衝突判定
    // ---------------------------------------------------------
    if (Collision::IntersectCylinderVsAABB(
        p.GetPosition(),
        p.GetRadius(),
        p.GetHeight(),
        aabbMin,
        aabbMax,
        push))
    {
        // プレイヤーを押し戻す（めり込み補正）
        DirectX::XMFLOAT3 pos = p.GetPosition();
        pos.x += push.x;
        pos.z += push.z;
        p.SetPosition(pos);

        // プレイヤーが触れているフラグを立てる
        isTouchingPlayer = true;
    }
}

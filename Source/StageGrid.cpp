#include "StageGrid.h"
#include"Collision.h"
#include"Player.h"



StageGrid::StageGrid()
{
    model = std::make_unique<Model>("Data/Model/Objects/Box/Box.mdl");
    pushEffect = std::make_unique<Effect>("Data/Effect/hikizuri.efkefc");

    position = { 0.0f, 0.0f, 0.0f };
    scale = { 1.0f, 1.0f, 1.0f };   // ← 必須
    angle = { 0.0f, 0.0f, 0.0f };

    isMoving = false;
    moveRemain = 0.0f;
    moveDir = { 0,0,0 };

    aabbMin = { 0,0,0 };
    aabbMax = { 0,0,0 };

    gridX = 0;
    gridZ = 0;
}


StageGrid::~StageGrid()
{
}

void StageGrid::Update(float elapsedTime)
{
    float moveSpeed = 2.0f;

    bool nowP = (GetAsyncKeyState('P') & 0x8000);

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
        // 今フレームで木箱が動く量
        float moveX = moveDir.x * delta;
        float moveZ = moveDir.z * delta;

        // 木箱を移動する
        position.x += moveX;
        position.z += moveZ;

        // 木箱を押しているプレイヤーも同じ量だけ動かす
        // これをしないと、木箱だけ進んでプレイヤーが置いていかれる
        if (pushingPlayer)
        {
            DirectX::XMFLOAT3 playerPos = pushingPlayer->GetPosition();
            playerPos.x += moveX;
            playerPos.z += moveZ;
            pushingPlayer->SetPosition(playerPos);
        }

        // 残り移動量を減らす
        moveRemain -= delta;

        // 規定量動いたら停止
        if (moveRemain <= 0.0f)
        {
            isMoving = false;

            //固定用・消す
            if (pushingPlayer)
            {
                pushingPlayer->StopBoxPush();
            }

            pushingPlayer = nullptr;
        }
    }

    // ---------------------------------------------------------
    // ★ AABB（当たり判定）の更新
    // ---------------------------------------------------------
    aabbMin = {
        position.x - 2.4f,
        position.y - 2.4f,
        position.z - 2.4f
    };

    aabbMax = {
        position.x + 2.4f,
        position.y + 2.4f,
        position.z + 2.4f
    };

    // Pキーの状態を保存
    prevP = nowP;

    isTouchingPlayer = false;
    UpdateTransform();
}



//描画処理
void StageGrid::Render(const RenderContext& rc, ModelRenderer* renderer)
{
    // スケールと位置を行列に変換
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

    DirectX::XMMATRIX M = S * T;

    DirectX::XMFLOAT4X4 transform;
    DirectX::XMStoreFloat4x4(&transform, M);

    // モデル描画
    renderer->Render(rc, transform, model.get(), ShaderId::Lambert);
}

//デバッグプリミティブ描画
void StageGrid::RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer)
{
  

    // AABB の中心
    DirectX::XMFLOAT3 center = {
      (aabbMin.x + aabbMax.x) * 0.5f,
      (aabbMin.y + aabbMax.y) * 0.5f,
      (aabbMin.z + aabbMax.z) * 0.5f
    };


    // AABB のサイズ（縮小版）
    DirectX::XMFLOAT3 size = {
      (aabbMax.x - aabbMin.x) * 0.6f,
      (aabbMax.y - aabbMin.y) * 0.6f,
      (aabbMax.z - aabbMin.z) * 0.6f
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
        pos.y += push.y;
        pos.z += push.z;
        p.SetPosition(pos);

        // プレイヤーが触れているフラグを立てる
        isTouchingPlayer = true;

        bool nowP = (GetAsyncKeyState('P') & 0x8000);
        bool trgP = (nowP && !prevP);

        auto playerPos = p.GetPosition();
        auto playerForward = p.GetForward();

        // 操作中のプレイヤーだけ、かつロボット以外だけ木箱を押せる
        if (trgP && !isMoving && p.GetIsControlling() && !p.GetIsRobot())
        {
            DirectX::XMFLOAT3 toBox = {
            position.x - playerPos.x,
            0,
            position.z - playerPos.z
            };

            float len = sqrtf(toBox.x * toBox.x + toBox.z * toBox.z);
            if (len > 0.0001f) {
                toBox.x /= len;
                toBox.z /= len;
            }

            float dot = playerForward.x * toBox.x + playerForward.z * toBox.z;
            isFacingBox = (dot > 0.7f);
            if (isTouchingPlayer && isFacingBox)
            {
                // 箱が実際に動き始めた時だけ、Pushアニメを再生する
                //if (StartMove(p))
                //{
                //    p.PlayPushAnimation();
                //}
                if (StartMove(p)) //木箱押すまでの固定用
                {
                    p.StartBoxPush();

					// プッシュエフェクト再生
                    if (pushEffect)
                    {
                        DirectX::XMFLOAT3 effectPos = position;

                        // エフェクトを箱の中心から押している側へずらす距離
                        // 値を大きくすると、エフェクトが箱から離れる
                        const float effectBackOffset = 2.0f;

                        // エフェクトの高さ
                        // 値を大きくすると、エフェクトが上に出る
                        const float effectHeight = 0.05f;

                        // エフェクトの大きさ
                        // 値を大きくすると、エフェクトが大きく表示される
                        const float effectScale = 0.08f;

                        effectPos.x -= moveDir.x * effectBackOffset;
                        effectPos.z -= moveDir.z * effectBackOffset;
                        effectPos.y += effectHeight;

                        pushEffect->Play(effectPos, effectScale);
                    }
                }
            }
        }
    }
}

bool StageGrid::StartMove(Player& player)
{
    const int limit = 111111115;

    int nextX = gridX;
    int nextZ = gridZ;

    DirectX::XMFLOAT3 targetPos = player.GetPosition();

    float dx = targetPos.x - position.x;
    float dz = targetPos.z - position.z;

	// プレイヤーから見て、どの方向に箱を動かすかを決める
    if (fabs(dx) > fabs(dz))
    {
        moveDir = (dx > 0) ? DirectX::XMFLOAT3{ -1, 0, 0 } : DirectX::XMFLOAT3{ 1, 0, 0 };
        nextX += (moveDir.x > 0 ? 1 : -1);
    }
    else
    {
        moveDir = (dz > 0) ? DirectX::XMFLOAT3{ 0, 0, -1 } : DirectX::XMFLOAT3{ 0, 0, 1 };
        nextZ += (moveDir.z > 0 ? 1 : -1);
    }

    // 動けない場合は、押しているプレイヤーも登録しない
    if (abs(nextX) > limit || abs(nextZ) > limit)
    {
        return false;
    }

    gridX = nextX;
    gridZ = nextZ;

    // ここまで来たら、箱は実際に動く
    pushingPlayer = &player;

    isMoving = true;
    moveRemain = 1.0f;

    return true;
}

void StageGrid::CollisionVsStage(StageObjectManager& stageObjectManager)
{
    if (!isMoving) return;

    // --------------------------------------
    // ★ 進行方向ベクトル
    // --------------------------------------
    DirectX::XMFLOAT3 forward =
    {
        moveDir.x,
        0.0f,
        moveDir.z
    };

    // --------------------------------------
    // ★ 箱の半径（AABB基準）
    // --------------------------------------
    float startOffset = 2.4f;

    // --------------------------------------
    // ★ レイ発射位置（箱の外側）
    // --------------------------------------
    DirectX::XMFLOAT3 start =
    {
        position.x + forward.x * startOffset,
        position.y + 0.5f,
        position.z + forward.z * startOffset
    };

    // --------------------------------------
    // ★ 進行方向へ少しだけレイを飛ばす
    // --------------------------------------
    float rayLength = 0.6f;

    const float offset = 0.3f;

    DirectX::XMFLOAT3 starts[3] =
    {
        start,
        { start.x, start.y, start.z + offset },
        { start.x, start.y, start.z - offset }
    };

    DirectX::XMFLOAT3 ends[3] =
    {
        {
            starts[0].x + moveDir.x * rayLength,
            starts[0].y,
            starts[0].z + moveDir.z * rayLength
        },
        {
            starts[1].x + moveDir.x * rayLength,
            starts[1].y,
            starts[1].z + moveDir.z * rayLength
        },
        {
            starts[2].x + moveDir.x * rayLength,
            starts[2].y,
            starts[2].z + moveDir.z * rayLength
        }
    };

    DirectX::XMFLOAT3 hitPos, hitNormal;
    RayHitResult result = stageObjectManager.RayCastAny(
        starts,
        ends,
        3,
        this,
        hitPos,
        hitNormal);

    if (result.hit)
    {
        isMoving = false;
        moveRemain = 0.0f;

		//固定用・消す
        if (pushingPlayer)
        {
            pushingPlayer->StopBoxPush();
        }

        pushingPlayer = nullptr;
        return;
    }
}

void StageGrid::CollisionVsGrid(
    std::vector<StageGrid*>& grids)
{
    // 移動中だけ判定
    if (!isMoving)
        return;

    // -----------------------------
    // 次の位置
    // -----------------------------
    DirectX::XMFLOAT3 nextPos =
    {
        position.x + moveDir.x * moveRemain,
        position.y,
        position.z + moveDir.z * moveRemain
    };

    // -----------------------------
    // 次フレームAABB
    // -----------------------------
    DirectX::XMFLOAT3 nextMin =
    {
        nextPos.x - 2.4f,
        nextPos.y - 2.4f,
        nextPos.z - 2.4f
    };

    DirectX::XMFLOAT3 nextMax =
    {
        nextPos.x + 2.4f,
        nextPos.y + 2.4f,
        nextPos.z + 2.4f
    };

    // -----------------------------
    // 全木箱と判定
    // -----------------------------
    for (StageGrid* grid : grids)
    {
        // 自分は無視
        if (grid == this)
            continue;

        // AABB判定
        if (Collision::IntersectAABBVsAABB(
            nextMin,
            nextMax,
            grid->GetAABBMin(),
            grid->GetAABBMax()))
        {
            // 停止
            isMoving = false;
            moveRemain = 0.0f;

			//固定用・消す
            if (pushingPlayer)
            {
                pushingPlayer->StopBoxPush();
            }

            pushingPlayer = nullptr;

            return;

        }
    }
}

void StageGrid::CollisionVsMirror(
    std::vector<Mirror*>& mirrors)
{
    if (!isMoving)
        return;

    // 次位置
    DirectX::XMFLOAT3 nextPos =
    {
        position.x + moveDir.x * moveRemain,
        position.y,
        position.z + moveDir.z * moveRemain
    };

    // 次AABB
    DirectX::XMFLOAT3 nextMin =
    {
        nextPos.x - 2.4f,
        nextPos.y - 2.4f,
        nextPos.z - 2.4f
    };

    DirectX::XMFLOAT3 nextMax =
    {
        nextPos.x + 2.4f,
        nextPos.y + 2.4f,
        nextPos.z + 2.4f
    };

    for (Mirror* mirror : mirrors)
    {
        if (Collision::IntersectAABBVsAABB(
            nextMin,
            nextMax,
            mirror->GetAABBMin(),
            mirror->GetAABBMax()))
        {
            // 木箱停止
            isMoving = false;
            moveRemain = 0.0f;

			//固定用・消す
            if (pushingPlayer)
            {
                pushingPlayer->StopBoxPush();
            }

            pushingPlayer = nullptr;

            return;
        }
    }
}

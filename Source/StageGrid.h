#pragma once

#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include"Player.h"
#include"../Source/stage.h"
#include"StageObject.h"
#include"StageObjectManager.h"
#include"Mirror.h"

class Mirror;

// ------------------------------------------------------------
// StageGrid
// ・木箱（押せるブロック）を表すクラス
// ・プレイヤーが触れて P を押すと 1 グリッド分だけ動く
// ・AABB を使ってプレイヤーとの衝突判定を行う
// ------------------------------------------------------------
class StageGrid : public StageObject
{
public:

    StageGrid();
    ~StageGrid();

    // 毎フレーム更新処理（移動処理・AABB更新など）
    void Update(float elapsedTime);

    // モデル描画
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    // プレイヤーとの衝突処理（Cylinder vs AABB）
    void CollisionVsPlayer(Player& p);

    // デバッグ用 AABB 描画
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

    // 動かせたら true、動かせなかったら false
    bool StartMove(Player& player);

    void CollisionVsStage(StageObjectManager& stageObjectManager);

    void CollisionVsGrid(
        std::vector<StageGrid*>& grids);

    void CollisionVsMirror(
        std::vector<Mirror*>& mirrors);

    DirectX::XMFLOAT3 GetAABBMin() const { return aabbMin; }
    DirectX::XMFLOAT3 GetAABBMax() const { return aabbMax; }

    RayHitResult ReallyHit(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 hitPos, DirectX::XMFLOAT3 hitNormal) override {
        // StageGrid は「絶対に」レーザーを止める存在であることを保証する
        RayHitResult result;
        result.hit = true;
        result.object = this;
        result.type = RayHitType::Stop; // 強制的に Stop を返す
        result.hitPos = hitPos;
        return result;
    }


private:
    // --------------------------------------------------------
    // ★ 木箱の移動状態管理
    // --------------------------------------------------------
public:
    bool isTouchingPlayer = false; // プレイヤーが触れているか
private:
    bool prevP = false;            // Pキーの前フレーム状態
    bool isMoving = false;         // 現在移動中か
    bool isFacingBox=false;
    float moveRemain = 0.0f;       // 残り移動距離（1.0f 分動く）
    DirectX::XMFLOAT3 moveDir = { 0,0,0 }; // 移動方向（±X or ±Z）

    int gridX;
    int gridZ;

    // この木箱を押しているプレイヤー
    // 木箱が移動している間、プレイヤーも同じ量だけ動かすために使う
    Player* pushingPlayer = nullptr;


    // --------------------------------------------------------
    // ★ AABB（当たり判定用）
    //   pos と scale から毎フレーム計算される
    // --------------------------------------------------------
    DirectX::XMFLOAT3 aabbMin; // 最小点
    DirectX::XMFLOAT3 aabbMax; // 最大点
};

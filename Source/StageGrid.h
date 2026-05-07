#pragma once

#include"System/ModelRenderer.h"
#include"System/ShapeRenderer.h"
#include"Player.h"
#include"../Source/stage.h"

// ------------------------------------------------------------
// StageGrid
// ・木箱（押せるブロック）を表すクラス
// ・プレイヤーが触れて P を押すと 1 グリッド分だけ動く
// ・AABB を使ってプレイヤーとの衝突判定を行う
// ------------------------------------------------------------
class StageGrid : public Stage
{
public:
    StageGrid(const char* filename);
    StageGrid();
    ~StageGrid();

    // 毎フレーム更新処理（移動処理・AABB更新など）
    void Update(float elapsedTime, Player& p);

    // モデル描画
    void Render(const RenderContext& rc, ModelRenderer* renderer);

    // プレイヤーとの衝突処理（Cylinder vs AABB）
    void CollisionVsPlayer(Player& p);

    // デバッグ用 AABB 描画
    void RenderDebugPrimitive(const RenderContext& rc, ShapeRenderer* renderer);

private:
    // 木箱モデル
    Model* model = nullptr;

    // --------------------------------------------------------
    // ★ 木箱の移動状態管理
    // --------------------------------------------------------
public:
    bool isTouchingPlayer = false; // プレイヤーが触れているか
private:
    bool prevP = false;            // Pキーの前フレーム状態
    bool isMoving = false;         // 現在移動中か
    float moveRemain = 0.0f;       // 残り移動距離（1.0f 分動く）
    DirectX::XMFLOAT3 moveDir = { 0,0,0 }; // 移動方向（±X or ±Z）

    // --------------------------------------------------------
    // ★ 木箱の位置・大きさ
    // --------------------------------------------------------
    DirectX::XMFLOAT3 scale;     // 木箱の大きさ
    DirectX::XMFLOAT3 pos; // 木箱の中心位置

    // --------------------------------------------------------
    // ★ AABB（当たり判定用）
    //   pos と scale から毎フレーム計算される
    // --------------------------------------------------------
    DirectX::XMFLOAT3 aabbMin; // 最小点
    DirectX::XMFLOAT3 aabbMax; // 最大点
};

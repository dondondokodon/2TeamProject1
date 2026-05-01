#pragma once
#include"CollisionResult.h"
class CylinderCollider
{
public:
    CylinderCollider() = default;

    void SetCenter(const DirectX::XMFLOAT3& c) { center = c; }
    void SetRadius(float r) { radius = r; }
    void SetHeight(float h) { height = h; }

    const DirectX::XMFLOAT3& GetCenter() const { return center; }
    float GetRadius() const { return radius; }
    float GetHeight() const { return height; }

    // レーザー（カプセル）との判定用
    // Laser側から呼び出すか、ここに追加する
    // CollisionResult IntersectLaser(const LaserBeam& laser) const;

private:
    DirectX::XMFLOAT3 center = { 0, 0, 0 };
    float radius = 1.0f;
    float height = 2.0f;
};


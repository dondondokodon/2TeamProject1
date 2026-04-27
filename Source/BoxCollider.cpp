#include "BoxCollider.h"
CollisionResult BoxCollider::Intersect(const BoxCollider& other) const
{
    CollisionResult result{};

    // this = Player
    // other = Laser など

    float dx = other.center.x - center.x;
    float dy = other.center.y - center.y;
    float dz = other.center.z - center.z;

    // 半サイズ同士の重なり量
    float px = (size.x + other.size.x) - fabsf(dx);
    float py = (size.y + other.size.y) - fabsf(dy);
    float pz = (size.z + other.size.z) - fabsf(dz);

    // 全軸で重なっていれば衝突
    if (px > 0.0f && py > 0.0f && pz > 0.0f)
    {
        result.hit = true;

        // 一番浅いめり込み方向を押し戻し方向にする
        if (px < py && px < pz)
        {
            // X方向
            result.pushOut.x = (dx > 0.0f) ? -px : px;

            // normal は「this を押し出す向き」
            result.normal =
            {
                (dx > 0.0f) ? -1.0f : 1.0f,
                0.0f,
                0.0f
            };
        }
        else if (py < pz)
        {
            // Y方向
            result.pushOut.y = (dy > 0.0f) ? -py : py;

            // 上から乗った時に normal.y > 0 になるようにする
            result.normal =
            {
                0.0f,
                (dy > 0.0f) ? -1.0f : 1.0f,
                0.0f
            };
        }
        else
        {
            // Z方向
            result.pushOut.z = (dz > 0.0f) ? -pz : pz;

            result.normal =
            {
                0.0f,
                0.0f,
                (dz > 0.0f) ? -1.0f : 1.0f
            };
        }
    }

    return result;
}
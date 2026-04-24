#include "BoxCollider.h"

CollisionResult BoxCollider::Intersect(const BoxCollider& other) const
{
    CollisionResult result;

    float dx = other.center.x - center.x;
    float dy = other.center.y - center.y;
    float dz = other.center.z - center.z;

    float px = (size.x + other.size.x) - fabsf(dx);
    float py = (size.y + other.size.y) - fabsf(dy);
    float pz = (size.z + other.size.z) - fabsf(dz);

    if (px > 0 && py > 0 && pz > 0)
    {
        result.hit = true;

        // ˆê”Ôó‚¢‚ß‚è‚İ•ûŒü‚ğ‰Ÿ‚µ–ß‚µ•ûŒü‚É‚·‚é
        if (px < py && px < pz)
        {
            result.pushOut.x = (dx > 0) ? px : -px;
            result.normal = { (dx > 0) ? 1.0f : -1.0f, 0, 0 };
        }
        else if (py < pz)
        {
            result.pushOut.y = (dy > 0) ? py : -py;
            result.normal = { 0, (dy > 0) ? 1.0f : -1.0f, 0 };
        }
        else
        {
            result.pushOut.z = (dz > 0) ? pz : -pz;
            result.normal = { 0, 0, (dz > 0) ? 1.0f : -1.0f };
        }
    }

    return result;
}
#pragma once

#include "../core/Types.h"

class CollisionDetector {
public:
    static bool checkCollision(const Rectangle& a, const Rectangle& b);
    static bool checkPointInRect(const Vector2& point, const Rectangle& rect);
    static bool checkCircleRect(const Vector2& center, float radius, const Rectangle& rect);
    
    static CollisionInfo sweepAABB(const Rectangle& moving, const Vector2& velocity, const Rectangle& stationary);
    static bool rayIntersectRect(const Vector2& rayStart, const Vector2& rayDir, const Rectangle& rect, float& t);
    
    static Vector2 closestPointOnRect(const Vector2& point, const Rectangle& rect);
    static float distanceSquared(const Vector2& a, const Vector2& b);
};
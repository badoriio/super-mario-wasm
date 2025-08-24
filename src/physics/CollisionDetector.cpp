#include "CollisionDetector.h"
#include <cmath>
#include <algorithm>

bool CollisionDetector::checkCollision(const Rectangle& a, const Rectangle& b) {
    return !(a.x + a.w <= b.x || 
             b.x + b.w <= a.x || 
             a.y + a.h <= b.y || 
             b.y + b.h <= a.y);
}

bool CollisionDetector::checkPointInRect(const Vector2& point, const Rectangle& rect) {
    return point.x >= rect.x && 
           point.x <= rect.x + rect.w && 
           point.y >= rect.y && 
           point.y <= rect.y + rect.h;
}

bool CollisionDetector::checkCircleRect(const Vector2& center, float radius, const Rectangle& rect) {
    Vector2 closest = closestPointOnRect(center, rect);
    return distanceSquared(center, closest) <= radius * radius;
}

CollisionInfo CollisionDetector::sweepAABB(const Rectangle& moving, const Vector2& velocity, const Rectangle& stationary) {
    CollisionInfo info;
    
    if (velocity.x == 0 && velocity.y == 0) {
        info.collided = checkCollision(moving, stationary);
        return info;
    }
    
    Rectangle expandedTarget;
    expandedTarget.x = stationary.x - moving.w;
    expandedTarget.y = stationary.y - moving.h;
    expandedTarget.w = stationary.w + moving.w;
    expandedTarget.h = stationary.h + moving.h;
    
    Vector2 rayStart = moving.center();
    
    float t;
    if (rayIntersectRect(rayStart, velocity, expandedTarget, t) && t >= 0 && t <= 1) {
        info.collided = true;
        
        Vector2 hitPoint = rayStart + velocity * t;
        
        float dx1 = hitPoint.x - stationary.x;
        float dx2 = (stationary.x + stationary.w) - hitPoint.x;
        float dy1 = hitPoint.y - stationary.y;
        float dy2 = (stationary.y + stationary.h) - hitPoint.y;
        
        float minDist = std::min({dx1, dx2, dy1, dy2});
        
        if (minDist == dx1) {
            info.normal = Vector2(-1, 0);
        } else if (minDist == dx2) {
            info.normal = Vector2(1, 0);
        } else if (minDist == dy1) {
            info.normal = Vector2(0, -1);
        } else {
            info.normal = Vector2(0, 1);
        }
        
        info.penetration = 1.0f - t;
    }
    
    return info;
}

bool CollisionDetector::rayIntersectRect(const Vector2& rayStart, const Vector2& rayDir, const Rectangle& rect, float& t) {
    if (rayDir.x == 0 && rayDir.y == 0) {
        return false;
    }
    
    float tMin = 0;
    float tMax = 1;
    
    if (rayDir.x != 0) {
        float t1 = (rect.x - rayStart.x) / rayDir.x;
        float t2 = (rect.x + rect.w - rayStart.x) / rayDir.x;
        
        if (t1 > t2) std::swap(t1, t2);
        
        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
        
        if (tMin > tMax) return false;
    } else {
        if (rayStart.x < rect.x || rayStart.x > rect.x + rect.w) {
            return false;
        }
    }
    
    if (rayDir.y != 0) {
        float t1 = (rect.y - rayStart.y) / rayDir.y;
        float t2 = (rect.y + rect.h - rayStart.y) / rayDir.y;
        
        if (t1 > t2) std::swap(t1, t2);
        
        tMin = std::max(tMin, t1);
        tMax = std::min(tMax, t2);
        
        if (tMin > tMax) return false;
    } else {
        if (rayStart.y < rect.y || rayStart.y > rect.y + rect.h) {
            return false;
        }
    }
    
    t = tMin;
    return true;
}

Vector2 CollisionDetector::closestPointOnRect(const Vector2& point, const Rectangle& rect) {
    return Vector2(
        std::max(rect.x, std::min(point.x, rect.x + rect.w)),
        std::max(rect.y, std::min(point.y, rect.y + rect.h))
    );
}

float CollisionDetector::distanceSquared(const Vector2& a, const Vector2& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx * dx + dy * dy;
}
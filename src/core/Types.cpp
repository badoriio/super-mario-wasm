#include "Types.h"
#include <cmath>

float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

Vector2 Vector2::normalized() const {
    float len = length();
    if (len < 1e-6f) return Vector2(0, 0);  // Use epsilon comparison instead of exact equality
    return Vector2(x / len, y / len);
}

bool Rectangle::intersects(const Rectangle& other) const {
    return !(x + w <= other.x || 
             other.x + other.w <= x || 
             y + h <= other.y || 
             other.y + other.h <= y);
}

bool Rectangle::contains(const Vector2& point) const {
    return point.x >= x && point.x <= x + w && 
           point.y >= y && point.y <= y + h;
}
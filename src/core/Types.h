#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}
    
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    
    float length() const;
    Vector2 normalized() const;
};

struct Rectangle {
    float x, y, w, h;
    
    Rectangle() : x(0), y(0), w(0), h(0) {}
    Rectangle(float x_, float y_, float w_, float h_) : x(x_), y(y_), w(w_), h(h_) {}
    
    bool intersects(const Rectangle& other) const;
    bool contains(const Vector2& point) const;
    Vector2 center() const { return Vector2(x + w/2, y + h/2); }
    
    SDL_Rect toSDL() const { return {(int)x, (int)y, (int)w, (int)h}; }
};

enum class EntityType {
    PLAYER,
    ENEMY_GOOMBA,
    ENEMY_KOOPA,
    ITEM_COIN,
    ITEM_MUSHROOM,
    ITEM_FIREFLOWER,
    BLOCK_QUESTION,
    BLOCK_BRICK,
    BLOCK_PIPE,
    PLATFORM_MOVING,
    FLAGPOLE
};

enum class PlayerState {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    HURT,
    DEAD,
    POWERED_UP
};

enum class EnemyState {
    PATROL,
    STUNNED,
    DEAD
};

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    VICTORY,
    SETTINGS
};

enum class Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct CollisionInfo {
    bool collided = false;
    Vector2 normal;
    float penetration = 0.0f;
    EntityType otherType;
};

using EntityID = uint32_t;
using TextureID = uint32_t;
using SoundID = uint32_t;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T, typename... Args>
Ptr<T> MakeShared(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}
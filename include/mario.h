#pragma once
#include <SDL.h>
#include "physics.h"

enum class MarioState {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING
};

class Mario {
public:
    Mario(float x, float y);
    ~Mario();
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void handleInput(const Uint8* keyState);
    
    void jump();
    void moveLeft();
    void moveRight();
    void stopMoving();
    
    // Getters
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    SDL_Rect getBounds() const;
    
    // Collision
    void onGround(bool grounded) { isGrounded = grounded; }
    bool isOnGround() const { return isGrounded; }
    
private:
    Vector2 position;
    Vector2 velocity;
    
    float width;
    float height;
    
    MarioState state;
    bool isGrounded;
    bool facingRight;
    
    // Physics constants
    static constexpr float GRAVITY = 800.0f;
    static constexpr float JUMP_FORCE = -400.0f;
    static constexpr float MOVE_SPEED = 200.0f;
    static constexpr float MAX_FALL_SPEED = 600.0f;
};
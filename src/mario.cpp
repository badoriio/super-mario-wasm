#include "mario.h"
#include <algorithm>

Mario::Mario(float x, float y) 
    : position(x, y), velocity(0, 0), width(32), height(32),
      state(MarioState::IDLE), isGrounded(false), facingRight(true) {
}

Mario::~Mario() {
}

void Mario::update(float deltaTime) {
    // Apply gravity if not grounded
    if (!isGrounded) {
        velocity.y += GRAVITY * deltaTime;
        velocity.y = std::min(velocity.y, MAX_FALL_SPEED);
        state = velocity.y > 0 ? MarioState::FALLING : MarioState::JUMPING;
    } else {
        velocity.y = 0;
        if (std::abs(velocity.x) < 0.1f) {
            state = MarioState::IDLE;
        } else {
            state = MarioState::RUNNING;
        }
    }
    
    // Apply friction to horizontal movement
    if (isGrounded) {
        velocity.x *= 0.8f;
    }
    
    // Update position
    position += velocity * deltaTime;
    
    // Keep Mario within screen bounds
    position.x = std::max(0.0f, std::min(position.x, 800.0f - width));
}

void Mario::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(width),
        static_cast<int>(height)
    };
    
    // Render Mario with different colors based on state
    switch (state) {
        case MarioState::IDLE:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
            break;
        case MarioState::RUNNING:
            SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255); // Light red
            break;
        case MarioState::JUMPING:
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
            break;
        case MarioState::FALLING:
            SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255); // Orange
            break;
    }
    
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw eyes to show direction
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int eyeSize = 4;
    int eyeY = static_cast<int>(position.y + 8);
    
    if (facingRight) {
        SDL_Rect leftEye = {static_cast<int>(position.x + 8), eyeY, eyeSize, eyeSize};
        SDL_Rect rightEye = {static_cast<int>(position.x + 20), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &leftEye);
        SDL_RenderFillRect(renderer, &rightEye);
    } else {
        SDL_Rect leftEye = {static_cast<int>(position.x + 8), eyeY, eyeSize, eyeSize};
        SDL_Rect rightEye = {static_cast<int>(position.x + 20), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &leftEye);
        SDL_RenderFillRect(renderer, &rightEye);
    }
}

void Mario::handleInput(const Uint8* keyState) {
    bool moving = false;
    
    if (keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A]) {
        moveLeft();
        moving = true;
    }
    
    if (keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D]) {
        moveRight();
        moving = true;
    }
    
    if (keyState[SDL_SCANCODE_SPACE] || keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W]) {
        jump();
    }
    
    if (!moving && isGrounded) {
        stopMoving();
    }
}

void Mario::jump() {
    if (isGrounded) {
        velocity.y = JUMP_FORCE;
        isGrounded = false;
        state = MarioState::JUMPING;
    }
}

void Mario::moveLeft() {
    velocity.x = -MOVE_SPEED;
    facingRight = false;
}

void Mario::moveRight() {
    velocity.x = MOVE_SPEED;
    facingRight = true;
}

void Mario::stopMoving() {
    if (isGrounded) {
        velocity.x *= 0.5f; // Quick deceleration
    }
}

SDL_Rect Mario::getBounds() const {
    return {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(width),
        static_cast<int>(height)
    };
}
#pragma once

#include "../core/Types.h"
#include "../engine/Animation.h"
#include "../engine/SpriteSheet.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <unordered_map>
#include <string>

class PhysicsWorld;
class PhysicsBody;
class InputManager;
class Renderer;
class AudioManager;

class Player {
public:
    Player(const Vector2& position);
    ~Player();
    
    void initialize(PhysicsWorld* physics, InputManager* input, AudioManager* audio);
    void update(float deltaTime);
    void render(Renderer* renderer);
    
    Vector2 getPosition() const;
    void setPosition(const Vector2& position);
    Rectangle getBounds() const;
    
    PlayerState getState() const { return m_state; }
    void setState(PlayerState newState);
    
    void takeDamage();
    void powerUp();
    void addScore(int points) { m_score += points; }
    void addCoins(int coins) { m_coins += coins; }
    void addLife() { m_lives++; }
    
    int getScore() const { return m_score; }
    int getCoins() const { return m_coins; }
    int getLives() const { return m_lives; }
    
    bool isAlive() const { return m_state != PlayerState::DEAD; }
    bool isInvulnerable() const { return m_invulnerabilityTimer > 0.0f; }
    
    void reset();
    void respawn(const Vector2& position);

private:
    void handleInput(float deltaTime);
    void updatePhysics(float deltaTime);
    void updateAnimation(float deltaTime);
    void updateTimers(float deltaTime);
    void checkCollisions();
    
    void jump();
    void move(float direction);
    void stopMoving();
    
    void setupAnimations();
    void playAnimation(const std::string& name);
    
    PhysicsWorld* m_physics;
    PhysicsBody* m_body;
    InputManager* m_input;
    AudioManager* m_audio;
    
    PlayerState m_state;
    PlayerState m_previousState;
    
    SpriteSheet m_spriteSheet;
    std::unordered_map<std::string, Animation> m_animations;
    Animation m_currentAnimation;
    std::string m_currentAnimationName;
    
    Direction m_facing;
    bool m_isGrounded;
    bool m_wasGrounded;
    
    float m_coyoteTimer;
    float m_jumpBufferTimer;
    float m_invulnerabilityTimer;
    
    int m_score;
    int m_coins;
    int m_lives;
    
    bool m_isPoweredUp;
    
    static constexpr float PLAYER_WIDTH = 24.0f;
    static constexpr float PLAYER_HEIGHT = 32.0f;
};
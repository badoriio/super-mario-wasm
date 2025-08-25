#include "Player.h"
#include "../physics/PhysicsWorld.h"
#include "../core/InputManager.h"
#include "../core/AudioManager.h"
#include "../core/ResourceManager.h"
#include "../engine/Renderer.h"
#include "../core/Constants.h"
#include <iostream>

Player::Player(const Vector2& position)
    : m_physics(nullptr)
    , m_body(nullptr)
    , m_input(nullptr)
    , m_audio(nullptr)
    , m_state(PlayerState::IDLE)
    , m_previousState(PlayerState::IDLE)
    , m_facing(Direction::RIGHT)
    , m_isGrounded(false)
    , m_wasGrounded(false)
    , m_coyoteTimer(0.0f)
    , m_jumpBufferTimer(0.0f)
    , m_invulnerabilityTimer(0.0f)
    , m_score(0)
    , m_coins(0)
    , m_lives(Constants::MAX_LIVES)
    , m_isPoweredUp(false)
{
    setupAnimations();
}

Player::~Player() {
    if (m_physics && m_body) {
        m_physics->removeBody(m_body->id);
    }
}

void Player::initialize(PhysicsWorld* physics, InputManager* input, AudioManager* audio) {
    m_physics = physics;
    m_input = input;
    m_audio = audio;
    
    if (m_physics) {
        m_body = m_physics->createBody(1, getPosition(), Vector2(PLAYER_WIDTH, PLAYER_HEIGHT));
        if (m_body) {
            m_body->mass = 1.0f;
            m_body->friction = Constants::GROUND_FRICTION;
            // Initialize with zero velocity to prevent initial falling
            m_body->velocity = Vector2(0, 0);
            m_body->acceleration = Vector2(0, 0);
        }
    }
    
    ResourceManager* resources = ResourceManager::getInstance();
    if (resources) {
        SDL_Texture* texture = resources->getTexture("sprites/mario_small.png");
        m_spriteSheet.setTexture(texture);
        m_spriteSheet.setSpriteSize(16, 16);
    }
}

void Player::update(float deltaTime) {
    updateTimers(deltaTime);
    handleInput(deltaTime);
    updatePhysics(deltaTime);
    checkCollisions();
    updateAnimation(deltaTime);
}

void Player::render(Renderer* renderer) {
    if (!renderer || (m_invulnerabilityTimer > 0.0f && static_cast<int>(m_invulnerabilityTimer * 10) % 2)) {
        return;
    }
    
    SDL_Texture* texture = m_spriteSheet.getTexture();
    if (!texture) return;
    
    SDL_Rect srcRect = m_currentAnimation.getCurrentFrame();
    Vector2 position = getPosition();
    
    SDL_RendererFlip flip = (m_facing == Direction::LEFT) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    
    SDL_Rect dstRect = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT)
    };
    
    renderer->drawTexture(texture, srcRect, dstRect, 0.0f, nullptr, flip, 10);
}

Vector2 Player::getPosition() const {
    return m_body ? m_body->position : Vector2(0, 0);
}

void Player::setPosition(const Vector2& position) {
    if (m_body) {
        m_body->position = position;
        m_body->updateBounds();
    }
}

Rectangle Player::getBounds() const {
    return m_body ? m_body->bounds : Rectangle(0, 0, PLAYER_WIDTH, PLAYER_HEIGHT);
}

void Player::setState(PlayerState newState) {
    if (m_state != newState) {
        m_previousState = m_state;
        m_state = newState;
        
        switch (m_state) {
            case PlayerState::IDLE:
                playAnimation("idle");
                break;
            case PlayerState::RUNNING:
                playAnimation("run");
                break;
            case PlayerState::JUMPING:
                playAnimation("jump");
                break;
            case PlayerState::FALLING:
                playAnimation("fall");
                break;
            case PlayerState::HURT:
                playAnimation("hurt");
                if (m_audio) m_audio->playSound("hurt");
                break;
            case PlayerState::DEAD:
                playAnimation("dead");
                if (m_audio) m_audio->playSound("die");
                break;
            case PlayerState::POWERED_UP:
                break;
        }
    }
}

void Player::takeDamage() {
    if (m_invulnerabilityTimer > 0.0f) return;
    
    if (m_isPoweredUp) {
        m_isPoweredUp = false;
        m_invulnerabilityTimer = Constants::INVINCIBILITY_TIME;
        setState(PlayerState::HURT);
    } else {
        m_lives--;
        if (m_lives <= 0) {
            setState(PlayerState::DEAD);
        } else {
            m_invulnerabilityTimer = Constants::INVINCIBILITY_TIME;
            setState(PlayerState::HURT);
        }
    }
}

void Player::powerUp() {
    if (!m_isPoweredUp) {
        m_isPoweredUp = true;
        setState(PlayerState::POWERED_UP);
        if (m_audio) m_audio->playSound("powerup");
    }
}

void Player::reset() {
    m_score = 0;
    m_coins = 0;
    m_lives = Constants::MAX_LIVES;
    m_isPoweredUp = false;
    m_invulnerabilityTimer = 0.0f;
    setState(PlayerState::IDLE);
    
    if (m_body) {
        m_body->velocity = Vector2(0, 0);
        m_body->acceleration = Vector2(0, 0);
    }
}

void Player::respawn(const Vector2& position) {
    setPosition(position);
    m_invulnerabilityTimer = Constants::INVINCIBILITY_TIME;
    setState(PlayerState::IDLE);
    
    if (m_body) {
        m_body->velocity = Vector2(0, 0);
        m_body->acceleration = Vector2(0, 0);
    }
}

void Player::handleInput(float /* deltaTime */) {
    if (!m_input || m_state == PlayerState::DEAD) return;
    
    bool moveLeft = m_input->isActionPressed(InputAction::MOVE_LEFT);
    bool moveRight = m_input->isActionPressed(InputAction::MOVE_RIGHT);
    bool jumpPressed = m_input->isActionJustPressed(InputAction::JUMP);
    // bool runPressed = m_input->isActionPressed(InputAction::RUN);
    
    // Remove excessive debug output
    
    // Only allow jumping when grounded and jump key is pressed
    if (jumpPressed && m_isGrounded) {
        printf("=== JUMPING! Player is grounded ===\n");
        jump();
    }
    
    if (moveLeft && !moveRight) {
        move(-1.0f);
        m_facing = Direction::LEFT;
    } else if (moveRight && !moveLeft) {
        move(1.0f);
        m_facing = Direction::RIGHT;
    } else {
        stopMoving();
    }
    
    if (!m_input->isActionPressed(InputAction::JUMP) && m_body && m_body->velocity.y < 0) {
        m_body->velocity.y *= 0.5f;
    }
}

void Player::updatePhysics(float deltaTime) {
    if (!m_body) return;
    
    m_wasGrounded = m_isGrounded;
    m_isGrounded = m_body->isGrounded;
    
    if (!m_wasGrounded && m_isGrounded) {
        printf("=== PLAYER LANDED! ===\n");
        if (m_audio) m_audio->playSound("land");
    }
    
    if (m_wasGrounded && !m_isGrounded && m_body->velocity.y > 5.0f) {
        printf("=== PLAYER LEFT GROUND! Coyote time activated ===\n");
        m_coyoteTimer = Constants::COYOTE_TIME;
    }
    
    PlayerState newState = m_state;
    
    if (m_isGrounded) {
        if (std::abs(m_body->velocity.x) > 10.0f) {
            newState = PlayerState::RUNNING;
        } else {
            newState = PlayerState::IDLE;
        }
    } else {
        if (m_body->velocity.y < 0) {
            newState = PlayerState::JUMPING;
        } else {
            newState = PlayerState::FALLING;
        }
    }
    
    if (m_state != PlayerState::HURT && m_state != PlayerState::DEAD) {
        setState(newState);
    }
}

void Player::updateAnimation(float deltaTime) {
    m_currentAnimation.update(deltaTime);
}

void Player::updateTimers(float deltaTime) {
    if (m_coyoteTimer > 0.0f) {
        m_coyoteTimer -= deltaTime;
    }
    
    if (m_jumpBufferTimer > 0.0f) {
        m_jumpBufferTimer -= deltaTime;
    }
    
    if (m_invulnerabilityTimer > 0.0f) {
        m_invulnerabilityTimer -= deltaTime;
        if (m_invulnerabilityTimer <= 0.0f && m_state == PlayerState::HURT) {
            setState(PlayerState::IDLE);
        }
    }
}

void Player::checkCollisions() {
    if (!m_physics) return;
    
    std::vector<Collision> collisions = m_physics->getCollisions(m_body->id);
    for (const auto& collision : collisions) {
    }
}

void Player::jump() {
    if (m_body) {
        m_body->velocity.y = -Constants::PLAYER_JUMP_SPEED;
        if (m_audio) m_audio->playSound("jump");
    }
}

void Player::move(float direction) {
    if (m_body) {
        float acceleration = Constants::PLAYER_ACCELERATION;
        float oldVelX = m_body->velocity.x;
        m_body->velocity.x += direction * acceleration * 0.016f;
        
        float maxSpeed = Constants::PLAYER_MAX_SPEED;
        if (m_body->velocity.x > maxSpeed) m_body->velocity.x = maxSpeed;
        if (m_body->velocity.x < -maxSpeed) m_body->velocity.x = -maxSpeed;
        
        // Movement debug removed
    }
}

void Player::stopMoving() {
    if (m_body && m_isGrounded) {
        m_body->velocity.x *= Constants::GROUND_FRICTION;
    }
}

void Player::setupAnimations() {
    std::vector<AnimationFrame> idleFrames;
    idleFrames.emplace_back(SDL_Rect{0, 0, 16, 16}, Constants::PLAYER_IDLE_FRAME_TIME);
    
    std::vector<AnimationFrame> runFrames;
    runFrames.emplace_back(SDL_Rect{16, 0, 16, 16}, Constants::PLAYER_RUN_FRAME_TIME);
    runFrames.emplace_back(SDL_Rect{32, 0, 16, 16}, Constants::PLAYER_RUN_FRAME_TIME);
    runFrames.emplace_back(SDL_Rect{48, 0, 16, 16}, Constants::PLAYER_RUN_FRAME_TIME);
    
    std::vector<AnimationFrame> jumpFrames;
    jumpFrames.emplace_back(SDL_Rect{64, 0, 16, 16}, 1.0f);
    
    std::vector<AnimationFrame> fallFrames;
    fallFrames.emplace_back(SDL_Rect{80, 0, 16, 16}, 1.0f);
    
    std::vector<AnimationFrame> hurtFrames;
    hurtFrames.emplace_back(SDL_Rect{96, 0, 16, 16}, 0.5f);
    
    std::vector<AnimationFrame> deadFrames;
    deadFrames.emplace_back(SDL_Rect{112, 0, 16, 16}, 1.0f);
    
    m_animations["idle"] = Animation(idleFrames, true);
    m_animations["run"] = Animation(runFrames, true);
    m_animations["jump"] = Animation(jumpFrames, false);
    m_animations["fall"] = Animation(fallFrames, false);
    m_animations["hurt"] = Animation(hurtFrames, false);
    m_animations["dead"] = Animation(deadFrames, false);
    
    playAnimation("idle");
}

void Player::playAnimation(const std::string& name) {
    if (m_currentAnimationName != name) {
        auto it = m_animations.find(name);
        if (it != m_animations.end()) {
            m_currentAnimation = it->second;
            m_currentAnimationName = name;
            m_currentAnimation.reset();
            m_currentAnimation.play();
        }
    }
}
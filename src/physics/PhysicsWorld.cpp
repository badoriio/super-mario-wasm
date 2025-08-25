#include "PhysicsWorld.h"
#include "CollisionDetector.h"
#include "../game/Level.h"
#include "../core/Constants.h"
#include <algorithm>
#include <iostream>

PhysicsWorld::PhysicsWorld()
    : m_gravity(0, Constants::GRAVITY)
    , m_level(nullptr)
{
}

PhysicsWorld::~PhysicsWorld() {
    m_bodies.clear();
}

void PhysicsWorld::update(float deltaTime) {
    m_collisions.clear();
    
    for (auto& body : m_bodies) {
        if (!body->isStatic) {
            checkLevelCollisions(body.get());      // Check ground FIRST
            integrateVelocity(body.get(), deltaTime);  // Then apply forces based on ground state
            integratePosition(body.get(), deltaTime);  // Finally apply velocity to position
        }
    }
    
    resolveCollisions();
}

PhysicsBody* PhysicsWorld::createBody(EntityID id, const Vector2& position, const Vector2& size) {
    // Check for existing body with same ID
    for (const auto& existingBody : m_bodies) {
        if (existingBody->id == id) {
            std::cerr << "Warning: Attempting to create physics body with duplicate ID " << id << std::endl;
            return nullptr;
        }
    }
    
    auto body = std::make_unique<PhysicsBody>(id, position, size);
    PhysicsBody* bodyPtr = body.get();
    m_bodies.push_back(std::move(body));
    return bodyPtr;
}

void PhysicsWorld::removeBody(EntityID id) {
    m_bodies.erase(
        std::remove_if(m_bodies.begin(), m_bodies.end(),
                      [id](const std::unique_ptr<PhysicsBody>& body) {
                          return body->id == id;
                      }),
        m_bodies.end());
}

PhysicsBody* PhysicsWorld::getBody(EntityID id) {
    for (auto& body : m_bodies) {
        if (body->id == id) {
            return body.get();
        }
    }
    return nullptr;
}

std::vector<Collision> PhysicsWorld::getCollisions(EntityID id) const {
    std::vector<Collision> result;
    for (const auto& collision : m_collisions) {
        if (collision.bodyA->id == id || collision.bodyB->id == id) {
            result.push_back(collision);
        }
    }
    return result;
}

bool PhysicsWorld::raycast(const Vector2& start, const Vector2& end, EntityID ignoreId, Vector2* hitPoint) {
    return false;
}

void PhysicsWorld::integrateVelocity(PhysicsBody* body, float deltaTime) {
    // Only apply gravity if not grounded
    if (!body->isGrounded) {
        body->acceleration += m_gravity;
    } else {
        // When grounded, stop vertical velocity to prevent bouncing
        if (body->velocity.y > 0) {
            body->velocity.y = 0;
        }
    }
    
    body->velocity += body->acceleration * deltaTime;
    
    if (body->velocity.y > Constants::MAX_FALL_SPEED) {
        body->velocity.y = Constants::MAX_FALL_SPEED;
    }
    
    // Let individual entities handle their own friction/resistance
    // Only apply air resistance when not grounded to allow some air control
    if (!body->isGrounded) {
        body->velocity.x *= Constants::AIR_RESISTANCE;
    }
    
    body->acceleration = Vector2(0, 0);
}

void PhysicsWorld::integratePosition(PhysicsBody* body, float deltaTime) {
    Vector2 oldPos = body->position;
    body->position += body->velocity * deltaTime;
    body->updateBounds();
    
    // Debug position integration once per second
    static int debugCount = 0;
    debugCount++;
    if (debugCount % 60 == 0 && (body->velocity.x != 0 || body->velocity.y != 0)) {
        printf("=== POSITION UPDATE: vel=(%.1f,%.1f), oldPos=(%.1f,%.1f), newPos=(%.1f,%.1f) ===\n",
               body->velocity.x, body->velocity.y, oldPos.x, oldPos.y, body->position.x, body->position.y);
    }
}

void PhysicsWorld::resolveCollisions() {
    for (size_t i = 0; i < m_bodies.size(); ++i) {
        for (size_t j = i + 1; j < m_bodies.size(); ++j) {
            PhysicsBody* bodyA = m_bodies[i].get();
            PhysicsBody* bodyB = m_bodies[j].get();
            
            if (!bodyA->isSolid || !bodyB->isSolid) continue;
            if (bodyA->isStatic && bodyB->isStatic) continue;
            
            if (CollisionDetector::checkCollision(bodyA->bounds, bodyB->bounds)) {
                Collision collision;
                collision.bodyA = bodyA;
                collision.bodyB = bodyB;
                
                Vector2 centerA = bodyA->bounds.center();
                Vector2 centerB = bodyB->bounds.center();
                Vector2 delta = centerB - centerA;
                
                float overlapX = (bodyA->bounds.w + bodyB->bounds.w) / 2 - std::abs(delta.x);
                float overlapY = (bodyA->bounds.h + bodyB->bounds.h) / 2 - std::abs(delta.y);
                
                if (overlapX < overlapY) {
                    collision.normal = Vector2(delta.x > 0 ? 1.0f : -1.0f, 0);
                    collision.penetration = overlapX;
                } else {
                    collision.normal = Vector2(0, delta.y > 0 ? 1.0f : -1.0f);
                    collision.penetration = overlapY;
                }
                
                collision.contactPoint = centerA + delta * 0.5f;
                m_collisions.push_back(collision);
                
                if (bodyA->isStatic) {
                    bodyB->position -= collision.normal * collision.penetration;
                    bodyB->updateBounds();
                } else if (bodyB->isStatic) {
                    bodyA->position += collision.normal * collision.penetration;
                    bodyA->updateBounds();
                } else {
                    Vector2 correction = collision.normal * (collision.penetration * 0.5f);
                    bodyA->position += correction;
                    bodyB->position -= correction;
                    bodyA->updateBounds();
                    bodyB->updateBounds();
                }
                
                if (collision.normal.y < -0.5f) {
                    if (!bodyA->isStatic) bodyA->isGrounded = true;
                    if (!bodyB->isStatic && collision.normal.y > 0.5f) bodyB->isGrounded = true;
                }
            }
        }
    }
}

void PhysicsWorld::checkLevelCollisions(PhysicsBody* body) {
    if (!m_level) return;
    
    // Check level boundaries first
    // Left boundary - prevent going off the left edge
    if (body->position.x < 0) {
        body->position.x = 0;
        body->velocity.x = 0;
        body->updateBounds();
    }
    
    // Right boundary - prevent going beyond level width
    float levelWidth = m_level->getWidth() * Constants::TILE_SIZE;
    if (body->position.x + body->bounds.w > levelWidth) {
        body->position.x = levelWidth - body->bounds.w;
        body->velocity.x = 0;
        body->updateBounds();
    }
    
    // Simple ground detection - check if there's solid ground directly below the player
    body->isGrounded = false;
    
    Rectangle groundCheckBounds = body->bounds;
    groundCheckBounds.y = body->position.y + body->bounds.h; // Check at bottom edge of player
    groundCheckBounds.h = 2.0f; // Check 2 pixels down
    
    if (m_level->checkCollision(groundCheckBounds)) {
        body->isGrounded = true;
        // If we're grounded, make sure we're not sinking into the ground
        int tileY = static_cast<int>((body->position.y + body->bounds.h) / Constants::TILE_SIZE);
        float groundY = tileY * Constants::TILE_SIZE - body->bounds.h;
        if (body->position.y > groundY) {
            body->position.y = groundY;
            body->updateBounds();
        }
        // Clear any downward velocity when on ground
        if (body->velocity.y > 0) {
            body->velocity.y = 0;
        }
    }
}
#pragma once

#include "../core/Types.h"
#include <vector>
#include <memory>

struct PhysicsBody {
    EntityID id;
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Rectangle bounds;
    
    bool isStatic = false;
    bool isGrounded = false;
    bool isSolid = true;
    
    float mass = 1.0f;
    float friction = 0.0f;
    float restitution = 0.0f;
    
    PhysicsBody(EntityID entityId, const Vector2& pos, const Vector2& size)
        : id(entityId), position(pos), velocity(0, 0), acceleration(0, 0)
        , bounds(pos.x, pos.y, size.x, size.y) {}
    
    void updateBounds() {
        bounds.x = position.x;
        bounds.y = position.y;
    }
};

struct Collision {
    PhysicsBody* bodyA;
    PhysicsBody* bodyB;
    Vector2 normal;
    float penetration;
    Vector2 contactPoint;
};

class Level;

class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();
    
    void update(float deltaTime);
    void setGravity(const Vector2& gravity) { m_gravity = gravity; }
    Vector2 getGravity() const { return m_gravity; }
    
    PhysicsBody* createBody(EntityID id, const Vector2& position, const Vector2& size);
    void removeBody(EntityID id);
    PhysicsBody* getBody(EntityID id);
    
    void setLevel(Level* level) { m_level = level; }
    Level* getLevel() const { return m_level; }
    
    std::vector<Collision> getCollisions(EntityID id) const;
    
    bool raycast(const Vector2& start, const Vector2& end, EntityID ignoreId = 0, Vector2* hitPoint = nullptr);

private:
    void integrateVelocity(PhysicsBody* body, float deltaTime);
    void integratePosition(PhysicsBody* body, float deltaTime);
    void resolveCollisions();
    void checkLevelCollisions(PhysicsBody* body);
    
    std::vector<std::unique_ptr<PhysicsBody>> m_bodies;
    std::vector<Collision> m_collisions;
    Vector2 m_gravity;
    Level* m_level;
};
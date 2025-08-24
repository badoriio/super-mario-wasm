#pragma once

#include "../core/Types.h"

class Camera {
public:
    Camera();
    Camera(const Vector2& position, const Vector2& size);
    
    void update(float deltaTime);
    
    void setPosition(const Vector2& position) { m_position = position; }
    Vector2 getPosition() const { return m_position; }
    
    void setTarget(const Vector2& target);
    Vector2 getTarget() const { return m_target; }
    
    void setSize(const Vector2& size) { m_size = size; }
    Vector2 getSize() const { return m_size; }
    
    void setDeadZone(const Vector2& deadZone) { m_deadZone = deadZone; }
    Vector2 getDeadZone() const { return m_deadZone; }
    
    void setFollowSpeed(float speed) { m_followSpeed = speed; }
    float getFollowSpeed() const { return m_followSpeed; }
    
    void setLookAhead(float lookAhead) { m_lookAhead = lookAhead; }
    float getLookAhead() const { return m_lookAhead; }
    
    void setBounds(const Rectangle& bounds) { m_bounds = bounds; m_hasBounds = true; }
    void clearBounds() { m_hasBounds = false; }
    
    bool isVisible(const Rectangle& rect) const;
    
    void shake(float intensity, float duration);

private:
    Vector2 m_position;
    Vector2 m_target;
    Vector2 m_size;
    Vector2 m_deadZone;
    
    float m_followSpeed;
    float m_lookAhead;
    
    Rectangle m_bounds;
    bool m_hasBounds;
    
    // Screen shake
    float m_shakeIntensity;
    float m_shakeDuration;
    Vector2 m_shakeOffset;
    
    Vector2 m_lastTargetPosition;
    Vector2 m_targetVelocity;
    
    void updateShake(float deltaTime);
    void clampToBounds();
};
#include "Camera.h"
#include "../core/Constants.h"
#include <algorithm>
#include <cmath>
#include <random>

Camera::Camera()
    : m_position(0, 0)
    , m_target(0, 0)
    , m_size(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT)
    , m_deadZone(Constants::CAMERA_DEADZONE_X, Constants::CAMERA_DEADZONE_Y)
    , m_followSpeed(Constants::CAMERA_FOLLOW_SPEED)
    , m_lookAhead(Constants::CAMERA_LOOK_AHEAD)
    , m_hasBounds(false)
    , m_shakeIntensity(0.0f)
    , m_shakeDuration(0.0f)
    , m_shakeOffset(0, 0)
    , m_lastTargetPosition(0, 0)
    , m_targetVelocity(0, 0)
{
}

Camera::Camera(const Vector2& position, const Vector2& size)
    : m_position(position)
    , m_target(position)
    , m_size(size)
    , m_deadZone(Constants::CAMERA_DEADZONE_X, Constants::CAMERA_DEADZONE_Y)
    , m_followSpeed(Constants::CAMERA_FOLLOW_SPEED)
    , m_lookAhead(Constants::CAMERA_LOOK_AHEAD)
    , m_hasBounds(false)
    , m_shakeIntensity(0.0f)
    , m_shakeDuration(0.0f)
    , m_shakeOffset(0, 0)
    , m_lastTargetPosition(position)
    , m_targetVelocity(0, 0)
{
}

void Camera::update(float deltaTime) {
    // Simple camera following - center on target with discrete positioning
    Vector2 idealPosition = m_target;
    idealPosition.x -= m_size.x / 2;
    idealPosition.y -= m_size.y / 2;
    
    // Use direct positioning instead of smooth interpolation to avoid seam artifacts
    m_position = idealPosition;
    
    updateShake(deltaTime);
    clampToBounds();
    
    // Snap to pixel-perfect positions to prevent visual seams
    m_position.x = std::round(m_position.x);
    m_position.y = std::round(m_position.y);
}

void Camera::setTarget(const Vector2& target) {
    m_target = target;
}

bool Camera::isVisible(const Rectangle& rect) const {
    Rectangle cameraRect(m_position.x, m_position.y, m_size.x, m_size.y);
    return cameraRect.intersects(rect);
}

void Camera::shake(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
}

void Camera::updateShake(float deltaTime) {
    if (m_shakeDuration > 0.0f) {
        m_shakeDuration -= deltaTime;
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        m_shakeOffset.x = dis(gen) * m_shakeIntensity;
        m_shakeOffset.y = dis(gen) * m_shakeIntensity;
        
        m_position += m_shakeOffset;
        
        if (m_shakeDuration <= 0.0f) {
            m_shakeOffset = Vector2(0, 0);
            m_shakeIntensity = 0.0f;
        }
    }
}

void Camera::clampToBounds() {
    if (!m_hasBounds) return;
    
    // Clamp camera to level boundaries - prevent showing empty space beyond level edges
    m_position.x = std::max(m_bounds.x, std::min(m_position.x, m_bounds.x + m_bounds.w - m_size.x));
    m_position.y = std::max(m_bounds.y, std::min(m_position.y, m_bounds.y + m_bounds.h - m_size.y));
}
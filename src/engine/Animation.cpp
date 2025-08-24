#include "Animation.h"

Animation::Animation()
    : m_currentFrame(0)
    , m_frameTime(0.0f)
    , m_loop(true)
    , m_finished(false)
    , m_playing(true)
    , m_speed(1.0f)
{
}

Animation::Animation(const std::vector<AnimationFrame>& frames, bool loop)
    : m_frames(frames)
    , m_currentFrame(0)
    , m_frameTime(0.0f)
    , m_loop(loop)
    , m_finished(false)
    , m_playing(true)
    , m_speed(1.0f)
{
}

void Animation::addFrame(const SDL_Rect& srcRect, float duration) {
    m_frames.emplace_back(srcRect, duration);
}

void Animation::setFrames(const std::vector<AnimationFrame>& frames) {
    m_frames = frames;
    reset();
}

void Animation::update(float deltaTime) {
    if (!m_playing || m_frames.empty() || m_finished) {
        return;
    }
    
    m_frameTime += deltaTime * m_speed;
    
    while (m_frameTime >= m_frames[m_currentFrame].duration) {
        m_frameTime -= m_frames[m_currentFrame].duration;
        m_currentFrame++;
        
        if (m_currentFrame >= static_cast<int>(m_frames.size())) {
            if (m_loop) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = static_cast<int>(m_frames.size()) - 1;
                m_finished = true;
                m_playing = false;
                break;
            }
        }
    }
}

void Animation::reset() {
    m_currentFrame = 0;
    m_frameTime = 0.0f;
    m_finished = false;
}

SDL_Rect Animation::getCurrentFrame() const {
    if (m_frames.empty()) {
        return {0, 0, 0, 0};
    }
    
    return m_frames[m_currentFrame].srcRect;
}
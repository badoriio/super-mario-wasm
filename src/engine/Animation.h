#pragma once

#include "../core/Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <vector>

struct AnimationFrame {
    SDL_Rect srcRect;
    float duration;
    
    AnimationFrame() : srcRect{0, 0, 0, 0}, duration(0.0f) {}
    AnimationFrame(const SDL_Rect& rect, float dur) : srcRect(rect), duration(dur) {}
};

class Animation {
public:
    Animation();
    Animation(const std::vector<AnimationFrame>& frames, bool loop = true);
    
    void addFrame(const SDL_Rect& srcRect, float duration);
    void setFrames(const std::vector<AnimationFrame>& frames);
    
    void update(float deltaTime);
    void reset();
    
    SDL_Rect getCurrentFrame() const;
    int getCurrentFrameIndex() const { return m_currentFrame; }
    
    void setLoop(bool loop) { m_loop = loop; }
    bool isLooping() const { return m_loop; }
    
    bool isFinished() const { return m_finished; }
    bool isPlaying() const { return m_playing; }
    
    void play() { m_playing = true; }
    void pause() { m_playing = false; }
    void stop() { m_playing = false; reset(); }
    
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    
    size_t getFrameCount() const { return m_frames.size(); }

private:
    std::vector<AnimationFrame> m_frames;
    int m_currentFrame;
    float m_frameTime;
    bool m_loop;
    bool m_finished;
    bool m_playing;
    float m_speed;
};
#include "SpriteSheet.h"
#include <iostream>

SpriteSheet::SpriteSheet()
    : m_texture(nullptr)
    , m_spriteWidth(32)
    , m_spriteHeight(32)
{
}

SpriteSheet::SpriteSheet(SDL_Texture* texture, int spriteWidth, int spriteHeight)
    : m_texture(texture)
    , m_spriteWidth(spriteWidth)
    , m_spriteHeight(spriteHeight)
{
}

SDL_Rect SpriteSheet::getSpriteRect(int index) const {
    if (!m_texture || index < 0) {
        return {0, 0, 0, 0};
    }
    
    int columns = getColumns();
    if (columns <= 0) {
        return {0, 0, 0, 0};
    }
    
    int col = index % columns;
    int row = index / columns;
    
    return getSpriteRect(col, row);
}

SDL_Rect SpriteSheet::getSpriteRect(int col, int row) const {
    return {
        col * m_spriteWidth,
        row * m_spriteHeight,
        m_spriteWidth,
        m_spriteHeight
    };
}

void SpriteSheet::addAnimation(const std::string& name, const std::vector<int>& frameIndices, float frameDuration, bool loop) {
    std::vector<AnimationFrame> frames;
    frames.reserve(frameIndices.size());
    
    for (int frameIndex : frameIndices) {
        SDL_Rect srcRect = getSpriteRect(frameIndex);
        frames.emplace_back(srcRect, frameDuration);
    }
    
    m_animations[name] = frames;
}

void SpriteSheet::addAnimation(const std::string& name, int startFrame, int endFrame, float frameDuration, bool loop) {
    std::vector<int> frameIndices;
    for (int i = startFrame; i <= endFrame; ++i) {
        frameIndices.push_back(i);
    }
    
    addAnimation(name, frameIndices, frameDuration, loop);
}

Animation SpriteSheet::getAnimation(const std::string& name) const {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        return Animation(it->second, true);
    }
    
    return Animation();
}

bool SpriteSheet::hasAnimation(const std::string& name) const {
    return m_animations.find(name) != m_animations.end();
}

int SpriteSheet::getColumns() const {
    if (!m_texture || m_spriteWidth <= 0) {
        return 0;
    }
    
    int textureWidth;
    SDL_QueryTexture(m_texture, nullptr, nullptr, &textureWidth, nullptr);
    return textureWidth / m_spriteWidth;
}

int SpriteSheet::getRows() const {
    if (!m_texture || m_spriteHeight <= 0) {
        return 0;
    }
    
    int textureHeight;
    SDL_QueryTexture(m_texture, nullptr, nullptr, nullptr, &textureHeight);
    return textureHeight / m_spriteHeight;
}

int SpriteSheet::getTotalSprites() const {
    return getColumns() * getRows();
}
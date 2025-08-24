#pragma once

#include "../core/Types.h"
#include "Animation.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <unordered_map>
#include <string>

class SpriteSheet {
public:
    SpriteSheet();
    SpriteSheet(SDL_Texture* texture, int spriteWidth, int spriteHeight);
    
    void setTexture(SDL_Texture* texture) { m_texture = texture; }
    SDL_Texture* getTexture() const { return m_texture; }
    
    void setSpriteSize(int width, int height) { m_spriteWidth = width; m_spriteHeight = height; }
    Vector2 getSpriteSize() const { return Vector2(static_cast<float>(m_spriteWidth), static_cast<float>(m_spriteHeight)); }
    
    SDL_Rect getSpriteRect(int index) const;
    SDL_Rect getSpriteRect(int col, int row) const;
    
    void addAnimation(const std::string& name, const std::vector<int>& frameIndices, float frameDuration, bool loop = true);
    void addAnimation(const std::string& name, int startFrame, int endFrame, float frameDuration, bool loop = true);
    
    Animation getAnimation(const std::string& name) const;
    bool hasAnimation(const std::string& name) const;
    
    int getColumns() const;
    int getRows() const;
    int getTotalSprites() const;

private:
    SDL_Texture* m_texture;
    int m_spriteWidth;
    int m_spriteHeight;
    
    std::unordered_map<std::string, std::vector<AnimationFrame>> m_animations;
};
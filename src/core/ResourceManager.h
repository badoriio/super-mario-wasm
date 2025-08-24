#pragma once

#include "Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#ifdef __EMSCRIPTEN__
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif
#include <unordered_map>
#include <string>

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    bool initialize();
    void cleanup();
    
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    SDL_Texture* getTexture(const std::string& path) const;
    
    void preloadTextures(SDL_Renderer* renderer);
    
    static ResourceManager* getInstance() { return s_instance; }

private:
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    static ResourceManager* s_instance;
};
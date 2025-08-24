#include "ResourceManager.h"
#include "Constants.h"
#include <iostream>

ResourceManager* ResourceManager::s_instance = nullptr;

ResourceManager::ResourceManager() {
    s_instance = this;
}

ResourceManager::~ResourceManager() {
    cleanup();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool ResourceManager::initialize() {
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }
    
    return true;
}

void ResourceManager::cleanup() {
    for (auto& pair : m_textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    m_textures.clear();
    
    IMG_Quit();
}

SDL_Texture* ResourceManager::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    
    std::string fullPath = std::string(Constants::ASSETS_PATH) + path;
    
    SDL_Surface* surface = IMG_Load(fullPath.c_str());
    if (!surface) {
        std::cerr << "Failed to load image " << fullPath << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        std::cerr << "Failed to create texture from " << fullPath << ": " << SDL_GetError() << std::endl;
        return nullptr;
    }
    
    m_textures[path] = texture;
    return texture;
}

SDL_Texture* ResourceManager::getTexture(const std::string& path) const {
    auto it = m_textures.find(path);
    return (it != m_textures.end()) ? it->second : nullptr;
}

void ResourceManager::preloadTextures(SDL_Renderer* renderer) {
    std::vector<std::string> texturePaths = {
        "sprites/mario_small.png",
        "sprites/mario_big.png",
        "sprites/mario_fire.png",
        "sprites/goomba.png",
        "sprites/koopa.png",
        "sprites/items.png",
        "sprites/blocks.png",
        "tiles/ground.png",
        "tiles/brick.png",
        "tiles/pipe.png",
        "backgrounds/sky.png",
        "backgrounds/clouds.png",
        "backgrounds/hills.png",
        "ui/hud.png",
        "ui/font.png"
    };
    
    for (const auto& path : texturePaths) {
        loadTexture(renderer, path);
    }
}
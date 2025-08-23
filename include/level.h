#pragma once
#include <SDL.h>
#include <vector>

struct Platform {
    float x, y, width, height;
    
    SDL_Rect getRect() const {
        return {static_cast<int>(x), static_cast<int>(y), 
                static_cast<int>(width), static_cast<int>(height)};
    }
};

class Level {
public:
    Level();
    ~Level();
    
    void render(SDL_Renderer* renderer);
    bool checkCollision(const SDL_Rect& rect, float& newY);
    
    const std::vector<Platform>& getPlatforms() const { return platforms; }
    
private:
    void createLevel1();
    
    std::vector<Platform> platforms;
    
    // Level dimensions
    static const int LEVEL_WIDTH = 1600;
    static const int LEVEL_HEIGHT = 600;
    static const int GROUND_HEIGHT = 50;
};
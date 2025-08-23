#include "level.h"

Level::Level() {
    createLevel1();
}

Level::~Level() {
}

void Level::createLevel1() {
    // Ground platform
    platforms.push_back({0, 550, 800, 50});
    
    // Floating platforms
    platforms.push_back({200, 450, 150, 20});
    platforms.push_back({400, 350, 150, 20});
    platforms.push_back({150, 250, 100, 20});
    platforms.push_back({500, 200, 120, 20});
    platforms.push_back({650, 400, 100, 20});
    
    // Stairs-like platforms
    platforms.push_back({50, 500, 50, 20});
    platforms.push_back({100, 480, 50, 20});
    platforms.push_back({150, 460, 50, 20});
    
    // High platform
    platforms.push_back({300, 150, 200, 20});
}

void Level::render(SDL_Renderer* renderer) {
    // Set platform color to brown
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    
    for (const auto& platform : platforms) {
        SDL_Rect rect = platform.getRect();
        SDL_RenderFillRect(renderer, &rect);
        
        // Add border for better visibility
        SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    }
}

bool Level::checkCollision(const SDL_Rect& rect, float& newY) {
    bool collision = false;
    
    for (const auto& platform : platforms) {
        SDL_Rect platformRect = platform.getRect();
        
        // Check if Mario is above the platform and falling onto it
        if (rect.x < platformRect.x + platformRect.w &&
            rect.x + rect.w > platformRect.x &&
            rect.y + rect.h >= platformRect.y &&
            rect.y + rect.h <= platformRect.y + platformRect.h + 10) {
            
            // Mario lands on top of platform
            newY = platformRect.y - rect.h;
            collision = true;
            break;
        }
    }
    
    return collision;
}
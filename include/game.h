#pragma once
#include <SDL.h>
#include <memory>
#include "mario.h"
#include "level.h"

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    void cleanup();
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    
    std::unique_ptr<Mario> mario;
    std::unique_ptr<Level> level;
    
    // Game constants
    static const int SCREEN_WIDTH = 800;
    static const int SCREEN_HEIGHT = 600;
    static const int FPS = 60;
};
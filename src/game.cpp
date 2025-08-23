#include "game.h"
#include <iostream>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Game::Game() : window(nullptr), renderer(nullptr), running(false) {}

Game::~Game() {
    cleanup();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    window = SDL_CreateWindow("Super Mario WebAssembly",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH, SCREEN_HEIGHT,
                            SDL_WINDOW_SHOWN);
    
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize game objects
    mario = std::make_unique<Mario>(100, 400);
    level = std::make_unique<Level>();
    
    running = true;
    return true;
}

void Game::run() {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;
    
    // Cap delta time to prevent large jumps
    if (deltaTime > 0.016f) deltaTime = 0.016f;
    
    handleEvents();
    update(deltaTime);
    render();
    
#ifndef __EMSCRIPTEN__
    // For native builds, add frame limiting
    SDL_Delay(16); // ~60 FPS
#endif
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#endif
        }
    }
    
    // Handle continuous input
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    mario->handleInput(keyState);
}

void Game::update(float deltaTime) {
    mario->update(deltaTime);
    
    // Check collision with level
    SDL_Rect marioBounds = mario->getBounds();
    float newY;
    bool onGround = level->checkCollision(marioBounds, newY);
    
    if (onGround) {
        mario->onGround(true);
    } else {
        mario->onGround(false);
    }
}

void Game::render() {
    // Clear screen with sky blue
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);
    
    // Render game objects
    level->render(renderer);
    mario->render(renderer);
    
    // Present the rendered frame
    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    mario.reset();
    level.reset();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}
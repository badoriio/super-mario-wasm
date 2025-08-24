#include "Engine.h"
#include "ResourceManager.h"
#include "AudioManager.h"
#include "InputManager.h"
#include "SaveManager.h"
#include "../engine/Renderer.h"
#include "../game/Game.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static Engine* g_engine = nullptr;

void emscripten_loop() {
    static int frameCount = 0;
    frameCount++;
    if (frameCount == 1) {
        printf("=== FIRST EMSCRIPTEN LOOP CALL ===\n");
    }
    if (g_engine && g_engine->isRunning()) {
        g_engine->run();
    }
}
#endif

Engine::Engine()
    : m_running(false)
    , m_deltaTime(0.0f)
    , m_totalTime(0.0f)
    , m_accumulator(0.0f)
    , m_window(nullptr)
{
    m_lastTime = std::chrono::high_resolution_clock::now();
}

Engine::~Engine() {
    shutdown();
}

bool Engine::initialize() {
    printf("=== ENGINE INITIALIZE START ===\n");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("=== SDL INIT FAILED ===\n");
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    printf("=== SDL INITIALIZED ===\n");
    
    m_window = SDL_CreateWindow(
        "Super Mario WebAssembly",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!m_window) {
        printf("=== WINDOW CREATION FAILED ===\n");
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }
    printf("=== WINDOW CREATED ===\n");
    
    printf("=== CREATING MANAGERS ===\n");
    m_resourceManager = MakeUnique<ResourceManager>();
    m_audioManager = MakeUnique<AudioManager>();
    m_inputManager = MakeUnique<InputManager>();
    m_saveManager = MakeUnique<SaveManager>();
    m_renderer = MakeUnique<Renderer>();
    printf("=== MANAGERS CREATED ===\n");
    
    printf("=== INITIALIZING RENDERER ===\n");
    if (!m_renderer->initialize(m_window)) {
        printf("=== RENDERER INIT FAILED ===\n");
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }
    printf("=== RENDERER INITIALIZED ===\n");
    
    printf("=== INITIALIZING AUDIO MANAGER ===\n");
    if (!m_audioManager->initialize()) {
        printf("=== AUDIO MANAGER INIT FAILED ===\n");
        std::cerr << "Failed to initialize audio manager" << std::endl;
        return false;
    }
    printf("=== AUDIO MANAGER INITIALIZED ===\n");
    
    printf("=== INITIALIZING RESOURCE MANAGER ===\n");
    if (!m_resourceManager->initialize()) {
        printf("=== RESOURCE MANAGER INIT FAILED ===\n");
        std::cerr << "Failed to initialize resource manager" << std::endl;
        return false;
    }
    printf("=== RESOURCE MANAGER INITIALIZED ===\n");
    
    printf("=== INITIALIZING SAVE MANAGER ===\n");
    if (!m_saveManager->initialize()) {
        printf("=== SAVE MANAGER INIT FAILED ===\n");
        std::cerr << "Failed to initialize save manager" << std::endl;
        return false;
    }
    printf("=== SAVE MANAGER INITIALIZED ===\n");
    
    m_running = true;
    
    printf("=== ENGINE INITIALIZATION COMPLETE ===\n");
    return true;
}

void Engine::run() {
#ifndef __EMSCRIPTEN__
    while (m_running) {
        calculateDeltaTime();
        
        m_accumulator += m_deltaTime;
        
        handleEvents();
        
        while (m_accumulator >= FIXED_TIMESTEP) {
            update(FIXED_TIMESTEP);
            m_accumulator -= FIXED_TIMESTEP;
        }
        
        render();
        
        SDL_Delay(1);
    }
#else
    calculateDeltaTime();
    
    m_accumulator += m_deltaTime;
    
    handleEvents();
    
    while (m_accumulator >= FIXED_TIMESTEP) {
        update(FIXED_TIMESTEP);
        m_accumulator -= FIXED_TIMESTEP;
    }
    
    render();
#endif
}

void Engine::shutdown() {
    m_game.reset();
    m_renderer.reset();
    m_saveManager.reset();
    m_inputManager.reset();
    m_audioManager.reset();
    m_resourceManager.reset();
    
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    SDL_Quit();
    
#ifdef __EMSCRIPTEN__
    if (g_engine == this) {
        g_engine = nullptr;
    }
#endif
}

void Engine::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit();
        }
        
        if (m_inputManager) {
            m_inputManager->handleEvent(event);
        }
        
        if (m_game) {
            m_game->handleEvent(event);
        }
    }
    
    if (m_inputManager) {
        m_inputManager->update();
    }
}

void Engine::update(float deltaTime) {
    m_totalTime += deltaTime;
    
    if (m_game) {
        m_game->update(deltaTime);
    }
}

void Engine::render() {
    m_renderer->clear();
    
    if (m_game) {
        m_game->render(m_renderer.get());
    }
    
    m_renderer->present();
}

void Engine::calculateDeltaTime() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - m_lastTime);
    m_deltaTime = duration.count() / 1000000.0f;
    
    if (m_deltaTime > MAX_DELTA_TIME) {
        m_deltaTime = MAX_DELTA_TIME;
    }
    
    m_lastTime = currentTime;
}
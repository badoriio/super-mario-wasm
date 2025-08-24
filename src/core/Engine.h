#pragma once

#include "Types.h"
#include "Constants.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <chrono>

class ResourceManager;
class AudioManager;
class InputManager;
class SaveManager;
class Renderer;
class Game;

class Engine {
public:
    Engine();
    ~Engine();

    bool initialize();
    void run();
    void shutdown();
    
    void setGame(Ptr<Game> game) { m_game = game; }
    
    ResourceManager* getResourceManager() const { return m_resourceManager.get(); }
    AudioManager* getAudioManager() const { return m_audioManager.get(); }
    InputManager* getInputManager() const { return m_inputManager.get(); }
    SaveManager* getSaveManager() const { return m_saveManager.get(); }
    Renderer* getRenderer() const { return m_renderer.get(); }
    
    float getDeltaTime() const { return m_deltaTime; }
    float getTotalTime() const { return m_totalTime; }
    bool isRunning() const { return m_running; }
    
    void quit() { m_running = false; }

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void calculateDeltaTime();
    
    bool m_running;
    float m_deltaTime;
    float m_totalTime;
    float m_accumulator;
    
    SDL_Window* m_window;
    
    UniquePtr<ResourceManager> m_resourceManager;
    UniquePtr<AudioManager> m_audioManager;
    UniquePtr<InputManager> m_inputManager;
    UniquePtr<SaveManager> m_saveManager;
    UniquePtr<Renderer> m_renderer;
    
    Ptr<Game> m_game;
    
    std::chrono::high_resolution_clock::time_point m_lastTime;
    
    static constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
    static constexpr float MAX_DELTA_TIME = 0.05f;
};
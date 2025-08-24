#pragma once

#include "../core/Types.h"
#include "../core/SaveManager.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <memory>

class Engine;
class PhysicsWorld;
class Player;
class Level;
class Camera;
class UI;
class InputManager;
class AudioManager;

class Game {
public:
    Game();
    ~Game();
    
    bool initialize(Engine* engine);
    void shutdown();
    
    void handleEvent(const SDL_Event& event);
    void update(float deltaTime);
    void render(class Renderer* renderer);
    
    GameState getState() const { return m_gameState; }
    void setState(GameState newState);
    
    void startNewGame();
    void loadGame();
    void saveGame();
    
    void loadLevel(int world, int level);
    void nextLevel();
    void restartLevel();
    
    void pauseGame();
    void resumeGame();
    void gameOver();
    void victory();
    
    Player* getPlayer() const { return m_player.get(); }
    Level* getLevel() const { return m_level.get(); }
    Camera* getCamera() const { return m_camera.get(); }

private:
    void updatePlaying(float deltaTime);
    void updatePaused(float deltaTime);
    void updateMenu(float deltaTime);
    void updateGameOver(float deltaTime);
    void updateVictory(float deltaTime);
    void updateSettings(float deltaTime);
    
    void checkPlayerCollisions();
    void checkLevelCompletion();
    void respawnPlayer();
    
    Engine* m_engine;
    
    GameState m_gameState;
    GameState m_previousState;
    
    UniquePtr<PhysicsWorld> m_physics;
    UniquePtr<Player> m_player;
    UniquePtr<Level> m_level;
    UniquePtr<Camera> m_camera;
    UniquePtr<UI> m_ui;
    
    InputManager* m_input;
    AudioManager* m_audio;
    
    GameSave m_currentSave;
    
    int m_currentWorld;
    int m_currentLevel;
    
    float m_gameTimer;
    float m_pauseTimer;
    
    bool m_levelCompleted;
    bool m_playerDied;
    
    static constexpr int MAX_WORLDS = 4;
    static constexpr int MAX_LEVELS_PER_WORLD = 4;
};
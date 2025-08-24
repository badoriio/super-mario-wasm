#include "Game.h"
#include "../core/Engine.h"
#include "../core/InputManager.h"
#include "../core/AudioManager.h"
#include "../physics/PhysicsWorld.h"
#include "Player.h"
#include "Level.h"
#include "../engine/Camera.h"
#include "../ui/UI.h"
#include "../engine/Renderer.h"
#include "../core/Constants.h"
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Game::Game()
    : m_engine(nullptr)
    , m_gameState(GameState::MENU)
    , m_previousState(GameState::MENU)
    , m_input(nullptr)
    , m_audio(nullptr)
    , m_currentWorld(1)
    , m_currentLevel(1)
    , m_gameTimer(0.0f)
    , m_pauseTimer(0.0f)
    , m_levelCompleted(false)
    , m_playerDied(false)
{
}

Game::~Game() {
    shutdown();
}

bool Game::initialize(Engine* engine) {
    printf("=== GAME INITIALIZING ===\n");
    m_engine = engine;
    m_input = engine->getInputManager();
    m_audio = engine->getAudioManager();
    
    m_physics = MakeUnique<PhysicsWorld>();
    m_physics->setGravity(Vector2(0, Constants::GRAVITY));
    
    m_camera = MakeUnique<Camera>(Vector2(0, 0), Vector2(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT));
    m_ui = MakeUnique<UI>();
    
    if (!m_ui->initialize(engine)) {
        std::cerr << "Failed to initialize UI" << std::endl;
        return false;
    }
    
    engine->getRenderer()->setCamera(m_camera.get());
    
    m_currentSave.currentWorld = 1;
    m_currentSave.currentLevel = 1;
    m_currentSave.lives = Constants::MAX_LIVES;
    
    printf("=== LOADING LEVEL %d-%d ===\n", m_currentWorld, m_currentLevel);
    loadLevel(m_currentWorld, m_currentLevel);
    
    // Start the game immediately for testing
    printf("=== SETTING GAME STATE TO PLAYING ===\n");
    setState(GameState::PLAYING);
    
    if (m_audio) {
        m_audio->playMusic("menu");
    }
    
    return true;
}

void Game::shutdown() {
    m_ui.reset();
    m_camera.reset();
    m_level.reset();
    m_player.reset();
    m_physics.reset();
}

void Game::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                if (m_gameState == GameState::PLAYING) {
                    pauseGame();
                } else if (m_gameState == GameState::PAUSED) {
                    resumeGame();
                }
            }
            break;
    }
    
    if (m_ui) {
        m_ui->handleEvent(event);
    }
}

void Game::update(float deltaTime) {
    // Game state debug removed
    
    switch (m_gameState) {
        case GameState::MENU:
            updateMenu(deltaTime);
            break;
        case GameState::PLAYING:
            updatePlaying(deltaTime);
            break;
        case GameState::PAUSED:
            updatePaused(deltaTime);
            break;
        case GameState::GAME_OVER:
            updateGameOver(deltaTime);
            break;
        case GameState::VICTORY:
            updateVictory(deltaTime);
            break;
        case GameState::SETTINGS:
            updateSettings(deltaTime);
            break;
    }
    
    if (m_ui) {
        m_ui->update(deltaTime);
    }
}

void Game::render(Renderer* renderer) {
    if (m_level && (m_gameState == GameState::PLAYING || m_gameState == GameState::PAUSED)) {
        m_level->render(renderer, m_camera.get());
        
        if (m_player) {
            m_player->render(renderer);
        }
        
        // Debug: render player position as a red square
        if (m_player && m_camera) {
            Vector2 playerPos = m_player->getPosition();
            
            Rectangle playerRect = {
                playerPos.x,
                playerPos.y,
                32, 32
            };
            
            SDL_Color red = {255, 0, 0, 255};
            renderer->drawRectangle(playerRect, red, true, 10);
        }
    }
    
    if (m_ui) {
        m_ui->render(renderer, m_gameState);
    }
}

void Game::setState(GameState newState) {
    if (m_gameState != newState) {
        m_previousState = m_gameState;
        m_gameState = newState;
        
        switch (m_gameState) {
            case GameState::MENU:
                if (m_audio) m_audio->playMusic("menu");
                break;
            case GameState::PLAYING:
                if (m_level && m_audio) {
                    m_audio->playMusic(m_level->getMusicTrack());
                }
                break;
            case GameState::PAUSED:
                if (m_audio) m_audio->pauseMusic();
                break;
            case GameState::GAME_OVER:
                if (m_audio) m_audio->playMusic("gameover");
                break;
            case GameState::VICTORY:
                if (m_audio) m_audio->playMusic("victory");
                break;
            case GameState::SETTINGS:
                break;
        }
    }
}

void Game::startNewGame() {
    m_currentSave = GameSave{};
    m_currentSave.currentWorld = 1;
    m_currentSave.currentLevel = 1;
    m_currentSave.lives = Constants::MAX_LIVES;
    
    loadLevel(1, 1);
    setState(GameState::PLAYING);
}

void Game::loadGame() {
    SaveManager* saveManager = m_engine->getSaveManager();
    if (saveManager) {
        saveManager->loadGame([this](const GameSave& save) {
            m_currentSave = save;
            loadLevel(save.currentWorld, save.currentLevel);
            setState(GameState::PLAYING);
        });
    }
}

void Game::saveGame() {
    SaveManager* saveManager = m_engine->getSaveManager();
    if (saveManager && m_player) {
        m_currentSave.currentWorld = m_currentWorld;
        m_currentSave.currentLevel = m_currentLevel;
        m_currentSave.score = m_player->getScore();
        m_currentSave.coins = m_player->getCoins();
        m_currentSave.lives = m_player->getLives();
        
        saveManager->saveGame(m_currentSave);
    }
}

void Game::loadLevel(int world, int level) {
    m_currentWorld = world;
    m_currentLevel = level;
    
    m_level = MakeUnique<Level>();
    m_level->setPhysicsWorld(m_physics.get());
    
    std::string filename = "world" + std::to_string(world) + "_level" + std::to_string(level) + ".json";
    if (!m_level->loadFromFile(filename)) {
        printf("=== LEVEL LOADING FAILED: %s ===\n", filename.c_str());
        std::cerr << "Failed to load level: " << filename << std::endl;
        return;
    }
    
    // Connect physics world to level for collision detection
    m_physics->setLevel(m_level.get());
    
    if (m_player) {
        m_player.reset();
    }
    
    m_player = MakeUnique<Player>(m_level->getPlayerSpawn());
    m_player->initialize(m_physics.get(), m_input, m_audio);
    
    if (m_camera) {
        Vector2 playerSpawn = m_level->getPlayerSpawn();
        Vector2 cameraPos = Vector2(playerSpawn.x - Constants::SCREEN_WIDTH/2, 
                                   playerSpawn.y - Constants::SCREEN_HEIGHT/2);
        
        // Ensure camera doesn't go below x=0 (left boundary of level)
        cameraPos.x = std::max(0.0f, cameraPos.x);
        
        printf("=== Player spawn: (%.1f, %.1f) ===\n", playerSpawn.x, playerSpawn.y);
        printf("=== Camera position: (%.1f, %.1f) ===\n", cameraPos.x, cameraPos.y);
        
        m_camera->setTarget(playerSpawn);
        m_camera->setPosition(cameraPos);
    }
    
    m_gameTimer = static_cast<float>(m_level->getTimeLimit());
    m_levelCompleted = false;
    m_playerDied = false;
    
    if (m_ui) {
        m_ui->setPlayer(m_player.get());
        m_ui->setLevel(m_level.get());
    }
}

void Game::nextLevel() {
    if (m_currentLevel < MAX_LEVELS_PER_WORLD) {
        loadLevel(m_currentWorld, m_currentLevel + 1);
    } else if (m_currentWorld < MAX_WORLDS) {
        loadLevel(m_currentWorld + 1, 1);
    } else {
        victory();
    }
}

void Game::restartLevel() {
    loadLevel(m_currentWorld, m_currentLevel);
}

void Game::pauseGame() {
    setState(GameState::PAUSED);
}

void Game::resumeGame() {
    setState(GameState::PLAYING);
    if (m_audio) m_audio->resumeMusic();
}

void Game::gameOver() {
    setState(GameState::GAME_OVER);
}

void Game::victory() {
    setState(GameState::VICTORY);
    saveGame();
}

void Game::updatePlaying(float deltaTime) {
    
    m_gameTimer -= deltaTime;
    if (m_gameTimer <= 0.0f) {
        gameOver();
        return;
    }
    
    if (m_physics) {
        m_physics->update(deltaTime);
    }
    
    if (m_player) {
        m_player->update(deltaTime);
        
        if (!m_player->isAlive()) {
            if (m_player->getLives() > 0) {
                m_playerDied = true;
                m_pauseTimer = 2.0f;
            } else {
                gameOver();
            }
        }
    }
    
    if (m_playerDied) {
        m_pauseTimer -= deltaTime;
        if (m_pauseTimer <= 0.0f) {
            respawnPlayer();
            m_playerDied = false;
        }
        return;
    }
    
    if (m_level) {
        m_level->update(deltaTime, m_camera.get());
    }
    
    if (m_camera && m_player) {
        m_camera->setTarget(m_player->getPosition() + Vector2(12.0f, 16.0f));
        m_camera->update(deltaTime);
    }
    
    checkPlayerCollisions();
    checkLevelCompletion();
}

void Game::updatePaused(float deltaTime) {
}

void Game::updateMenu(float deltaTime) {
    if (m_input && m_input->isActionJustPressed(InputAction::MENU_SELECT)) {
        startNewGame();
    }
}

void Game::updateGameOver(float deltaTime) {
    if (m_input && m_input->isActionJustPressed(InputAction::MENU_SELECT)) {
        setState(GameState::MENU);
    }
}

void Game::updateVictory(float deltaTime) {
    if (m_input && m_input->isActionJustPressed(InputAction::MENU_SELECT)) {
        setState(GameState::MENU);
    }
}

void Game::updateSettings(float deltaTime) {
}

void Game::checkPlayerCollisions() {
}

void Game::checkLevelCompletion() {
    if (m_player && m_level) {
        Vector2 playerPos = m_player->getPosition();
        Vector2 goalPos = m_level->getGoalPosition();
        
        float distance = (playerPos - goalPos).length();
        if (distance < 50.0f) {
            m_levelCompleted = true;
            nextLevel();
        }
    }
}

void Game::respawnPlayer() {
    if (m_player && m_level) {
        m_player->respawn(m_level->getPlayerSpawn());
    }
}
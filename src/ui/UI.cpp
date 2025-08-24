#include "UI.h"
#include "Menu.h"
#include "HUD.h"
#include "../core/Engine.h"

UI::UI()
    : m_engine(nullptr)
    , m_player(nullptr)
    , m_level(nullptr)
{
}

UI::~UI() {
    shutdown();
}

bool UI::initialize(Engine* engine) {
    m_engine = engine;
    
    m_menu = MakeUnique<Menu>();
    m_hud = MakeUnique<HUD>();
    
    if (!m_menu->initialize(engine)) {
        return false;
    }
    
    if (!m_hud->initialize(engine)) {
        return false;
    }
    
    return true;
}

void UI::shutdown() {
    m_hud.reset();
    m_menu.reset();
}

void UI::handleEvent(const SDL_Event& event) {
    if (m_menu) {
        m_menu->handleEvent(event);
    }
}

void UI::update(float deltaTime) {
    if (m_menu) {
        m_menu->update(deltaTime);
    }
    
    if (m_hud) {
        m_hud->setPlayer(m_player);
        m_hud->setLevel(m_level);
        m_hud->update(deltaTime);
    }
}

void UI::render(Renderer* renderer, GameState gameState) {
    switch (gameState) {
        case GameState::MENU:
        case GameState::GAME_OVER:
        case GameState::VICTORY:
        case GameState::SETTINGS:
            if (m_menu) {
                m_menu->render(renderer, gameState);
            }
            break;
            
        case GameState::PLAYING:
        case GameState::PAUSED:
            if (m_hud) {
                m_hud->render(renderer);
            }
            
            if (gameState == GameState::PAUSED && m_menu) {
                m_menu->render(renderer, gameState);
            }
            break;
    }
}
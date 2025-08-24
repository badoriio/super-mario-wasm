#include "HUD.h"
#include "../core/Engine.h"
#include "../core/ResourceManager.h"
#include "../game/Player.h"
#include "../game/Level.h"
#include "../engine/Renderer.h"
#include "../core/Constants.h"
#include <sstream>

HUD::HUD()
    : m_engine(nullptr)
    , m_player(nullptr)
    , m_level(nullptr)
    , m_hudTexture(nullptr)
    , m_fontTexture(nullptr)
    , m_gameTime(0.0f)
{
}

HUD::~HUD() {
    shutdown();
}

bool HUD::initialize(Engine* engine) {
    m_engine = engine;
    
    ResourceManager* resources = ResourceManager::getInstance();
    if (resources) {
        m_hudTexture = resources->getTexture("ui/hud.png");
        m_fontTexture = resources->getTexture("ui/font.png");
    }
    
    return true;
}

void HUD::shutdown() {
}

void HUD::update(float deltaTime) {
    m_gameTime += deltaTime;
}

void HUD::render(Renderer* renderer) {
    // Background for HUD
    SDL_Color hudBg = {0, 0, 0, 200};
    renderer->drawRectangle(Rectangle(0, 0, Constants::SCREEN_WIDTH, Constants::HUD_HEIGHT), 
                           hudBg, true, 50);
    
    renderScore(renderer);
    renderLives(renderer);
    renderCoins(renderer);
    renderTime(renderer);
    renderWorld(renderer);
}

void HUD::renderScore(Renderer* renderer) {
    Vector2 position(20, 10);
    renderText(renderer, "SCORE", position);
    
    if (m_player) {
        Vector2 scorePos(20, 30);
        renderNumber(renderer, m_player->getScore(), scorePos);
    }
}

void HUD::renderLives(Renderer* renderer) {
    Vector2 position(200, 10);
    renderText(renderer, "LIVES", position);
    
    if (m_player) {
        Vector2 livesPos(200, 30);
        renderNumber(renderer, m_player->getLives(), livesPos);
    }
}

void HUD::renderCoins(Renderer* renderer) {
    Vector2 position(300, 10);
    renderText(renderer, "COINS", position);
    
    if (m_player) {
        Vector2 coinsPos(300, 30);
        renderNumber(renderer, m_player->getCoins(), coinsPos);
    }
}

void HUD::renderTime(Renderer* renderer) {
    Vector2 position(500, 10);
    renderText(renderer, "TIME", position);
    
    if (m_level) {
        int timeRemaining = std::max(0, m_level->getTimeLimit() - static_cast<int>(m_gameTime));
        Vector2 timePos(500, 30);
        renderNumber(renderer, timeRemaining, timePos);
    }
}

void HUD::renderWorld(Renderer* renderer) {
    Vector2 position(700, 10);
    renderText(renderer, "WORLD", position);
    
    if (m_level) {
        Vector2 worldPos(700, 30);
        renderText(renderer, "1-1", worldPos);
    }
}

void HUD::renderText(Renderer* renderer, const std::string& text, const Vector2& position, SDL_Color color) {
    if (!m_fontTexture) {
        // Fallback rendering
        for (size_t i = 0; i < text.length(); ++i) {
            Vector2 charPos(position.x + i * 12, position.y);
            renderer->drawRectangle(Rectangle(charPos.x, charPos.y, 10, 14), color, true, 51);
        }
        return;
    }
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c >= 32 && c < 127) {
            int charIndex = c - 32;
            int srcX = (charIndex % 16) * 8;
            int srcY = (charIndex / 16) * 16;
            
            SDL_Rect srcRect = {srcX, srcY, 8, 16};
            SDL_Rect dstRect = {
                static_cast<int>(position.x + i * 8),
                static_cast<int>(position.y),
                8, 16
            };
            
            renderer->drawTexture(m_fontTexture, srcRect, dstRect, 0.0f, nullptr, SDL_FLIP_NONE, 51);
        }
    }
}

void HUD::renderNumber(Renderer* renderer, int number, const Vector2& position, SDL_Color color) {
    std::stringstream ss;
    ss << number;
    renderText(renderer, ss.str(), position, color);
}
#pragma once

#include "../core/Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <string>
#include <vector>

class Engine;
class Renderer;

struct MenuItem {
    std::string text;
    std::function<void()> callback;
    bool enabled = true;
};

class Menu {
public:
    Menu();
    ~Menu();
    
    bool initialize(Engine* engine);
    void shutdown();
    
    void handleEvent(const SDL_Event& event);
    void update(float deltaTime);
    void render(Renderer* renderer, GameState gameState);

private:
    void setupMainMenu();
    void setupPauseMenu();
    void setupGameOverMenu();
    void setupVictoryMenu();
    void setupSettingsMenu();
    
    void renderMenu(Renderer* renderer, const std::vector<MenuItem>& items, const std::string& title);
    void renderText(Renderer* renderer, const std::string& text, const Vector2& position, SDL_Color color = {255, 255, 255, 255});
    
    Engine* m_engine;
    
    std::vector<MenuItem> m_mainMenuItems;
    std::vector<MenuItem> m_pauseMenuItems;
    std::vector<MenuItem> m_gameOverMenuItems;
    std::vector<MenuItem> m_victoryMenuItems;
    std::vector<MenuItem> m_settingsMenuItems;
    
    int m_selectedItem;
    
    SDL_Texture* m_fontTexture;
};
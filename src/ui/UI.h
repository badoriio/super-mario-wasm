#pragma once

#include "../core/Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <string>

class Engine;
class Player;
class Level;
class Renderer;
class Menu;
class HUD;

class UI {
public:
    UI();
    ~UI();
    
    bool initialize(Engine* engine);
    void shutdown();
    
    void handleEvent(const SDL_Event& event);
    void update(float deltaTime);
    void render(Renderer* renderer, GameState gameState);
    
    void setPlayer(Player* player) { m_player = player; }
    void setLevel(Level* level) { m_level = level; }

private:
    Engine* m_engine;
    Player* m_player;
    Level* m_level;
    
    UniquePtr<Menu> m_menu;
    UniquePtr<HUD> m_hud;
};
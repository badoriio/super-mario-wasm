#pragma once

#include "../core/Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

class Engine;
class Player;
class Level;
class Renderer;

class HUD {
public:
    HUD();
    ~HUD();
    
    bool initialize(Engine* engine);
    void shutdown();
    
    void update(float deltaTime);
    void render(Renderer* renderer);
    
    void setPlayer(Player* player) { m_player = player; }
    void setLevel(Level* level) { m_level = level; }

private:
    void renderScore(Renderer* renderer);
    void renderLives(Renderer* renderer);
    void renderCoins(Renderer* renderer);
    void renderTime(Renderer* renderer);
    void renderWorld(Renderer* renderer);
    
    void renderText(Renderer* renderer, const std::string& text, const Vector2& position, SDL_Color color = {255, 255, 255, 255});
    void renderNumber(Renderer* renderer, int number, const Vector2& position, SDL_Color color = {255, 255, 255, 255});
    
    Engine* m_engine;
    Player* m_player;
    Level* m_level;
    
    SDL_Texture* m_hudTexture;
    SDL_Texture* m_fontTexture;
    
    float m_gameTime;
};
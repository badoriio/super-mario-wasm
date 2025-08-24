#pragma once

#include "../core/Types.h"
#include <vector>
#include <string>
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

struct Tile {
    int id = 0;
    bool solid = false;
    bool oneWay = false;
    SDL_Rect srcRect = {0, 0, 0, 0};
    
    Tile() = default;
    Tile(int tileId, bool isSolid = false, bool isOneWay = false) 
        : id(tileId), solid(isSolid), oneWay(isOneWay) {}
};

struct BackgroundLayer {
    SDL_Texture* texture = nullptr;
    float scrollSpeed = 1.0f;
    float offset = 0.0f;
    int layer = -10;
};

struct LevelData {
    int width = 0;
    int height = 0;
    std::vector<std::vector<int>> tiles;
    std::vector<std::vector<int>> background;
    std::vector<std::vector<int>> collision;
    
    Vector2 playerSpawn = {64, 400};
    Vector2 goalPosition = {0, 0};
    
    std::string name;
    std::string musicTrack;
    int timeLimit = 300;
};

class Renderer;
class Camera;
class PhysicsWorld;

class Level {
public:
    Level();
    ~Level();
    
    bool loadFromFile(const std::string& filename);
    bool loadFromData(const LevelData& data);
    
    void update(float deltaTime, Camera* camera);
    void render(Renderer* renderer, Camera* camera);
    
    bool checkCollision(const Rectangle& rect) const;
    bool checkCollision(int tileX, int tileY) const;
    
    Tile getTile(int x, int y) const;
    void setTile(int x, int y, const Tile& tile);
    
    Vector2 getPlayerSpawn() const { return m_data.playerSpawn; }
    Vector2 getGoalPosition() const { return m_data.goalPosition; }
    
    int getWidth() const { return m_data.width; }
    int getHeight() const { return m_data.height; }
    
    Rectangle getBounds() const;
    
    std::string getName() const { return m_data.name; }
    std::string getMusicTrack() const { return m_data.musicTrack; }
    int getTimeLimit() const { return m_data.timeLimit; }
    
    void setPhysicsWorld(PhysicsWorld* physics) { m_physics = physics; }

private:
    void setupTileTextures();
    void setupBackgroundLayers();
    void renderTileLayer(Renderer* renderer, const std::vector<std::vector<int>>& layer, 
                        int renderLayer, Camera* camera);
    void renderBackgrounds(Renderer* renderer, Camera* camera);
    void renderDebugTiles(Renderer* renderer, Camera* camera);
    
    LevelData m_data;
    std::vector<Tile> m_tileSet;
    std::vector<BackgroundLayer> m_backgroundLayers;
    
    SDL_Texture* m_tilesetTexture;
    int m_tilesetColumns;
    
    PhysicsWorld* m_physics;
};
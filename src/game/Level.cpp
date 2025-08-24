#include "Level.h"
#include "../engine/Renderer.h"
#include "../engine/Camera.h"
#include "../physics/PhysicsWorld.h"
#include "../core/ResourceManager.h"
#include "../core/Constants.h"
#include "../data/LevelLoader.h"
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Level::Level()
    : m_tilesetTexture(nullptr)
    , m_tilesetColumns(0)
    , m_physics(nullptr)
{
    setupTileTextures();
    setupBackgroundLayers();
}

Level::~Level() {
}

bool Level::loadFromFile(const std::string& filename) {
    printf("=== Level::loadFromFile called with: %s ===\n", filename.c_str());
    LevelLoader loader;
    if (loader.loadLevel(filename, m_data)) {
        printf("=== LevelLoader succeeded, calling loadFromData ===\n");
        return loadFromData(m_data);
    }
    printf("=== LevelLoader failed for %s ===\n", filename.c_str());
    return false;
}

bool Level::loadFromData(const LevelData& data) {
    printf("=== Level::loadFromData called, level size: %dx%d ===\n", data.width, data.height);
    m_data = data;
    
    m_tileSet.clear();
    m_tileSet.resize(256);
    
    for (int i = 0; i < 256; ++i) {
        m_tileSet[i] = Tile(i);
        
        int col = i % m_tilesetColumns;
        int row = i / m_tilesetColumns;
        m_tileSet[i].srcRect = {
            col * Constants::TILE_SIZE,
            row * Constants::TILE_SIZE,
            Constants::TILE_SIZE,
            Constants::TILE_SIZE
        };
        
        if (i == 1 || i == 2 || i == 3) {
            m_tileSet[i].solid = true;
        }
    }
    
    return true;
}

void Level::update(float deltaTime, Camera* camera) {
    if (camera) {
        Rectangle levelBounds = getBounds();
        camera->setBounds(levelBounds);
    }
    
    for (auto& layer : m_backgroundLayers) {
        layer.offset += layer.scrollSpeed * deltaTime * 10.0f;
        if (layer.offset >= 1024.0f) {
            layer.offset -= 1024.0f;
        }
    }
}

void Level::render(Renderer* renderer, Camera* camera) {
    renderBackgrounds(renderer, camera);
    renderTileLayer(renderer, m_data.background, -5, camera);
    renderTileLayer(renderer, m_data.tiles, 0, camera);
    
    // Fallback debug rendering - draw simple colored rectangles
    if (!m_tilesetTexture) {
        renderDebugTiles(renderer, camera);
    }
}

bool Level::checkCollision(const Rectangle& rect) const {
    int startX = static_cast<int>(rect.x / Constants::TILE_SIZE);
    int endX = static_cast<int>((rect.x + rect.w) / Constants::TILE_SIZE);
    int startY = static_cast<int>(rect.y / Constants::TILE_SIZE);
    int endY = static_cast<int>((rect.y + rect.h) / Constants::TILE_SIZE);
    
    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (checkCollision(x, y)) {
                return true;
            }
        }
    }
    
    return false;
}

bool Level::checkCollision(int tileX, int tileY) const {
    if (tileX < 0 || tileY < 0 || tileX >= m_data.width || tileY >= m_data.height) {
        return true;
    }
    
    if (!m_data.collision.empty()) {
        int tileId = m_data.collision[tileY][tileX];
        return tileId > 0;
    }
    
    int tileId = m_data.tiles[tileY][tileX];
    if (tileId > 0 && tileId < static_cast<int>(m_tileSet.size())) {
        return m_tileSet[tileId].solid;
    }
    
    return false;
}

Tile Level::getTile(int x, int y) const {
    if (x < 0 || y < 0 || x >= m_data.width || y >= m_data.height) {
        return Tile();
    }
    
    int tileId = m_data.tiles[y][x];
    if (tileId < static_cast<int>(m_tileSet.size())) {
        return m_tileSet[tileId];
    }
    
    return Tile();
}

void Level::setTile(int x, int y, const Tile& tile) {
    if (x >= 0 && y >= 0 && x < m_data.width && y < m_data.height) {
        m_data.tiles[y][x] = tile.id;
    }
}

Rectangle Level::getBounds() const {
    return Rectangle(0, 0, 
                    m_data.width * Constants::TILE_SIZE, 
                    m_data.height * Constants::TILE_SIZE);
}

void Level::setupTileTextures() {
    ResourceManager* resources = ResourceManager::getInstance();
    if (resources) {
        m_tilesetTexture = resources->getTexture("tiles/tileset.png");
        if (m_tilesetTexture) {
            int textureWidth;
            SDL_QueryTexture(m_tilesetTexture, nullptr, nullptr, &textureWidth, nullptr);
            m_tilesetColumns = textureWidth / Constants::TILE_SIZE;
        } else {
            // Fallback when texture is not available
            m_tilesetColumns = 16; // Default 16 columns for 16x16 tiles in 256px texture
        }
    } else {
        // Fallback when ResourceManager is not available
        m_tilesetColumns = 16;
    }
}

void Level::setupBackgroundLayers() {
    ResourceManager* resources = ResourceManager::getInstance();
    if (resources) {
        BackgroundLayer sky;
        sky.texture = resources->getTexture("backgrounds/sky.png");
        sky.scrollSpeed = 0.0f;
        sky.layer = -20;
        m_backgroundLayers.push_back(sky);
        
        BackgroundLayer clouds;
        clouds.texture = resources->getTexture("backgrounds/clouds.png");
        clouds.scrollSpeed = 0.2f;
        clouds.layer = -15;
        m_backgroundLayers.push_back(clouds);
        
        BackgroundLayer hills;
        hills.texture = resources->getTexture("backgrounds/hills.png");
        hills.scrollSpeed = 0.5f;
        hills.layer = -10;
        m_backgroundLayers.push_back(hills);
    }
}

void Level::renderTileLayer(Renderer* renderer, const std::vector<std::vector<int>>& layer, 
                           int renderLayer, Camera* camera) {
    if (!m_tilesetTexture || layer.empty()) return;
    
    Rectangle cameraBounds(0, 0, Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT);
    if (camera) {
        Vector2 cameraPos = camera->getPosition();
        cameraBounds.x = cameraPos.x;
        cameraBounds.y = cameraPos.y;
    }
    
    int startX = std::max(0, static_cast<int>(cameraBounds.x / Constants::TILE_SIZE) - 1);
    int endX = std::min(m_data.width, static_cast<int>((cameraBounds.x + cameraBounds.w) / Constants::TILE_SIZE) + 1);
    int startY = std::max(0, static_cast<int>(cameraBounds.y / Constants::TILE_SIZE) - 1);
    int endY = std::min(m_data.height, static_cast<int>((cameraBounds.y + cameraBounds.h) / Constants::TILE_SIZE) + 1);
    
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (y < static_cast<int>(layer.size()) && x < static_cast<int>(layer[y].size())) {
                int tileId = layer[y][x];
                if (tileId > 0 && tileId < static_cast<int>(m_tileSet.size())) {
                    const Tile& tile = m_tileSet[tileId];
                    
                    SDL_Rect dstRect = {
                        x * Constants::TILE_SIZE,
                        y * Constants::TILE_SIZE,
                        Constants::TILE_SIZE,
                        Constants::TILE_SIZE
                    };
                    
                    renderer->drawTexture(m_tilesetTexture, tile.srcRect, dstRect, 
                                        0.0f, nullptr, SDL_FLIP_NONE, renderLayer);
                }
            }
        }
    }
}

void Level::renderBackgrounds(Renderer* renderer, Camera* camera) {
    Vector2 cameraPos = camera ? camera->getPosition() : Vector2(0, 0);
    
    for (const auto& layer : m_backgroundLayers) {
        if (!layer.texture) continue;
        
        int textureWidth, textureHeight;
        SDL_QueryTexture(layer.texture, nullptr, nullptr, &textureWidth, &textureHeight);
        
        float parallaxOffset = cameraPos.x * layer.scrollSpeed + layer.offset;
        
        int numRepeats = (Constants::SCREEN_WIDTH / textureWidth) + 2;
        for (int i = -1; i < numRepeats; ++i) {
            SDL_Rect srcRect = {0, 0, textureWidth, textureHeight};
            SDL_Rect dstRect = {
                static_cast<int>(i * textureWidth - parallaxOffset),
                0,
                textureWidth,
                textureHeight
            };
            
            renderer->drawTexture(layer.texture, srcRect, dstRect, 
                                0.0f, nullptr, SDL_FLIP_NONE, layer.layer);
        }
    }
}

void Level::renderDebugTiles(Renderer* renderer, Camera* camera) {
    Vector2 cameraPos = camera ? camera->getPosition() : Vector2(0, 0);
    
    // Calculate visible area
    int startX = std::max(0, static_cast<int>(cameraPos.x / Constants::TILE_SIZE) - 1);
    int endX = std::min(m_data.width, static_cast<int>((cameraPos.x + Constants::SCREEN_WIDTH) / Constants::TILE_SIZE) + 1);
    int startY = std::max(0, static_cast<int>(cameraPos.y / Constants::TILE_SIZE) - 1);
    int endY = std::min(m_data.height, static_cast<int>((cameraPos.y + Constants::SCREEN_HEIGHT) / Constants::TILE_SIZE) + 1);
    
    // Render tiles as colored rectangles
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            if (x < 0 || x >= m_data.width || y < 0 || y >= m_data.height) continue;
            
            int tileId = m_data.tiles[y][x];
            if (tileId > 0) {
                Rectangle dstRect = {
                    static_cast<float>(x * Constants::TILE_SIZE),
                    static_cast<float>(y * Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE),
                    static_cast<float>(Constants::TILE_SIZE)
                };
                
                // Different colors for different tile types
                SDL_Color color;
                switch (tileId) {
                    case 1: color = {139, 69, 19, 255}; break;   // Brown - ground surface
                    case 2: color = {101, 67, 33, 255}; break;   // Dark brown - underground
                    case 3: color = {255, 215, 0, 255}; break;   // Gold - special blocks
                    default: color = {128, 128, 128, 255}; break; // Gray - default
                }
                
                renderer->drawRectangle(dstRect, color, true, 0);
            }
        }
    }
}
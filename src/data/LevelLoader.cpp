#include "LevelLoader.h"
#include "../core/Constants.h"
#include <fstream>
#include <sstream>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

LevelLoader::LevelLoader() {
}

LevelLoader::~LevelLoader() {
}

bool LevelLoader::loadLevel(const std::string& filename, LevelData& data) {
    std::string fullPath = std::string(Constants::ASSETS_PATH) + "levels/" + filename;
    printf("=== LevelLoader::loadLevel trying to read: %s ===\n", fullPath.c_str());
    std::string content = readFile(fullPath);
    
    printf("=== File content length: %zu ===\n", content.length());
    if (content.empty()) {
        printf("=== Creating default test level for %s ===\n", filename.c_str());
        data = createTestLevel(50, 15);
        printf("=== Test level created: %dx%d tiles ===\n", data.width, data.height);
        printf("=== Ground level tiles from %d to %d ===\n", (15-3), 15);
        return true;
    }
    
    printf("=== File content not empty, trying to parse ===\n");
    if (filename.find(".json") != std::string::npos) {
        printf("=== Parsing JSON file ===\n");
        bool jsonResult = parseJSON(content, data);
        if (!jsonResult) {
            printf("=== JSON parsing failed, creating test level instead ===\n");
            data = createTestLevel(50, 15);
            printf("=== Test level created: %dx%d tiles ===\n", data.width, data.height);
            return true;
        }
        return jsonResult;
    } else if (filename.find(".tmx") != std::string::npos) {
        printf("=== Parsing TMX file ===\n");
        return parseTMX(content, data);
    }
    
    printf("=== Unknown file format, creating test level ===\n");
    data = createTestLevel(50, 15);
    return true;
}

bool LevelLoader::saveLevel(const std::string& filename, const LevelData& data) {
    return false;
}

LevelData LevelLoader::createTestLevel(int width, int height) {
    LevelData data;
    data.width = width;
    data.height = height;
    data.name = "Test Level";
    data.musicTrack = "overworld";
    data.timeLimit = 400;
    data.playerSpawn = Vector2(64, (height - 5) * Constants::TILE_SIZE);
    data.goalPosition = Vector2((width - 5) * Constants::TILE_SIZE, (height - 8) * Constants::TILE_SIZE);
    
    data.tiles.resize(height);
    data.background.resize(height);
    data.collision.resize(height);
    
    for (int y = 0; y < height; ++y) {
        data.tiles[y].resize(width, 0);
        data.background[y].resize(width, 0);
        data.collision[y].resize(width, 0);
    }
    
    int groundLevel = height - 3;
    
    for (int x = 0; x < width; ++x) {
        for (int y = groundLevel; y < height; ++y) {
            if (y == groundLevel) {
                data.tiles[y][x] = 1;
            } else {
                data.tiles[y][x] = 2;
            }
            data.collision[y][x] = 1;
        }
    }
    
    for (int x = 10; x < 13; ++x) {
        data.tiles[groundLevel - 3][x] = 1;
        data.collision[groundLevel - 3][x] = 1;
    }
    
    for (int x = 20; x < 22; ++x) {
        data.tiles[groundLevel - 5][x] = 1;
        data.collision[groundLevel - 5][x] = 1;
    }
    
    for (int x = 30; x < 35; ++x) {
        data.tiles[groundLevel - 1][x] = 0;
        data.collision[groundLevel - 1][x] = 0;
    }
    
    data.tiles[groundLevel - 8][width - 5] = 3;
    data.collision[groundLevel - 8][width - 5] = 0;
    
    return data;
}

bool LevelLoader::parseJSON(const std::string& content, LevelData& data) {
    return false;
}

bool LevelLoader::parseTMX(const std::string& content, LevelData& data) {
    return false;
}

std::string LevelLoader::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool LevelLoader::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}
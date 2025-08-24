#pragma once

#include "../core/Types.h"
#include "../game/Level.h"
#include <string>

class LevelLoader {
public:
    LevelLoader();
    ~LevelLoader();
    
    bool loadLevel(const std::string& filename, LevelData& data);
    bool saveLevel(const std::string& filename, const LevelData& data);
    
    static LevelData createTestLevel(int width, int height);

private:
    bool parseJSON(const std::string& content, LevelData& data);
    bool parseTMX(const std::string& content, LevelData& data);
    
    std::string readFile(const std::string& filename);
    bool writeFile(const std::string& filename, const std::string& content);
};
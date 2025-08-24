#pragma once

#include <string>

struct GameConfig {
    float musicVolume = 0.7f;
    float sfxVolume = 0.8f;
    bool fullscreen = false;
    int screenWidth = 1024;
    int screenHeight = 576;
    bool vsync = true;
};

class ConfigLoader {
public:
    ConfigLoader();
    ~ConfigLoader();
    
    bool loadConfig(const std::string& filename, GameConfig& config);
    bool saveConfig(const std::string& filename, const GameConfig& config);
    
    static GameConfig getDefaultConfig();
};
#include "ConfigLoader.h"
#include <fstream>
#include <iostream>

ConfigLoader::ConfigLoader() {
}

ConfigLoader::~ConfigLoader() {
}

bool ConfigLoader::loadConfig(const std::string& filename, GameConfig& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        config = getDefaultConfig();
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "musicVolume") {
                config.musicVolume = std::stof(value);
            } else if (key == "sfxVolume") {
                config.sfxVolume = std::stof(value);
            } else if (key == "fullscreen") {
                config.fullscreen = (value == "true");
            } else if (key == "screenWidth") {
                config.screenWidth = std::stoi(value);
            } else if (key == "screenHeight") {
                config.screenHeight = std::stoi(value);
            } else if (key == "vsync") {
                config.vsync = (value == "true");
            }
        }
    }
    
    return true;
}

bool ConfigLoader::saveConfig(const std::string& filename, const GameConfig& config) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "musicVolume=" << config.musicVolume << "\n";
    file << "sfxVolume=" << config.sfxVolume << "\n";
    file << "fullscreen=" << (config.fullscreen ? "true" : "false") << "\n";
    file << "screenWidth=" << config.screenWidth << "\n";
    file << "screenHeight=" << config.screenHeight << "\n";
    file << "vsync=" << (config.vsync ? "true" : "false") << "\n";
    
    return true;
}

GameConfig ConfigLoader::getDefaultConfig() {
    return GameConfig{};
}
#pragma once

#include "Types.h"
#include <string>
#include <functional>

struct GameSave {
    int currentWorld = 1;
    int currentLevel = 1;
    int score = 0;
    int lives = 3;
    int coins = 0;
    bool levels[4][4] = {{false}};
    
    float musicVolume = 0.7f;
    float sfxVolume = 0.8f;
};

class SaveManager {
public:
    SaveManager();
    ~SaveManager();
    
    bool initialize();
    
    void loadGame(std::function<void(const GameSave&)> callback);
    void saveGame(const GameSave& save, std::function<void(bool)> callback = nullptr);
    
    void loadSettings(std::function<void(float, float)> callback);
    void saveSettings(float musicVolume, float sfxVolume, std::function<void(bool)> callback = nullptr);
    
    void syncFileSystem();

private:
    void mountFileSystem();
    void loadGameSync(GameSave& save);
    void saveGameSync(const GameSave& save);
    
    bool m_initialized;
};
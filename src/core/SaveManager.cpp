#include "SaveManager.h"
#include "Constants.h"
#include <fstream>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

extern "C" {
    void syncfs_callback(void* arg) {
        auto callback = static_cast<std::function<void()>*>(arg);
        if (callback) {
            (*callback)();
            delete callback;
        }
    }
}
#endif

SaveManager::SaveManager()
    : m_initialized(false)
{
}

SaveManager::~SaveManager() {
}

bool SaveManager::initialize() {
#ifdef __EMSCRIPTEN__
    mountFileSystem();
#endif
    m_initialized = true;
    return true;
}

void SaveManager::loadGame(std::function<void(const GameSave&)> callback) {
    GameSave save;
    loadGameSync(save);
    callback(save);
}

void SaveManager::saveGame(const GameSave& save, std::function<void(bool)> callback) {
    saveGameSync(save);
    syncFileSystem();
    if (callback) callback(true);
}

void SaveManager::loadSettings(std::function<void(float, float)> callback) {
    float musicVolume = Constants::DEFAULT_MUSIC_VOLUME;
    float sfxVolume = Constants::DEFAULT_SFX_VOLUME;
    
    std::ifstream file("/data/settings.txt");
    if (file.is_open()) {
        file >> musicVolume >> sfxVolume;
        file.close();
    }
    
    callback(musicVolume, sfxVolume);
}

void SaveManager::saveSettings(float musicVolume, float sfxVolume, std::function<void(bool)> callback) {
    std::ofstream file("/data/settings.txt");
    if (file.is_open()) {
        file << musicVolume << " " << sfxVolume;
        file.close();
        
        if (callback) callback(true);
    } else {
        if (callback) callback(false);
    }
    
    syncFileSystem();
}

void SaveManager::syncFileSystem() {
#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.syncfs(function (err) {
            if (err) {
                console.error('Error syncing filesystem:', err);
            }
        });
    );
#endif
}

void SaveManager::mountFileSystem() {
#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.mkdir('/data');
        FS.mount(IDBFS, {}, '/data');
        FS.syncfs(true, function (err) {
            if (err) {
                console.error('Error mounting IDBFS:', err);
            } else {
                console.log('IDBFS mounted successfully');
            }
        });
    );
#endif
}

void SaveManager::loadGameSync(GameSave& save) {
    std::ifstream file("/data/save.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&save), sizeof(GameSave));
        file.close();
    }
}

void SaveManager::saveGameSync(const GameSave& save) {
    std::ofstream file("/data/save.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&save), sizeof(GameSave));
        file.close();
    }
}
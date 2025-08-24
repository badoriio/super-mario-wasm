#pragma once

#include "Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL_mixer.h>
#else
#include <SDL2/SDL_mixer.h>
#endif
#include <unordered_map>
#include <string>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    bool initialize();
    void cleanup();
    
    void playSound(const std::string& name);
    void playMusic(const std::string& name, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    
    void setSFXVolume(float volume);
    void setMusicVolume(float volume);
    
    float getSFXVolume() const { return m_sfxVolume; }
    float getMusicVolume() const { return m_musicVolume; }
    
    bool isMusicPlaying() const;
    bool isMusicPaused() const;

private:
    Mix_Chunk* loadSound(const std::string& path);
    Mix_Music* loadMusic(const std::string& path);
    
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;
    std::unordered_map<std::string, Mix_Music*> m_music;
    
    float m_sfxVolume;
    float m_musicVolume;
    
    std::string m_currentMusic;
};
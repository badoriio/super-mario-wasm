#include "AudioManager.h"
#include "Constants.h"
#include <iostream>

AudioManager::AudioManager()
    : m_sfxVolume(Constants::DEFAULT_SFX_VOLUME)
    , m_musicVolume(Constants::DEFAULT_MUSIC_VOLUME)
{
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::initialize() {
    if (Mix_OpenAudio(Constants::AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, Constants::AUDIO_CHANNELS, Constants::AUDIO_BUFFER_SIZE) < 0) {
        std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }
    
    Mix_AllocateChannels(32);
    
    setSFXVolume(m_sfxVolume);
    setMusicVolume(m_musicVolume);
    
    return true;
}

void AudioManager::cleanup() {
    for (auto& pair : m_sounds) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    m_sounds.clear();
    
    for (auto& pair : m_music) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    m_music.clear();
    
    Mix_CloseAudio();
}

void AudioManager::playSound(const std::string& name) {
    Mix_Chunk* sound = nullptr;
    
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        sound = it->second;
    } else {
        sound = loadSound("sounds/" + name + ".ogg");
        if (!sound) {
            sound = loadSound("sounds/" + name + ".wav");
        }
    }
    
    if (sound) {
        Mix_PlayChannel(-1, sound, 0);
    }
}

void AudioManager::playMusic(const std::string& name, bool loop) {
    if (m_currentMusic == name && isMusicPlaying()) {
        return;
    }
    
    Mix_Music* music = nullptr;
    
    auto it = m_music.find(name);
    if (it != m_music.end()) {
        music = it->second;
    } else {
        music = loadMusic("music/" + name + ".ogg");
        if (!music) {
            music = loadMusic("music/" + name + ".mp3");
        }
    }
    
    if (music) {
        Mix_PlayMusic(music, loop ? -1 : 0);
        m_currentMusic = name;
    }
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
    m_currentMusic.clear();
}

void AudioManager::pauseMusic() {
    Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
    Mix_ResumeMusic();
}

void AudioManager::setSFXVolume(float volume) {
    m_sfxVolume = std::max(0.0f, std::min(1.0f, volume));
    Mix_Volume(-1, static_cast<int>(m_sfxVolume * MIX_MAX_VOLUME));
}

void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = std::max(0.0f, std::min(1.0f, volume));
    Mix_VolumeMusic(static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
}

bool AudioManager::isMusicPlaying() const {
    return Mix_PlayingMusic() && !Mix_PausedMusic();
}

bool AudioManager::isMusicPaused() const {
    return Mix_PausedMusic();
}

Mix_Chunk* AudioManager::loadSound(const std::string& path) {
    std::string fullPath = std::string(Constants::ASSETS_PATH) + path;
    
    Mix_Chunk* sound = Mix_LoadWAV(fullPath.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound " << fullPath << ": " << Mix_GetError() << std::endl;
        return nullptr;
    }
    
    m_sounds[path] = sound;
    return sound;
}

Mix_Music* AudioManager::loadMusic(const std::string& path) {
    std::string fullPath = std::string(Constants::ASSETS_PATH) + path;
    
    Mix_Music* music = Mix_LoadMUS(fullPath.c_str());
    if (!music) {
        std::cerr << "Failed to load music " << fullPath << ": " << Mix_GetError() << std::endl;
        return nullptr;
    }
    
    m_music[path] = music;
    return music;
}
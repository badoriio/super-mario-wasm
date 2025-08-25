#pragma once

namespace Constants {
    // Display settings
    constexpr int SCREEN_WIDTH = 1024;
    constexpr int SCREEN_HEIGHT = 576;
    constexpr float ASPECT_RATIO = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
    
    // Game world settings
    constexpr int TILE_SIZE = 32;
    constexpr int TILES_PER_SCREEN_X = SCREEN_WIDTH / TILE_SIZE;
    constexpr int TILES_PER_SCREEN_Y = SCREEN_HEIGHT / TILE_SIZE;
    
    // Physics constants
    constexpr float GRAVITY = 980.0f;
    constexpr float MAX_FALL_SPEED = 500.0f;
    constexpr float GROUND_FRICTION = 0.92f;  // Reduced friction for better movement
    constexpr float AIR_RESISTANCE = 0.99f;   // Slight air resistance
    
    // Player movement constants
    constexpr float PLAYER_MOVE_SPEED = 300.0f;      // Increased base speed
    constexpr float PLAYER_MAX_SPEED = 350.0f;       // Higher max speed
    constexpr float PLAYER_JUMP_SPEED = 450.0f;      // Slightly higher jump
    constexpr float PLAYER_ACCELERATION = 1200.0f;   // Faster acceleration for responsive feel
    constexpr float PLAYER_DECELERATION = 1200.0f;
    constexpr float PLAYER_SIZE = 32.0f;
    
    // Gameplay constants
    constexpr float COYOTE_TIME = 0.15f;
    constexpr float JUMP_BUFFER_TIME = 0.1f;
    constexpr float INVINCIBILITY_TIME = 2.0f;
    constexpr int MAX_LIVES = 3;
    constexpr int LEVEL_TIME = 300; // seconds
    
    // Enemy constants
    constexpr float GOOMBA_SPEED = 50.0f;
    constexpr float KOOPA_SPEED = 75.0f;
    constexpr float ENEMY_SIZE = 32.0f;
    
    // Animation constants
    constexpr float PLAYER_IDLE_FRAME_TIME = 1.0f;
    constexpr float PLAYER_RUN_FRAME_TIME = 0.1f;
    constexpr float ENEMY_FRAME_TIME = 0.3f;
    constexpr float COIN_FRAME_TIME = 0.2f;
    
    // Camera constants
    constexpr float CAMERA_DEADZONE_X = 200.0f;
    constexpr float CAMERA_DEADZONE_Y = 100.0f;
    constexpr float CAMERA_FOLLOW_SPEED = 5.0f;
    constexpr float CAMERA_LOOK_AHEAD = 150.0f;
    
    // Audio constants
    constexpr int AUDIO_FREQUENCY = 44100;
    constexpr int AUDIO_CHANNELS = 2;
    constexpr int AUDIO_BUFFER_SIZE = 2048;
    constexpr float DEFAULT_MUSIC_VOLUME = 0.7f;
    constexpr float DEFAULT_SFX_VOLUME = 0.8f;
    
    // File paths
    const char* const CONFIG_FILE = "/data/config.json";
    const char* const SAVE_FILE = "/data/save.json";
    const char* const ASSETS_PATH = "/assets/";
    
    // Level constants
    constexpr int MAX_WORLD = 4;
    constexpr int MAX_LEVEL = 4;
    
    // UI constants
    constexpr int HUD_HEIGHT = 64;
    constexpr int MENU_ITEM_HEIGHT = 48;
    constexpr int MENU_PADDING = 32;
}
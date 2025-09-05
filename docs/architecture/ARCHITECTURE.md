# Super Mario WebAssembly - Architecture Overview

## Project Structure

The Super Mario WebAssembly game follows a modular, component-based architecture designed for maintainability, performance, and cross-platform compatibility.

```
super-mario-wasm/
├── include/mario/           # Public API headers organized by module
│   ├── core/               # Core engine components
│   ├── engine/             # Rendering and graphics engine
│   ├── game/               # Game logic and mechanics
│   ├── physics/            # Physics simulation
│   ├── ui/                 # User interface components
│   └── data/               # Data loading and management
├── src/                    # Implementation files (mirrors include structure)
├── assets/                 # Game resources (sprites, audio, levels)
├── web/                    # Web deployment files
├── docs/                   # Project documentation
├── scripts/                # Build and utility scripts
├── tools/                  # Development tools and asset generators
└── external/               # Third-party dependencies (emsdk)
```

## Core Architecture

### Module Organization

#### Core Module (`mario/core/`)
- **Engine.h/cpp**: Main engine class, manages lifecycle and subsystems
- **Types.h/cpp**: Common type definitions and smart pointers
- **Constants.h**: Game-wide constants and configuration
- **ResourceManager.h/cpp**: Asset loading and caching system
- **AudioManager.h/cpp**: Audio playback and sound management
- **InputManager.h/cpp**: Input handling and key mapping
- **SaveManager.h/cpp**: Save/load game state functionality

#### Engine Module (`mario/engine/`)
- **Renderer.h/cpp**: OpenGL/WebGL rendering abstraction
- **Camera.h/cpp**: 2D camera system with smooth following
- **Animation.h/cpp**: Sprite animation system
- **SpriteSheet.h/cpp**: Texture atlas management

#### Game Module (`mario/game/`)
- **Game.h/cpp**: Main game state machine and logic
- **Player.h/cpp**: Mario character controller and abilities
- **Level.h/cpp**: Level data and tile-based world representation

#### Physics Module (`mario/physics/`)
- **PhysicsWorld.h/cpp**: 2D physics simulation
- **CollisionDetector.h/cpp**: AABB and pixel-perfect collision detection

#### UI Module (`mario/ui/`)
- **UI.h/cpp**: Base UI system and state management
- **HUD.h/cpp**: In-game heads-up display (score, lives, timer)
- **Menu.h/cpp**: Menu screens and navigation

#### Data Module (`mario/data/`)
- **LevelLoader.h/cpp**: Level file parsing and world loading
- **ConfigLoader.h/cpp**: Configuration file management

## System Interactions

### Initialization Flow
1. **main.cpp** creates Engine instance
2. **Engine** initializes all subsystems (ResourceManager, AudioManager, etc.)
3. **Game** object is created and initialized
4. Game loop begins (Emscripten for web, native loop for desktop)

### Game Loop Architecture
```
Engine::run() {
    while (running) {
        handleEvents()    // Process input via InputManager
        update(dt)        // Update game logic via Game::update()
        render()          // Render via Renderer and Camera
    }
}
```

### State Management
- **GameState enum**: Tracks current game state (Menu, Playing, Paused, GameOver, etc.)
- **Game class**: Manages state transitions and delegates to appropriate update methods
- **SaveManager**: Persists game progress using browser LocalStorage (web) or files (desktop)

## Design Principles

### 1. Separation of Concerns
- Each module has a single, well-defined responsibility
- Clear interfaces between systems prevent tight coupling
- Headers contain only declarations; implementation is in .cpp files

### 2. Resource Management
- RAII principles with smart pointers (`UniquePtr<T>`, `Ptr<T>`)
- Centralized resource loading through ResourceManager
- Automatic cleanup and memory management

### 3. Cross-Platform Compatibility
- Conditional compilation for web (`__EMSCRIPTEN__`) and desktop
- SDL2 abstraction layer for graphics, input, and audio
- WebGL 2.0 for web rendering, OpenGL for desktop

### 4. Performance Optimization
- Fixed timestep physics (60 FPS)
- Delta time accumulation for smooth rendering
- Efficient sprite batching and texture atlasing
- WebAssembly optimization flags for web deployment

### 5. Maintainability
- Consistent naming conventions (PascalCase for classes, camelCase for methods)
- Modular header organization with namespace prefixes (`<mario/module/Header.h>`)
- Comprehensive documentation and clear code structure

## Build System

### CMake Configuration
- Modern CMake (3.15+) with proper target-based configuration
- Separate include directories from source directories
- Emscripten-specific build flags and optimization settings
- Automated version generation from git hash

### Web Build Process
1. **scripts/build-web.sh**: One-click build script
2. **Emscripten SDK**: Downloaded to `external/emsdk/`
3. **CMake**: Configures build with Emscripten toolchain
4. **Output**: WebAssembly module with HTML/JS wrapper

## Testing and Validation

### Development Workflow
1. Make changes to source code
2. Run `scripts/build-web.sh` to build for web
3. Test in browser with local server
4. Deploy to GitHub Pages or other web host

### Quality Assurance
- Compiler warnings enabled (`-Wall -Wextra`)
- Modern C++17 features with backward compatibility
- Cross-browser testing (Chrome, Firefox, Safari, Edge)

## Future Considerations

### Planned Improvements
- Entity-Component-System (ECS) architecture for scalability
- Advanced physics with slopes and dynamic objects
- Level editor and custom level support
- Multiplayer capabilities
- Mobile touch input support

### Technical Debt
- Gradual migration from raw pointers to smart pointers
- Addition of unit tests and automated testing
- Performance profiling and optimization
- Asset pipeline improvements

This architecture provides a solid foundation for the Super Mario WebAssembly game while maintaining flexibility for future enhancements and modifications.
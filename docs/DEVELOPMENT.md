# Development Guide

## Quick Start

### Prerequisites
- Git
- CMake 3.15+
- Python 3.x (for local server)
- Modern C++ compiler (for desktop builds)

### Building for Web
```bash
# One-click build and serve
./scripts/build-web.sh --serve

# Or build without serving
./scripts/build-web.sh
```

### Project Structure Overview
```
super-mario-wasm/
├── include/mario/          # Public headers organized by module
├── src/                    # Implementation files
├── assets/                 # Game assets
├── docs/                   # Documentation
├── scripts/                # Build scripts
├── tools/                  # Development tools
└── external/               # Third-party dependencies
```

## Development Workflow

### 1. Making Code Changes
- Edit files in `src/` directory
- Headers are in `include/mario/` organized by module
- Use consistent include paths: `<mario/module/Header.h>`

### 2. Building and Testing
```bash
# Build for web
./scripts/build-web.sh

# Test locally
cd build-web
python3 -m http.server 8080
# Open http://localhost:8080/game.html
```

### 3. Adding New Components
1. Create header in appropriate `include/mario/module/` directory
2. Create implementation in corresponding `src/module/` directory  
3. Add files to `CMakeLists.txt` SOURCES and HEADERS lists
4. Update includes to use `<mario/module/Header.h>` format

## Code Standards

### Naming Conventions
- **Classes**: PascalCase (`GameEngine`, `PhysicsWorld`)
- **Methods/Functions**: camelCase (`update()`, `handleInput()`)
- **Member Variables**: m_prefix (`m_position`, `m_isRunning`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_VELOCITY`, `SCREEN_WIDTH`)

### Include Style
```cpp
// System includes first
#include <iostream>
#include <memory>

// Third-party includes
#include <SDL.h>

// Project includes with full namespace
#include <mario/core/Engine.h>
#include <mario/game/Player.h>
```

### Memory Management
- Use smart pointers: `UniquePtr<T>`, `Ptr<T>` (defined in Types.h)
- RAII principles for resource management
- Avoid raw pointers except for non-owning references

## Build Configuration

### CMake Structure
- Headers listed in `HEADERS` variable
- Source files in `SOURCES` variable
- Include directory: `include/`
- Emscripten-specific flags for web builds

### Adding Dependencies
1. Place in `external/` directory
2. Update CMake configuration
3. Update build scripts if needed

## Debugging

### Web Debugging
- Use browser developer tools
- Console logging via `printf()` or `std::cout`
- WebAssembly debugging with source maps

### Common Issues
- **Include path errors**: Ensure using `<mario/module/Header.h>` format
- **Emscripten not found**: Check `external/emsdk/` installation
- **Build failures**: Verify CMake configuration and file paths

## Asset Management

### Adding Assets
1. Place files in `assets/` directory
2. Update CMake preload configuration if needed
3. Load via ResourceManager in code

### Asset Tools
- `tools/asset_tools/create_simple_assets.py`: Generate placeholder assets
- `tools/asset_tools/create_audio.py`: Generate audio files

## Contributing

### Before Committing
1. Test build with `./scripts/build-web.sh`
2. Verify game runs in browser
3. Check for compiler warnings
4. Update documentation if needed

### Code Review
- Follow existing code style and patterns
- Ensure proper error handling
- Maintain performance considerations
- Update tests and documentation
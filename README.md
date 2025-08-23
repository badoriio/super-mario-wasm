# 🍄 Super Mario WebAssembly Game

A lightweight Super Mario-style platformer built with C++, SDL2, and compiled to WebAssembly for web deployment.

## ✨ Features

- **Mario Character**: Physics-based movement with gravity, jumping, and state management
- **Platform Levels**: Multiple floating platforms with collision detection  
- **Responsive Controls**: Keyboard input (Arrow keys, WASD, Spacebar)
- **Web-Ready**: Runs in any modern web browser via WebAssembly
- **Optimized Rendering**: SDL2-powered graphics with 60 FPS gameplay

## 🎮 Controls

| Action | Keys |
|--------|------|
| **Move Left/Right** | `←→` Arrow keys or `A/D` |
| **Jump** | `Spacebar`, `↑` Up Arrow, or `W` |

Navigate through the platforms to explore the level!

## 🚀 Quick Start

### 🌐 Play Online
**[🎮 Play on GitHub Pages →](https://yourusername.github.io/super-mario-wasm/)**

### 🛠️ Local Development

#### Prerequisites
- [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html)
- CMake 3.10+

#### Build & Run
```bash
# Build the game
./build.sh

# Serve locally (choose any available port)
cd build/web
python3 -m http.server 8080

# Open in browser
open http://localhost:8080/super_mario.html
```

### 🚀 Deploy to GitHub Pages
See [DEPLOYMENT.md](DEPLOYMENT.md) for complete deployment guide.

## 📁 Project Structure

```
SuperMarioWasm/
├── CMakeLists.txt        # Build configuration
├── build.sh             # Build script
├── shell.html           # WebAssembly HTML template
├── src/                 # Source code
│   ├── main.cpp        # Entry point & WebAssembly integration
│   ├── game.cpp        # Game engine & main loop
│   ├── mario.cpp       # Mario character implementation
│   ├── level.cpp       # Level system & platforms
│   └── physics.cpp     # Physics utilities
└── include/            # Header files
    ├── game.h
    ├── mario.h
    ├── level.h
    └── physics.h
```

## 🔧 Technical Details

- **Language**: C++17
- **Graphics**: SDL2 (included with Emscripten)
- **Physics**: Custom 2D physics with AABB collision detection
- **Build System**: CMake with Emscripten toolchain
- **Output**: WebAssembly (WASM) + JavaScript + HTML

## 🎯 Game Mechanics

**Mario States**:
- 🔴 **Idle**: Red (standing still)
- 🟠 **Running**: Light red (moving)
- 🟡 **Jumping**: Yellow (ascending)
- 🟠 **Falling**: Orange (descending)

**Level Features**:
- Ground platform for base navigation
- Multiple floating platforms at various heights
- Stair-like structures for progressive climbing
- Collision detection with proper landing mechanics

---

*Built with ❤️ using C++, SDL2, and WebAssembly*
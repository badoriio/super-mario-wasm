cmake_minimum_required(VERSION 3.15)
project(SuperMarioWasm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/core/Engine.cpp
    src/core/ResourceManager.cpp
    src/core/AudioManager.cpp
    src/core/InputManager.cpp
    src/core/SaveManager.cpp
    src/core/Types.cpp
    src/engine/Renderer.cpp
    src/engine/Camera.cpp
    src/engine/Animation.cpp
    src/engine/SpriteSheet.cpp
    src/physics/PhysicsWorld.cpp
    src/physics/CollisionDetector.cpp
    src/game/Game.cpp
    src/game/Player.cpp
    src/game/Level.cpp
    src/ui/UI.cpp
    src/ui/Menu.cpp
    src/ui/HUD.cpp
    src/data/LevelLoader.cpp
    src/data/ConfigLoader.cpp
)

# Header files
set(HEADERS
    include/mario/core/Engine.h
    include/mario/core/ResourceManager.h
    include/mario/core/AudioManager.h
    include/mario/core/InputManager.h
    include/mario/core/SaveManager.h
    include/mario/core/Types.h
    include/mario/core/Constants.h
    include/mario/engine/Renderer.h
    include/mario/engine/Camera.h
    include/mario/engine/Animation.h
    include/mario/engine/SpriteSheet.h
    include/mario/physics/PhysicsWorld.h
    include/mario/physics/CollisionDetector.h
    include/mario/game/Game.h
    include/mario/game/Player.h
    include/mario/game/Level.h
    include/mario/ui/UI.h
    include/mario/ui/Menu.h
    include/mario/ui/HUD.h
    include/mario/data/LevelLoader.h
    include/mario/data/ConfigLoader.h
)

# Add executable
add_executable(game ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(game PRIVATE include)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(game PRIVATE -Wall -Wextra -O3)
endif()

# Web build configuration (Emscripten only)
set_target_properties(game PROPERTIES
    SUFFIX ".html"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build-web
)

# Emscripten compile flags
target_compile_options(game PRIVATE
    -O3
    -DWEB_BUILD
    -sUSE_SDL=2
    -sUSE_SDL_IMAGE=2
    -sUSE_SDL_MIXER=2
)

# Emscripten link flags
target_link_options(game PRIVATE
    -O3
    "SHELL:-s WASM=1"
    "SHELL:-s ALLOW_MEMORY_GROWTH=1"
    "SHELL:-s USE_SDL=2"
    "SHELL:-s USE_SDL_IMAGE=2"
    "SHELL:-s SDL2_IMAGE_FORMATS=[\"png\"]"
    "SHELL:-s USE_SDL_MIXER=2"
    "SHELL:-s SDL2_MIXER_FORMATS=[\"ogg\",\"mp3\"]"
    "SHELL:-s MIN_WEBGL_VERSION=2"
    "SHELL:-s MAX_WEBGL_VERSION=2"
    "SHELL:-s FORCE_FILESYSTEM=1"
    "SHELL:-s ASSERTIONS=0"
    "SHELL:-s ASYNCIFY"
    "SHELL:-s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\"]"
    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/assets@/assets"
    "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/web/shell.html"
    "SHELL:-lidbfs.js"
)

# Create version file
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT GIT_HASH)
    set(GIT_HASH "unknown")
endif()

configure_file(
    ${CMAKE_SOURCE_DIR}/include/mario/core/Version.h.in
    ${CMAKE_BINARY_DIR}/include/mario/core/Version.h
    @ONLY
)

target_include_directories(game PRIVATE ${CMAKE_BINARY_DIR})
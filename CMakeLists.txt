cmake_minimum_required(VERSION 3.10)
project(SuperMarioWasm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(super_mario
    src/main.cpp
    src/game.cpp
    src/mario.cpp
    src/level.cpp
    src/physics.cpp
)

# Include directories
target_include_directories(super_mario PRIVATE include)

# Set output directory
set_target_properties(super_mario PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/web
)

# Emscripten-specific settings
if(EMSCRIPTEN)
    set_target_properties(super_mario PROPERTIES
        SUFFIX ".html"
    )
    
    target_compile_options(super_mario PRIVATE
        "SHELL:-s USE_SDL=2"
    )
    
    target_link_options(super_mario PRIVATE
        "SHELL:-s USE_SDL=2"
        "SHELL:-s WASM=1"
        "SHELL:-s ASYNCIFY"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/shell.html"
        -O3
    )
endif()
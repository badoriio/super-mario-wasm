#!/bin/bash

# Super Mario WebAssembly Build Script

echo "Building Super Mario WebAssembly Game..."

# Create build directory
mkdir -p build
cd build

# Check if emcmake is available
if ! command -v emcmake &> /dev/null; then
    echo "Error: Emscripten not found. Please install Emscripten first."
    echo "Installation guide: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

# Configure with CMake and Emscripten
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
emmake make

echo "Build complete! Check the build/web directory for the generated files."
echo "To run the game, serve the files from build/web with a web server."
echo ""
echo "To run the game:"
echo "cd build/web && python3 -m http.server 8080"
echo "Then open http://localhost:8080/super_mario.html"
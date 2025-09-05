#!/bin/bash
# build-web.sh - One-click web build script for Super Mario WebAssembly
# Usage: ./build-web.sh [--serve]

set -e

SERVE_AFTER_BUILD=false

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --serve|-s)
            SERVE_AFTER_BUILD=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [--serve|-s] [--help|-h]"
            echo ""
            echo "Options:"
            echo "  --serve, -s    Start local server after build"
            echo "  --help, -h     Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "🍄 Super Mario WebAssembly - Web Build Script"
echo "=============================================="

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ Error: CMakeLists.txt not found. Please run this script from the project root."
    exit 1
fi

echo "🔧 Setting up Emscripten SDK..."

# Setup Emscripten SDK
if [ ! -d "external/emsdk" ]; then
    echo "📥 Cloning Emscripten SDK..."
    mkdir -p external
    git clone https://github.com/emscripten-core/emsdk.git external/emsdk
    cd external/emsdk
    echo "⬇️ Installing latest Emscripten..."
    ./emsdk install latest
    echo "⚡ Activating Emscripten..."
    ./emsdk activate latest
    cd ../..
    echo "✅ Emscripten SDK installed successfully"
else
    echo "✅ Emscripten SDK already exists"
fi

# Activate Emscripten environment
echo "🌍 Activating Emscripten environment..."
source external/emsdk/emsdk_env.sh

# Verify Emscripten is working
echo "🔍 Verifying Emscripten installation..."
emcc --version | head -1 || {
    echo "❌ Error: Emscripten not properly installed"
    exit 1
}

echo "🏗️ Configuring build..."

# Configure CMake for web build
cmake -S . -B build-web \
    -DCMAKE_TOOLCHAIN_FILE=$PWD/external/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG" \
    -DEMSCRIPTEN=1

echo "⚙️ Building game..."

# Build the project
cmake --build build-web --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Build completed successfully!"
    echo ""
    echo "📁 Build outputs:"
    ls -lh build-web/*.html build-web/*.js build-web/*.wasm build-web/*.data 2>/dev/null | while read line; do
        echo "   $line"
    done
    echo ""
    echo "🎮 Game files ready for deployment!"
    echo ""
    
    if [ "$SERVE_AFTER_BUILD" = true ]; then
        echo "🌐 Starting local server..."
        echo "   Game will be available at: http://localhost:8080/game.html"
        echo "   Press Ctrl+C to stop the server"
        echo ""
        cd build-web
        
        # Try different Python commands
        if command -v python3 >/dev/null 2>&1; then
            python3 -m http.server 8080
        elif command -v python >/dev/null 2>&1; then
            python -m http.server 8080
        else
            echo "❌ Error: Python not found. Cannot start local server."
            echo "💡 Manually serve files with: cd build-web && python -m http.server 8080"
            exit 1
        fi
    else
        echo "💡 To test locally:"
        echo "   cd build-web && python -m http.server 8080"
        echo "   Then open: http://localhost:8080/game.html"
        echo ""
        echo "🚀 To serve with --serve flag next time:"
        echo "   ./build-web.sh --serve"
    fi
else
    echo "❌ Build failed!"
    exit 1
fi
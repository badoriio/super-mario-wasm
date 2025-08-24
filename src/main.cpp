#include "core/Engine.h"
#include "game/Game.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

static Engine* g_main_engine = nullptr;

void main_loop() {
    static int frameCount = 0;
    frameCount++;
    if (frameCount == 1) {
        printf("=== FIRST EMSCRIPTEN LOOP CALL ===\n");
    }
    if (g_main_engine && g_main_engine->isRunning()) {
        g_main_engine->run();
    }
}
#endif

int main(int argc, char* argv[]) {
    printf("=== MAIN() STARTING ===\n");
    Engine engine;
    
    printf("=== CREATING GAME OBJECT ===\n");
    auto game = MakeShared<Game>();
    
    printf("=== INITIALIZING ENGINE ===\n");
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    printf("=== INITIALIZING GAME ===\n");
    if (!game->initialize(&engine)) {
        printf("=== GAME INITIALIZATION FAILED ===\n");
        std::cerr << "Failed to initialize game!" << std::endl;
        engine.shutdown();
        return -1;
    }
    
    printf("=== SETTING GAME IN ENGINE ===\n");
    engine.setGame(game);
    
#ifdef __EMSCRIPTEN__
    // For Emscripten, set up the main loop after everything is initialized
    printf("=== SETTING UP EMSCRIPTEN MAIN LOOP ===\n");
    g_main_engine = &engine;
    emscripten_set_main_loop(main_loop, 0, 1);
    printf("=== MAIN() COMPLETE - EMSCRIPTEN LOOP RUNNING ===\n");
    return 0;
#else
    // For desktop builds, run the main loop
    engine.run();
    
    game->shutdown();
    engine.shutdown();
    
    return 0;
#endif
}
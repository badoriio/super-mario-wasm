#include "game.h"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>

Game* g_game = nullptr;

void gameLoop() {
    if (g_game) {
        g_game->run();
    }
}
#endif

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }
    
#ifdef __EMSCRIPTEN__
    g_game = &game;
    emscripten_set_main_loop(gameLoop, 60, 1);
#else
    game.run();
#endif
    
    game.cleanup();
    return 0;
}
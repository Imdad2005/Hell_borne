#include "src/core/GameEngine.h"
#include <SDL_log.h>

int main(int argc, char* argv[]) {
    SDL_Log("=== Hellborne - Dead Cells Style Roguelike ===");
    SDL_Log("Initializing...");

    GameEngine engine;
    
    if (!engine.init()) {
        SDL_Log("Failed to initialize game engine!");
        return 1;
    }

    SDL_Log("Starting game loop...");
    engine.run();

    SDL_Log("Shutting down...");
    return 0;
}

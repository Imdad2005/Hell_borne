#include "game.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    Game game;

    if (!game.init()) {
        return 1;
    }

    game.run();
    game.cleanup();

    return 0;
}

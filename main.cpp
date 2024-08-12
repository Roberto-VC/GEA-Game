#define SDL_MAIN_HANDLED
#include "Scene/DemoGame/DemoGame.h"

int main() {
    DemoGame demo = DemoGame();
    demo.setup();
    demo.run();
    return 0;
}
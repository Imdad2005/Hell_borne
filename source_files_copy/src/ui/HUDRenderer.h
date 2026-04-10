#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include <SDL.h>

class HUDRenderer {
public:
    HUDRenderer();
    ~HUDRenderer();

    void renderPlayingHUD(SDL_Renderer* renderer, float healthPercent, int enemyCount, int hellgold, int floor) const;
    void renderGameOverOverlay(SDL_Renderer* renderer) const;
};

#endif // HUD_RENDERER_H

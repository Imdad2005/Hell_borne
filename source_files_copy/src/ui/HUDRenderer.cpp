#include "HUDRenderer.h"

HUDRenderer::HUDRenderer() {
}

HUDRenderer::~HUDRenderer() {
}

void HUDRenderer::renderPlayingHUD(SDL_Renderer* renderer, float healthPercent, int enemyCount, int hellgold, int floor) const {
    // Health bar
    SDL_Rect hpBg = {20, 20, 220, 16};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &hpBg);
    SDL_Rect hpFill = {20, 20, static_cast<int>(220 * healthPercent), 16};
    SDL_SetRenderDrawColor(renderer, 200, 40, 40, 255);
    SDL_RenderFillRect(renderer, &hpFill);

    // Enemy count meter (visual proxy)
    SDL_Rect enemyBg = {20, 44, 220, 10};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderFillRect(renderer, &enemyBg);
    int enemyFillW = enemyCount * 60;
    if (enemyFillW > 220) enemyFillW = 220;
    SDL_Rect enemyFill = {20, 44, enemyFillW, 10};
    SDL_SetRenderDrawColor(renderer, 70, 220, 70, 255);
    SDL_RenderFillRect(renderer, &enemyFill);

    // Hellgold and floor as bars/blocks (fontless fallback style)
    SDL_Rect goldBar = {20, 60, (hellgold % 221), 8};
    SDL_SetRenderDrawColor(renderer, 220, 180, 40, 255);
    SDL_RenderFillRect(renderer, &goldBar);

    SDL_Rect floorBar = {20, 72, floor * 20, 8};
    if (floorBar.w > 220) floorBar.w = 220;
    SDL_SetRenderDrawColor(renderer, 90, 140, 255, 255);
    SDL_RenderFillRect(renderer, &floorBar);
}

void HUDRenderer::renderGameOverOverlay(SDL_Renderer* renderer) const {
    SDL_Rect gameOverRect = {440, 260, 400, 200};
    SDL_SetRenderDrawColor(renderer, 150, 0, 0, 255);
    SDL_RenderFillRect(renderer, &gameOverRect);

    SDL_Rect restartRect = {520, 400, 240, 40};
    SDL_SetRenderDrawColor(renderer, 230, 230, 80, 255);
    SDL_RenderFillRect(renderer, &restartRect);
}

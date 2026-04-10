#ifndef GLOBALS_H
#define GLOBALS_H

// Screen dimensions
const int SCREEN_WIDTH  = 1280;
const int SCREEN_HEIGHT = 720;

// Game loop
const int TARGET_FPS = 60;
const int FRAME_MS   = 1000 / TARGET_FPS;

// Physics constants
const float GRAVITY     = 1200.0f; // pixels/s²
const float MAX_FALL_SPEED = 800.0f;

// Tile size
const int TILE_SIZE = 32;

// Latency measurement and debugging (uncomment to enable)
// #define MEASURE_LATENCY 1

// Input-first architecture for low-latency gameplay:
// Priority order: Capture Input -> Update -> Render
// This ensures input is processed before physics and rendering,
// minimizing input-to-pixel latency

// Game states
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_SHOP
};

#endif // GLOBALS_H

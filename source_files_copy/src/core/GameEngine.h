#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <SDL.h>
#include <SDL_image.h>
// SDL_ttf not needed yet - Phase 1 uses colored rectangles
// #include <SDL_ttf.h>
#include "globals.h"
#include "Timer.h"

#ifdef AUDIO_ENABLED
#include "../audio/AudioManager.h"
#else
#include "../audio/AudioStub.h"
#endif

#include "../input/InputHandler.h"
#include "../entities/Player.h"
#include "../entities/EnemySpawner.h"
#include "../graphics/Camera.h"
#include "../ui/HUDRenderer.h"
#include "../persistence/SaveSystem.h"
#include "../world/RoomTemplate.h"
#include <vector>

class GameEngine {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    GameState currentState;
    Timer fpsTimer;
    Timer capTimer;
    int countedFrames;
    AudioManager audioManager;
    InputHandler inputHandler;
    Player player;
    EnemySpawner enemySpawner;
    Camera camera;
    HUDRenderer hudRenderer;
    SaveSystem saveSystem;
    SaveData saveData;
    RoomTemplate currentRoom;
    bool attackRequested;
    std::vector<AABB> worldPlatforms;
    int runKills;

public:
    GameEngine();
    ~GameEngine();

    bool init();
    void cleanup();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void run();

    bool isRunning() const { return running; }
    void quit() { running = false; }
    
    SDL_Renderer* getRenderer() { return renderer; }
    GameState getState() const { return currentState; }
    void setState(GameState state) { currentState = state; }
    AudioManager& getAudioManager() { return audioManager; }
};

#endif // GAME_ENGINE_H

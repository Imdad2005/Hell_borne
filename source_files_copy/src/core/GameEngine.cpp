#include "GameEngine.h"
#include <SDL_log.h>
#include <string>

GameEngine::GameEngine() 
    : window(nullptr), renderer(nullptr), running(false), 
      currentState(STATE_MENU), countedFrames(0), attackRequested(false), runKills(0) {
}

GameEngine::~GameEngine() {
    cleanup();
}

bool GameEngine::init() {
    // QNX: Set OpenGL ES2 hint before SDL_Init
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS
#ifdef AUDIO_ENABLED
                 | SDL_INIT_AUDIO
#endif
    ) < 0) {
        SDL_Log("SDL could not initialize! SDL Error: %s", SDL_GetError());
        return false;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        SDL_Log("SDL_image could not initialize! IMG Error: %s", IMG_GetError());
        // Don't return false - fallback rendering will handle missing images
    }

    // Initialize SDL_ttf (disabled for Phase 1 - not using fonts yet)
    /*
    if (TTF_Init() == -1) {
        SDL_Log("SDL_ttf could not initialize! TTF Error: %s", TTF_GetError());
        // Don't return false - fallback rendering will handle missing fonts
    }
    */

    // Create window
    window = SDL_CreateWindow(
        "Hellborne",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        SDL_Log("Window could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }

    // Create renderer WITHOUT VSync for lower input latency
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );

    if (renderer == nullptr) {
        SDL_Log("Renderer could not be created! SDL Error: %s", SDL_GetError());
        return false;
    }
    
    // QNX-specific hardware acceleration hints
    SDL_SetHint("SDL_QNX_FORCE_SCREEN_ROTATION_0", "1");
    SDL_SetHint("SDL_QNX_SCREEN_USAGE", "0x06000000"); // Hardware acceleration

    // Initialize audio manager
    if (!audioManager.init()) {
        SDL_Log("Audio could not initialize - continuing without audio");
        // Don't return false - game works without audio
    }

    SDL_Log("Hellborne engine initialized successfully");
    SDL_Log("Renderer: %s", SDL_GetCurrentVideoDriver());
    
    // Initialize player
    if (!player.init(renderer)) {
        SDL_Log("Failed to initialize player!");
        return false;
    }
    
    // Spawn player above ground
    player.setPosition(Vector2(400.0f, 300.0f));

    // Load persistent progression
    saveSystem.load(saveData);

    // Build room from template (Phase 7 scaffold)
    currentRoom = RoomTemplateLibrary::createStartRoom();
    worldPlatforms = currentRoom.platforms;

    // Spawn test enemies
    enemySpawner.spawnTestEnemies();
    
    running = true;
    return true;
}

void GameEngine::cleanup() {
    audioManager.shutdown();

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    // TTF_Quit(); // Disabled for Phase 1
    IMG_Quit();
    SDL_Quit();

    SDL_Log("Hellborne engine cleaned up");
}

void GameEngine::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }
        
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == STATE_PLAYING) {
                    currentState = STATE_PAUSED;
                } else if (currentState == STATE_PAUSED) {
                    currentState = STATE_PLAYING;
                } else if (currentState == STATE_MENU) {
                    running = false;
                }
            }
            
            // Press SPACE in menu to start game
            if (e.key.keysym.sym == SDLK_SPACE && currentState == STATE_MENU) {
                currentState = STATE_PLAYING;
            }

            if (currentState == STATE_PLAYING && e.key.keysym.sym == SDLK_j) {
                attackRequested = true;
            }

            if (currentState == STATE_GAMEOVER && e.key.keysym.sym == SDLK_r) {
                // Soft reset run state
                player.resetForNewRun();
                player.setPosition(Vector2(400.0f, 300.0f));
                enemySpawner.clear();
                enemySpawner.spawnTestEnemies();
                runKills = 0;
                saveData.totalRuns += 1;
                saveSystem.save(saveData);
                currentState = STATE_PLAYING;
            }
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (currentState == STATE_PLAYING && e.button.button == SDL_BUTTON_LEFT) {
                attackRequested = true;
            }
        }
    }
    
    // Update input handler state
    inputHandler.update();
}

void GameEngine::update(float deltaTime) {
    // Update game state based on current state
    switch (currentState) {
        case STATE_MENU:
            // TODO: Update menu
            break;
        case STATE_PLAYING:
            {
                // Handle player input
                if (inputHandler.isKeyDown(SDL_SCANCODE_A) || inputHandler.isKeyDown(SDL_SCANCODE_LEFT)) {
                    player.moveLeft();
                } else if (inputHandler.isKeyDown(SDL_SCANCODE_D) || inputHandler.isKeyDown(SDL_SCANCODE_RIGHT)) {
                    player.moveRight();
                } else {
                    player.stopMoving();
                }
                
                // Jump (SPACE or W or UP)
                if (inputHandler.wasKeyPressed(SDL_SCANCODE_SPACE) || 
                    inputHandler.wasKeyPressed(SDL_SCANCODE_W) ||
                    inputHandler.wasKeyPressed(SDL_SCANCODE_UP)) {
                    player.jump();
                }
                
                if (inputHandler.wasKeyReleased(SDL_SCANCODE_SPACE) || 
                    inputHandler.wasKeyReleased(SDL_SCANCODE_W) ||
                    inputHandler.wasKeyReleased(SDL_SCANCODE_UP)) {
                    player.releaseJump();
                }
                
                // Dash (LSHIFT or RSHIFT)
                if (inputHandler.wasKeyPressed(SDL_SCANCODE_LSHIFT) || 
                    inputHandler.wasKeyPressed(SDL_SCANCODE_RSHIFT)) {
                    player.dash();
                }
                
                // Attack (event-driven, avoids edge-detection misses)
                if (attackRequested) {
                    player.attack();
                    attackRequested = false;
                }
                
                // Update player
                player.update(deltaTime);
                
                // Reset grounded state before checking collisions
                player.setGrounded(false);
                
                // Check collision with platforms (hardcoded for Phase 2)
                for (size_t i = 0; i < worldPlatforms.size(); ++i) {
                    player.checkCollision(worldPlatforms[i]);
                }

                // Enemies
                enemySpawner.update(deltaTime, player.getPosition());
                enemySpawner.applyPlatformCollisions(worldPlatforms);

                // Player attacks enemies
                int aliveBefore = enemySpawner.getAliveCount();
                if (player.isCurrentlyAttacking()) {
                    enemySpawner.handlePlayerAttack(player.getAttackHitbox(), player.getAttackDamage());
                }
                int aliveAfter = enemySpawner.getAliveCount();
                if (aliveAfter < aliveBefore) {
                    int killed = aliveBefore - aliveAfter;
                    runKills += killed;
                    saveData.totalKills += killed;
                    saveData.hellgold += 5 * killed;
                }

                // Enemies damage player by contact
                int damage = enemySpawner.handlePlayerContactDamage(player.getHitbox());
                if (damage > 0) {
                    player.takeDamage(damage);
                }

                if (player.isDead()) {
                    saveSystem.save(saveData);
                    currentState = STATE_GAMEOVER;
                }
                
                // Update camera to follow player
                camera.followTarget(player.getPosition());
                camera.update(deltaTime);
            }
            break;
        case STATE_PAUSED:
            // Nothing to update when paused
            break;
        case STATE_GAMEOVER:
            // TODO: Update game over screen
            break;
        case STATE_SHOP:
            // TODO: Update shop
            break;
    }
}

void GameEngine::render() {
    // Clear screen with dark blue/purple background
    SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
    SDL_RenderClear(renderer);

    // Render based on current state
    switch (currentState) {
        case STATE_MENU:
            {
                // Title
                SDL_Rect titleRect = {SCREEN_WIDTH/2 - 200, 150, 400, 80};
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                SDL_RenderFillRect(renderer, &titleRect);
                
                // "Press SPACE to Start" indicator
                SDL_Rect startRect = {SCREEN_WIDTH/2 - 150, 350, 300, 50};
                SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
                SDL_RenderFillRect(renderer, &startRect);
            }
            break;
            
        case STATE_PLAYING:
            {
                // Apply camera offset to all world rendering
                int camX = static_cast<int>(camera.getX());
                int camY = static_cast<int>(camera.getY());
                
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                for (size_t i = 0; i < worldPlatforms.size(); ++i) {
                    SDL_Rect platformRect = {
                        static_cast<int>(worldPlatforms[i].x - camX),
                        static_cast<int>(worldPlatforms[i].y - camY),
                        static_cast<int>(worldPlatforms[i].w),
                        static_cast<int>(worldPlatforms[i].h)
                    };
                    SDL_RenderFillRect(renderer, &platformRect);
                }
                
                // Render player with camera offset
                player.render(renderer, camera.getX(), camera.getY());
                enemySpawner.render(renderer, camera.getX(), camera.getY());

                // Debug attack hitbox visualization
                if (player.isCurrentlyAttacking()) {
                    AABB atk = player.getAttackHitbox();
                    SDL_Rect atkRect = {
                        static_cast<int>(atk.x - camera.getX()),
                        static_cast<int>(atk.y - camera.getY()),
                        static_cast<int>(atk.w),
                        static_cast<int>(atk.h)
                    };
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 255, 80, 80, 120);
                    SDL_RenderFillRect(renderer, &atkRect);
                    SDL_SetRenderDrawColor(renderer, 255, 30, 30, 255);
                    SDL_RenderDrawRect(renderer, &atkRect);
                }

                hudRenderer.renderPlayingHUD(
                    renderer,
                    player.getHealthPercent(),
                    enemySpawner.getAliveCount(),
                    saveData.hellgold,
                    saveData.highestFloor
                );
            }
            break;
            
        case STATE_PAUSED:
            {
                // Render game world first (same as PLAYING) with camera
                int camX = static_cast<int>(camera.getX());
                int camY = static_cast<int>(camera.getY());
                
                SDL_Rect ground = {-camX, SCREEN_HEIGHT - 100 - camY, SCREEN_WIDTH * 3, 100};
                SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
                SDL_RenderFillRect(renderer, &ground);
                
                SDL_Rect platform1 = {200 - camX, 500 - camY, 200, 32};
                SDL_Rect platform2 = {500 - camX, 400 - camY, 200, 32};
                SDL_Rect platform3 = {100 - camX, 350 - camY, 200, 32};
                SDL_Rect platform4 = {-200 - camX, 450 - camY, 200, 32};
                SDL_Rect platform5 = {800 - camX, 350 - camY, 200, 32};
                SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                SDL_RenderFillRect(renderer, &platform1);
                SDL_RenderFillRect(renderer, &platform2);
                SDL_RenderFillRect(renderer, &platform3);
                SDL_RenderFillRect(renderer, &platform4);
                SDL_RenderFillRect(renderer, &platform5);
                
                // Render player with camera offset
                player.render(renderer, camera.getX(), camera.getY());
                
                // Dark overlay
                SDL_Rect overlay = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
                SDL_RenderFillRect(renderer, &overlay);
                
                // "PAUSED" indicator
                SDL_Rect pausedRect = {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 50, 300, 100};
                SDL_SetRenderDrawColor(renderer, 200, 200, 50, 255);
                SDL_RenderFillRect(renderer, &pausedRect);
            }
            break;
            
        case STATE_GAMEOVER:
            {
                hudRenderer.renderGameOverOverlay(renderer);
            }
            break;
            
        case STATE_SHOP:
            {
                // Shop interface placeholder
                SDL_Rect shopRect = {100, 100, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 200};
                SDL_SetRenderDrawColor(renderer, 50, 100, 50, 255);
                SDL_RenderFillRect(renderer, &shopRect);
            }
            break;
    }

    // Present to screen
    SDL_RenderPresent(renderer);
}

void GameEngine::run() {
    fpsTimer.start();
    
    const float FIXED_DT = 1.0f / TARGET_FPS;
    float accumulator = 0.0f;
    Uint32 currentTime = SDL_GetTicks();

    while (running) {
        // PRIORITY 1: Capture input immediately (input-first architecture)
        handleEvents();
        inputHandler.update();
        
        // PRIORITY 2: Measure frame time and update accumulator
        Uint32 newTime = SDL_GetTicks();
        Uint32 frameTime = newTime - currentTime;
        
        // Cap frame time to prevent spiral of death
        if (frameTime > 250) {
            frameTime = 250;
        }
        
        currentTime = newTime;
        accumulator += frameTime / 1000.0f;

        // PRIORITY 3: Fixed timestep update with captured input
        while (accumulator >= FIXED_DT) {
            update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        // PRIORITY 4: Render (latency from Here to screen limited by VSync)
        render();

        // Frame counting - VSync provides natural frame pacing without input delay
        ++countedFrames;
    }
}

#include "game.h"

#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <SDL2/SDL_image.h>

namespace {
constexpr int PLAYER_SHEET_FRAME_WIDTH = 32;
constexpr int PLAYER_SHEET_FRAME_HEIGHT = 48;
constexpr float PLAYER_IDLE_FRAME_DURATION = 0.10f;
constexpr float PLAYER_RUN_FRAME_DURATION = 0.08f;
constexpr float PLAYER_SHOOT_FRAME_DURATION = 0.06f;

void drawNightBackground(SDL_Renderer* renderer, int width, int height) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 38, 20, 34, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 62, 28, 54, 255);
    SDL_Rect bandTop = {0, 0, width, height / 3};
    SDL_RenderFillRect(renderer, &bandTop);

    SDL_SetRenderDrawColor(renderer, 140, 56, 72, 255);
    SDL_Rect bandMid = {0, height / 3, width, height / 3};
    SDL_RenderFillRect(renderer, &bandMid);

    SDL_SetRenderDrawColor(renderer, 236, 126, 72, 255);
    SDL_Rect bandLow = {0, height * 2 / 3, width, height / 3};
    SDL_RenderFillRect(renderer, &bandLow);

    SDL_SetRenderDrawColor(renderer, 252, 188, 90, 135);
    SDL_Rect sunGlow = {width - 330, 70, 240, 240};
    SDL_RenderFillRect(renderer, &sunGlow);

    SDL_SetRenderDrawColor(renderer, 255, 208, 110, 255);
    SDL_Rect sunCore = {width - 278, 122, 136, 136};
    SDL_RenderFillRect(renderer, &sunCore);

    SDL_SetRenderDrawColor(renderer, 255, 232, 178, 170);
    for (int index = 0; index < 36; ++index) {
        const int cloudX = ((index * 83) % (width + 180)) - 90;
        const int cloudY = 62 + ((index * 39) % (height / 2));
        const int cloudW = 72 + ((index * 13) % 96);
        const int cloudH = 10 + ((index * 7) % 16);
        SDL_Rect cloud = {cloudX, cloudY, cloudW, cloudH};
        SDL_RenderFillRect(renderer, &cloud);
    }

    SDL_SetRenderDrawColor(renderer, 52, 20, 26, 255);
    SDL_Rect horizon = {0, height - 124, width, 124};
    SDL_RenderFillRect(renderer, &horizon);

    SDL_SetRenderDrawColor(renderer, 34, 12, 20, 255);
    SDL_Rect silhouette1 = {0, height - 170, width / 4, 170};
    SDL_Rect silhouette2 = {width / 5, height - 150, width / 3, 150};
    SDL_Rect silhouette3 = {width / 2, height - 190, width / 4, 190};
    SDL_Rect silhouette4 = {width * 3 / 4, height - 160, width / 4, 160};
    SDL_RenderFillRect(renderer, &silhouette1);
    SDL_RenderFillRect(renderer, &silhouette2);
    SDL_RenderFillRect(renderer, &silhouette3);
    SDL_RenderFillRect(renderer, &silhouette4);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
}

Game::Game()
    : window(nullptr), renderer(nullptr), playerIdleTexture(nullptr), playerRunTexture(nullptr), playerShootTexture(nullptr), playerIdleFrameCount(1), playerRunFrameCount(1), playerShootFrameCount(1), playerCurrentFrame(0), playerAnimationTimer(0.0f), playerAnimationState(PlayerAnimationState::Idle), isRunning(false), currentPhase(1), enemiesKilledInPhase(0), bossActive(false), bossSpawnTriggered(false), bossSpawnTimer(0.0f), cameraX(0.0f), isPaused(false), showWelcomeScreen(false), isGameOver(false), isVictory(false), phaseBannerTimer(0.0f), controlsHintTimer(0.0f), selectedWeapon(WeaponSelection::Melee), perfFreq(0), lastInputEventCounter(0), hasPendingInputSample(false), totalFrameMs(0.0), totalFrameMsSq(0.0), minFrameMs(0.0), maxFrameMs(0.0), totalWorkMs(0.0), totalFixedDriftMs(0.0), maxFixedDriftMs(0.0), totalInputLatencyMs(0.0), inputLatencySamples(0), totalFrames(0), droppedFrames(0), prevEnemiesCapacity(0), prevProjectilesCapacity(0), prevGrenadesCapacity(0), capacityChangeEvents(0), totalEntitiesProcessed(0.0), totalEntitiesProcessedSq(0.0), entitySamples(0), entitiesProcessedThisFrame(0), player{100.0f, 0.0f, 0.0f, 0.0f, 50, 50, false, false, 0.0f, 0.0f, 1, false, false, 0.0f, 0.0f, 1, 100, 0.0f, false, 0, 0, false, 0, 2, false}, boss{900.0f, 0.0f, 120, 120, 35, 35, false} {
}

void Game::beginPerformanceTracking() {
    perfFreq = SDL_GetPerformanceFrequency();
    lastInputEventCounter = 0;
    hasPendingInputSample = false;
    totalFrameMs = 0.0;
    totalFrameMsSq = 0.0;
    minFrameMs = std::numeric_limits<double>::max();
    maxFrameMs = 0.0;
    totalWorkMs = 0.0;
    totalFixedDriftMs = 0.0;
    maxFixedDriftMs = 0.0;
    totalInputLatencyMs = 0.0;
    inputLatencySamples = 0;
    totalFrames = 0;
    droppedFrames = 0;
    prevEnemiesCapacity = enemies.capacity();
    prevProjectilesCapacity = projectiles.capacity();
    prevGrenadesCapacity = grenades.capacity();
    capacityChangeEvents = 0;
    totalEntitiesProcessed = 0.0;
    totalEntitiesProcessedSq = 0.0;
    entitySamples = 0;
    entitiesProcessedThisFrame = 0;
}

void Game::printPerformanceReport() const {
    const double targetFrameMs = 1000.0 / 60.0;
    const int frames = std::max(1, totalFrames);
    const double avgFrameMs = totalFrameMs / static_cast<double>(frames);
    const double avgWorkMs = totalWorkMs / static_cast<double>(frames);
    const double avgFixedDriftMs = totalFixedDriftMs / static_cast<double>(frames);
    const double frameVar = std::max(0.0, (totalFrameMsSq / static_cast<double>(frames)) - (avgFrameMs * avgFrameMs));
    const double frameStdDev = std::sqrt(frameVar);
    const double avgFps = (avgFrameMs > 0.0) ? (1000.0 / avgFrameMs) : 0.0;
    const double cpuUsageApprox = (totalFrameMs > 0.0) ? (totalWorkMs / totalFrameMs) * 100.0 : 0.0;
    const double dropRatio = std::min(1.0, static_cast<double>(droppedFrames) / static_cast<double>(frames));
    const double frameDropStabilityPercent = (1.0 - dropRatio) * 10.0;

    double avgEntities = 0.0;
    double entityStdDev = 0.0;
    if (entitySamples > 0) {
        avgEntities = totalEntitiesProcessed / static_cast<double>(entitySamples);
        const double eVar = std::max(0.0, (totalEntitiesProcessedSq / static_cast<double>(entitySamples)) - (avgEntities * avgEntities));
        entityStdDev = std::sqrt(eVar);
    }

    std::cout << "\n===== Runtime Performance Report =====\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Frame time consistency: avg=" << avgFrameMs << "ms, min=" << minFrameMs << "ms, max=" << maxFrameMs << "ms, jitter(stddev)=" << frameStdDev << "ms, avg fps=" << avgFps << "\n";
    if (inputLatencySamples > 0) {
        std::cout << "Input Latency: avg=" << (totalInputLatencyMs / static_cast<double>(inputLatencySamples)) << "ms across " << inputLatencySamples << " samples\n";
    } else {
        std::cout << "Input Latency: no input samples captured\n";
    }
    std::cout << "Fixed Timestamps Stability: target=" << targetFrameMs << "ms, avg drift=" << avgFixedDriftMs << "ms, max drift=" << maxFixedDriftMs << "ms\n";
    std::cout << "Zero Frame Drops: drops=" << droppedFrames << ", stability contribution=" << frameDropStabilityPercent << "%/10.00%\n";
    std::cout << "Work per frame: avg active work=" << avgWorkMs << "ms\n";
    std::cout << "Lockless behaviour: single-threaded frame path, no mutex/lock contention\n";
    std::cout << "Memory stability: container capacity change events=" << capacityChangeEvents << "\n";
    std::cout << "entity processing stability: avg entities/frame=" << avgEntities << ", stddev=" << entityStdDev << "\n";
    std::cout << "Threads communication: single-threaded mode, inter-thread messaging not required\n";
    std::cout << "Cpu usage: approx " << cpuUsageApprox << "% of one core\n";
    std::cout << "======================================\n";
}

void Game::restartGame() {
    resetPlayerStateForPhaseStart();

    player.health = 100;
    player.damageCooldownTimer = 0.0f;
    player.hasPistol = false;
    player.hasShotgun = false;
    player.pistolAmmo = 0;
    player.shotgunAmmo = 0;
    player.killCount = 0;
    player.grenadeCount = 2;

    projectiles.clear();
    grenades.clear();

    currentPhase = 1;
    enemiesKilledInPhase = 0;
    cameraX = 0.0f;
    isPaused = false;
    showWelcomeScreen = false;
    isGameOver = false;
    isVictory = false;
    phaseBannerTimer = 1.5f;
    controlsHintTimer = 8.0f;
    selectedWeapon = WeaponSelection::Melee;

    bossActive = false;
    bossSpawnTriggered = false;
    bossSpawnTimer = 0.0f;
    boss = {900.0f, 0.0f, 120, 120, 35, 35, false};

    spawnEnemiesForPhase();
}

void Game::resetPlayerStateForPhaseStart() {
    player.x = 100.0f;
    player.y = static_cast<float>(WINDOW_HEIGHT - player.height);
    player.vx = 0.0f;
    player.vy = 0.0f;
    player.onGround = true;
    player.jumpPressedLastFrame = false;
    player.isDashing = false;
    player.dashTimer = 0.0f;
    player.dashCooldownTimer = 0.0f;
    player.isAttacking = false;
    player.attackTimer = 0.0f;
    player.attackCooldownTimer = 0.0f;
    player.jumpRequested = false;
    playerAnimationState = PlayerAnimationState::Idle;
    playerCurrentFrame = 0;
    playerAnimationTimer = 0.0f;
}

bool Game::loadPlayerSpriteSheets() {
    auto loadSheet = [this](const char* path, SDL_Texture*& texture, int& frameCount) -> bool {
        texture = IMG_LoadTexture(renderer, path);
        if (texture == nullptr) {
            SDL_Log("Unable to load player sheet %s: %s", path, IMG_GetError());
            frameCount = 1;
            return false;
        }

        int textureWidth = 0;
        int textureHeight = 0;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight) != 0) {
            SDL_Log("Unable to query player sheet %s: %s", path, SDL_GetError());
            frameCount = 1;
            return true;
        }

        (void)textureHeight;
        frameCount = std::max(1, textureWidth / PLAYER_SHEET_FRAME_WIDTH);
        return true;
    };

    const bool idleLoaded = loadSheet("assets/sprites/player/player_idle_sheet.png", playerIdleTexture, playerIdleFrameCount);
    const bool runLoaded = loadSheet("assets/sprites/player/player_run_sheet.png", playerRunTexture, playerRunFrameCount);
    const bool shootLoaded = loadSheet("assets/sprites/player/player_shoot_sheet.png", playerShootTexture, playerShootFrameCount);

    if (!idleLoaded && !runLoaded && !shootLoaded) {
        SDL_Log("Player sprite sheets not found - using fallback rendering");
        return false;
    }

    return true;
}

void Game::updatePlayerAnimation(float frameSeconds) {
    PlayerAnimationState desiredState = PlayerAnimationState::Idle;
    if (player.isAttacking) {
        desiredState = PlayerAnimationState::Shoot;
    } else if (std::fabs(player.vx) > 1.0f) {
        desiredState = PlayerAnimationState::Run;
    }

    if (desiredState != playerAnimationState) {
        playerAnimationState = desiredState;
        playerCurrentFrame = 0;
        playerAnimationTimer = 0.0f;
    }

    int frameCount = 1;
    float frameDuration = PLAYER_IDLE_FRAME_DURATION;
    bool looping = true;

    switch (playerAnimationState) {
        case PlayerAnimationState::Idle:
            frameCount = playerIdleFrameCount;
            frameDuration = PLAYER_IDLE_FRAME_DURATION;
            looping = true;
            break;
        case PlayerAnimationState::Run:
            frameCount = playerRunFrameCount;
            frameDuration = PLAYER_RUN_FRAME_DURATION;
            looping = true;
            break;
        case PlayerAnimationState::Shoot:
            frameCount = playerShootFrameCount;
            frameDuration = PLAYER_SHOOT_FRAME_DURATION;
            looping = false;
            break;
    }

    frameCount = std::max(1, frameCount);

    playerAnimationTimer += frameSeconds;
    while (playerAnimationTimer >= frameDuration) {
        playerAnimationTimer -= frameDuration;
        playerCurrentFrame += 1;
        if (playerCurrentFrame >= frameCount) {
            playerCurrentFrame = looping ? 0 : frameCount - 1;
        }
    }
}

void Game::renderPlayerSprite(float cameraX) {
    SDL_Texture* texture = nullptr;
    int frameCount = 1;

    switch (playerAnimationState) {
        case PlayerAnimationState::Idle:
            texture = playerIdleTexture;
            frameCount = playerIdleFrameCount;
            break;
        case PlayerAnimationState::Run:
            texture = playerRunTexture;
            frameCount = playerRunFrameCount;
            break;
        case PlayerAnimationState::Shoot:
            texture = playerShootTexture;
            frameCount = playerShootFrameCount;
            break;
    }

    if (texture == nullptr) {
        SDL_Rect playerRect = {
            static_cast<int>(player.x - cameraX),
            static_cast<int>(player.y),
            player.width,
            player.height
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &playerRect);
        return;
    }

    const int safeFrameCount = std::max(1, frameCount);
    const int frameIndex = std::min(playerCurrentFrame, safeFrameCount - 1);

    SDL_Rect srcRect = {
        PLAYER_SHEET_FRAME_WIDTH * frameIndex,
        0,
        PLAYER_SHEET_FRAME_WIDTH,
        PLAYER_SHEET_FRAME_HEIGHT
    };
    SDL_Rect destRect = {
        static_cast<int>(player.x - cameraX),
        static_cast<int>(player.y),
        player.width,
        player.height
    };

    SDL_RendererFlip flip = player.facingDirection >= 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, nullptr, flip);
}

void Game::spawnEnemiesForPhase() {
    const float groundY = static_cast<float>(WINDOW_HEIGHT - 50);
    const float phaseBaseX = static_cast<float>((currentPhase - 1) * 1400);

    enemies.clear();

    if (currentPhase == 1) {
        enemies.push_back({phaseBaseX + 600.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 800.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1000.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
    } else if (currentPhase == 2) {
        enemies.push_back({phaseBaseX + 600.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 800.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1000.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
    } else {
        enemies.push_back({phaseBaseX + 350.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 500.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 650.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 800.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 950.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1100.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1250.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1400.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1550.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1700.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 1850.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 1, 2.0f, false, 0.0f});
        enemies.push_back({phaseBaseX + 2000.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 3, 2.0f, false, 0.0f});
    }
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return false;
    }

    std::srand(static_cast<unsigned int>(SDL_GetTicks()));

    window = SDL_CreateWindow(
        "Hellborne",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        SDL_Log("Window Error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == nullptr) {
        SDL_Log("Renderer Error: %s", SDL_GetError());
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }

    loadPlayerSpriteSheets();

    restartGame();
    showWelcomeScreen = true;
    SDL_SetWindowTitle(window, "Hellborne - Phase 1: Move A/D, Jump SPACE/W/UP, Attack J/Left Click, Switch 1/2/3, Pause P");

    isRunning = true;
    return true;
}

void Game::run() {
    const Uint32 frameDelay = 16; // ~60 FPS target
    beginPerformanceTracking();

    while (isRunning) {
        const Uint32 frameStart = SDL_GetTicks();
        const Uint64 frameStartCounter = SDL_GetPerformanceCounter();
        const Uint64 workStartCounter = frameStartCounter;

        handleEvents();
        update();
        render();

        const Uint64 workEndCounter = SDL_GetPerformanceCounter();

        const Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime > frameDelay) {
            droppedFrames += 1;
        }
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }

        const Uint64 frameEndCounter = SDL_GetPerformanceCounter();
        const double workMs = (static_cast<double>(workEndCounter - workStartCounter) * 1000.0) / static_cast<double>(perfFreq);
        const double frameMs = (static_cast<double>(frameEndCounter - frameStartCounter) * 1000.0) / static_cast<double>(perfFreq);
        const double driftMs = std::fabs(frameMs - (1000.0 / 60.0));

        totalFrames += 1;
        totalWorkMs += workMs;
        totalFrameMs += frameMs;
        totalFrameMsSq += frameMs * frameMs;
        totalFixedDriftMs += driftMs;

        if (driftMs > maxFixedDriftMs) {
            maxFixedDriftMs = driftMs;
        }
        if (frameMs < minFrameMs) {
            minFrameMs = frameMs;
        }
        if (frameMs > maxFrameMs) {
            maxFrameMs = frameMs;
        }

        if (hasPendingInputSample) {
            const double latencyMs = (static_cast<double>(workEndCounter - lastInputEventCounter) * 1000.0) / static_cast<double>(perfFreq);
            totalInputLatencyMs += latencyMs;
            inputLatencySamples += 1;
            hasPendingInputSample = false;
        }

        if (enemies.capacity() != prevEnemiesCapacity) {
            prevEnemiesCapacity = enemies.capacity();
            capacityChangeEvents += 1;
        }
        if (projectiles.capacity() != prevProjectilesCapacity) {
            prevProjectilesCapacity = projectiles.capacity();
            capacityChangeEvents += 1;
        }
        if (grenades.capacity() != prevGrenadesCapacity) {
            prevGrenadesCapacity = grenades.capacity();
            capacityChangeEvents += 1;
        }

        totalEntitiesProcessed += static_cast<double>(entitiesProcessedThisFrame);
        totalEntitiesProcessedSq += static_cast<double>(entitiesProcessedThisFrame * entitiesProcessedThisFrame);
        entitySamples += 1;
    }

    printPerformanceReport();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
            lastInputEventCounter = SDL_GetPerformanceCounter();
            hasPendingInputSample = true;
        }

        if (showWelcomeScreen) {
            if (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN) {
                showWelcomeScreen = false;
            }
            continue;
        }

        if (isGameOver || isVictory) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_r) {
                    restartGame();
                } else if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (isPointInRect(mouseX, mouseY, getRetryButtonRect())) {
                    restartGame();
                } else if (isPointInRect(mouseX, mouseY, getQuitButtonRect())) {
                    isRunning = false;
                }
            }
            continue;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }

            if (event.key.keysym.sym == SDLK_p) {
                isPaused = !isPaused;
                SDL_Log(isPaused ? "Paused" : "Resumed");
                continue;
            }

            if (isPaused) {
                continue;
            }

            if (event.key.keysym.sym == SDLK_SPACE ||
                event.key.keysym.sym == SDLK_w ||
                event.key.keysym.sym == SDLK_UP) {
                player.jumpRequested = true;
            }

            if (event.key.keysym.sym == SDLK_LSHIFT) {
                if (!player.isDashing && player.dashCooldownTimer <= 0.0f) {
                    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
                    if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
                        player.dashDirection = -1;
                    } else if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
                        player.dashDirection = 1;
                    } else {
                        player.dashDirection = 1;
                    }

                    player.isDashing = true;
                    player.dashTimer = 0.15f;
                    player.vx = 0.0f;
                    player.vy = 0.0f;
                }
            }

        }

        if (!isPaused) {
            handleWeaponInput(event);
        }
    }
}

SDL_Rect Game::getAttackRect() {
    const int attackOffset = 5;

    SDL_Rect attackRect = {
        0,
        static_cast<int>(player.y) + player.height / 4,
        40,
        player.height / 2
    };

    if (player.facingDirection >= 0) {
        attackRect.x = static_cast<int>(player.x) + player.width - attackOffset;
    } else {
        attackRect.x = static_cast<int>(player.x) - attackRect.w + attackOffset;
    }

    return attackRect;
}

SDL_Rect Game::getRetryButtonRect() const {
    return SDL_Rect{WINDOW_WIDTH / 2 - 170, WINDOW_HEIGHT / 2 + 92, 150, 44};
}

SDL_Rect Game::getQuitButtonRect() const {
    return SDL_Rect{WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2 + 92, 150, 44};
}

bool Game::isPointInRect(int x, int y, const SDL_Rect& rect) const {
    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
}

void Game::handleWeaponInput(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_1 && event.key.repeat == 0) {
            selectedWeapon = WeaponSelection::Melee;
        }

        if (event.key.keysym.sym == SDLK_2 && event.key.repeat == 0 && player.hasPistol) {
            selectedWeapon = WeaponSelection::Pistol;
        }

        if (event.key.keysym.sym == SDLK_3 && event.key.repeat == 0 && player.hasShotgun) {
            selectedWeapon = WeaponSelection::Shotgun;
        }

        if (event.key.keysym.sym == SDLK_q && event.key.repeat == 0) {
            if (selectedWeapon == WeaponSelection::Melee) {
                selectedWeapon = player.hasPistol ? WeaponSelection::Pistol : WeaponSelection::Melee;
            } else if (selectedWeapon == WeaponSelection::Pistol) {
                selectedWeapon = player.hasShotgun ? WeaponSelection::Shotgun : WeaponSelection::Melee;
            } else {
                selectedWeapon = WeaponSelection::Melee;
            }
        }

        if (event.key.keysym.sym == SDLK_j) {
            performSelectedAttack();
        }

        if (event.key.keysym.sym == SDLK_g) {
            if (player.grenadeCount > 0) {
                Grenade g;
                g.x = player.x + player.width / 2.0f;
                g.y = player.y + player.height / 2.0f;
                g.vx = 6.0f * static_cast<float>(player.facingDirection);
                g.vy = -9.0f;
                g.timer = 1.4f;
                g.isActive = true;
                g.bounceCount = 0;
                grenades.push_back(g);
                player.grenadeCount--;
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        performSelectedAttack();
    }
}

void Game::performSelectedAttack() {
    if (player.isDashing || player.attackCooldownTimer > 0.0f) {
        return;
    }

    if (selectedWeapon == WeaponSelection::Melee) {
        player.isAttacking = true;
        player.attackTimer = 0.1f;
        player.attackCooldownTimer = 0.2f;
        return;
    }

    if (selectedWeapon == WeaponSelection::Pistol) {
        if (!player.hasPistol || player.pistolAmmo <= 0) {
            return;
        }

        Projectile p;
        p.isActive = true;
        p.direction = player.facingDirection >= 0 ? 1 : -1;
        p.speed = 12.0f;
        p.width = 12;
        p.height = 6;
        p.x = (p.direction > 0) ? player.x + player.width : player.x - p.width;
        p.y = player.y + player.height / 2.0f - p.height / 2.0f;
        projectiles.push_back(p);
        player.pistolAmmo -= 1;
        player.attackCooldownTimer = 0.12f;
        SDL_Log("Pistol ammo: %d", player.pistolAmmo);
        return;
    }

    if (!player.hasShotgun || player.shotgunAmmo <= 0) {
        return;
    }

    for (int i = -2; i <= 2; ++i) {
        Projectile p;
        p.isActive = true;
        p.direction = player.facingDirection >= 0 ? 1 : -1;
        p.speed = 10.0f;
        p.width = 10;
        p.height = 5;
        p.x = (p.direction > 0) ? player.x + player.width : player.x - p.width;
        p.y = player.y + player.height / 2 + i * 6;
        projectiles.push_back(p);
    }
    player.shotgunAmmo -= 1;
    player.attackCooldownTimer = 0.35f;
    SDL_Log("Shotgun ammo: %d", player.shotgunAmmo);
}

void Game::applyEnemyDamage(Enemy& enemy, int damage, bool triggerRage) {
    if (!enemy.isAlive || damage <= 0) {
        return;
    }

    enemy.health -= damage;

    if (enemy.type == Enemy::EnemyType::Medium && triggerRage) {
        enemy.isRaging = true;
        enemy.rageTimer = 1.5f;
    }

    if (enemy.health <= 0) {
        enemy.health = 0;
        enemy.isAlive = false;
        player.killCount += 1;
        enemiesKilledInPhase += 1;

        if (player.killCount % 3 == 0) {
            player.pistolAmmo += 3;
        } else if (std::rand() % 3 == 0) {
            player.pistolAmmo += 1;
        }

        if (player.hasShotgun) {
            if (player.killCount % 4 == 0) {
                player.shotgunAmmo += 1;
            } else if (std::rand() % 5 == 0) {
                player.shotgunAmmo += 1;
            }
        }
    }
}

void Game::applyMeleeDamage() {
    if (player.isAttacking) {
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) {
                continue;
            }

            SDL_Rect attackRect = getAttackRect();
            SDL_Rect enemyRect = {
                static_cast<int>(enemy.x),
                static_cast<int>(enemy.y),
                enemy.width,
                enemy.height
            };

            bool overlaps = attackRect.x < enemyRect.x + enemyRect.w &&
                            attackRect.x + attackRect.w > enemyRect.x &&
                            attackRect.y < enemyRect.y + enemyRect.h &&
                            attackRect.y + attackRect.h > enemyRect.y;

            if (overlaps) {
                applyEnemyDamage(enemy, 1, true);
            }
        }
    }

    if (player.isAttacking && bossActive && boss.isAlive) {
        SDL_Rect attackRect = getAttackRect();

        SDL_Rect bossRect = {
            static_cast<int>(boss.x),
            static_cast<int>(boss.y),
            boss.width,
            boss.height
        };

        bool overlaps = attackRect.x < bossRect.x + bossRect.w &&
                        attackRect.x + attackRect.w > bossRect.x &&
                        attackRect.y < bossRect.y + bossRect.h &&
                        attackRect.y + attackRect.h > bossRect.y;

        if (overlaps) {
            boss.health -= 1;
        }
    }
}

void Game::updateProjectiles() {
    for (auto& p : projectiles) {
        if (!p.isActive) {
            continue;
        }

        p.x += p.speed * static_cast<float>(p.direction);

        if (p.x < 0.0f || p.x > WORLD_WIDTH) {
            p.isActive = false;
            continue;
        }

        SDL_Rect projectileRect = {
            static_cast<int>(p.x),
            static_cast<int>(p.y),
            p.width,
            p.height
        };

        for (auto& enemy : enemies) {
            if (!enemy.isAlive) {
                continue;
            }

            SDL_Rect enemyRect = {
                static_cast<int>(enemy.x),
                static_cast<int>(enemy.y),
                enemy.width,
                enemy.height
            };

            bool overlaps = projectileRect.x < enemyRect.x + enemyRect.w &&
                            projectileRect.x + projectileRect.w > enemyRect.x &&
                            projectileRect.y < enemyRect.y + enemyRect.h &&
                            projectileRect.y + projectileRect.h > enemyRect.y;

            if (overlaps) {
                applyEnemyDamage(enemy, 1, true);
                p.isActive = false;
                break;
            }
        }

        if (bossActive && boss.isAlive) {
            SDL_Rect bossRect = {
                static_cast<int>(boss.x),
                static_cast<int>(boss.y),
                boss.width,
                boss.height
            };

            bool overlaps = projectileRect.x < bossRect.x + bossRect.w &&
                            projectileRect.x + projectileRect.w > bossRect.x &&
                            projectileRect.y < bossRect.y + bossRect.h &&
                            projectileRect.y + projectileRect.h > bossRect.y;

            if (overlaps) {
                boss.health -= 1;
                p.isActive = false;
            }
        }
    }
}

void Game::updateGrenades(float frameSeconds) {
    for (auto& g : grenades) {
        if (!g.isActive) {
            continue;
        }

        const float grenadeGravity = 16.0f;
        g.vy += grenadeGravity * frameSeconds;
        g.x += g.vx * frameSeconds * 60.0f;
        g.y += g.vy * frameSeconds * 60.0f;
        g.timer -= frameSeconds;

        const float groundY = static_cast<float>(WINDOW_HEIGHT - 10);

        if (g.y >= groundY) {
            g.y = groundY;
            g.vy = -g.vy * 0.45f;
            g.vx *= 0.78f;
            g.bounceCount += 1;

            if (std::fabs(g.vy) < 1.5f || g.bounceCount >= 2) {
                g.timer = 0.0f;
            }
        }

        if (g.x < 0.0f) {
            g.x = 0.0f;
            g.vx = -g.vx * 0.5f;
        }

        if (g.x > WORLD_WIDTH) {
            g.x = WORLD_WIDTH;
            g.vx = -g.vx * 0.5f;
        }

        if (g.timer <= 0.0f) {
            const float radius = 120.0f;
            const float grenadeCenterX = g.x + 5.0f;
            const float grenadeCenterY = g.y + 5.0f;

            SDL_Rect explosionRect = {
                static_cast<int>(grenadeCenterX - radius),
                static_cast<int>(grenadeCenterY - radius),
                static_cast<int>(radius * 2.0f),
                static_cast<int>(radius * 2.0f)
            };

            if (bossActive && boss.isAlive) {
                SDL_Rect bossRect = {
                    static_cast<int>(boss.x),
                    static_cast<int>(boss.y),
                    boss.width,
                    boss.height
                };

                bool overlaps = explosionRect.x < bossRect.x + bossRect.w &&
                                explosionRect.x + explosionRect.w > bossRect.x &&
                                explosionRect.y < bossRect.y + bossRect.h &&
                                explosionRect.y + explosionRect.h > bossRect.y;

                if (overlaps) {
                    boss.health -= 5;
                }
            }

            for (auto& enemy : enemies) {
                if (!enemy.isAlive) {
                    continue;
                }

                float enemyCenterX = enemy.x + static_cast<float>(enemy.width) * 0.5f;
                float enemyCenterY = enemy.y + static_cast<float>(enemy.height) * 0.5f;
                float dx = enemyCenterX - grenadeCenterX;
                float dy = enemyCenterY - grenadeCenterY;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance < radius) {
                    applyEnemyDamage(enemy, 2, true);
                }
            }

            g.isActive = false;
        }
    }
}

void Game::update() {
    const float moveSpeed = 5.0f;
    const float jumpVelocity = -14.0f;
    const float gravity = 0.7f;
    const float dashSpeed = 18.0f;
    const float dashCooldown = 0.5f;
    const float frameSeconds = 1.0f / 60.0f;

    if (phaseBannerTimer > 0.0f) {
        phaseBannerTimer -= frameSeconds;
        if (phaseBannerTimer < 0.0f) {
            phaseBannerTimer = 0.0f;
        }
    }

    if (controlsHintTimer > 0.0f) {
        controlsHintTimer -= frameSeconds;
        if (controlsHintTimer < 0.0f) {
            controlsHintTimer = 0.0f;
        }
    }

    if (currentPhase == 1 && controlsHintTimer > 0.0f) {
        SDL_SetWindowTitle(window, "Hellborne - Phase 1: Move A/D, Jump SPACE/W/UP, Attack J/Left Click, Switch 1/2/3, Pause P");
    } else {
        SDL_SetWindowTitle(window, "Hellborne");
    }

    if (showWelcomeScreen || isPaused || isGameOver || isVictory) {
        return;
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

    if (player.attackCooldownTimer > 0.0f) {
        player.attackCooldownTimer -= frameSeconds;
        if (player.attackCooldownTimer < 0.0f) {
            player.attackCooldownTimer = 0.0f;
        }
    }

    if (player.damageCooldownTimer > 0.0f) {
        player.damageCooldownTimer -= frameSeconds;
        if (player.damageCooldownTimer < 0.0f) {
            player.damageCooldownTimer = 0.0f;
        }
    }

    if (currentPhase >= 1 && player.killCount >= 5) {
        player.hasPistol = true;
    }

    if (currentPhase >= 2 && player.killCount >= 10) {
        if (!player.hasShotgun) {
            player.hasShotgun = true;
            player.shotgunAmmo += 6;
            SDL_Log("Shotgun unlocked. Shotgun ammo: %d", player.shotgunAmmo);
        }
    }

    if (player.isAttacking) {
        player.attackTimer -= frameSeconds;
        if (player.attackTimer <= 0.0f) {
            player.isAttacking = false;
            player.attackTimer = 0.0f;
        }
    }

    if (player.dashCooldownTimer > 0.0f) {
        player.dashCooldownTimer -= frameSeconds;
        if (player.dashCooldownTimer < 0.0f) {
            player.dashCooldownTimer = 0.0f;
        }
    }

    if (player.isDashing) {
        player.x += dashSpeed * static_cast<float>(player.dashDirection) * frameSeconds * 60.0f;
        player.y += player.vy;
        player.dashTimer -= frameSeconds;

        if (player.dashTimer <= 0.0f) {
            player.isDashing = false;
            player.dashTimer = 0.0f;
            player.dashCooldownTimer = dashCooldown;
            player.vx = 0.0f;
        }
    } else {
        player.vx = 0.0f;
        if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
            player.vx = -moveSpeed;
        }
        if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
            player.vx = moveSpeed;
        }

        if (player.vx < 0.0f) {
            player.facingDirection = -1;
        } else if (player.vx > 0.0f) {
            player.facingDirection = 1;
        }

        bool jumpPressed = keyboardState[SDL_SCANCODE_SPACE] ||
                           keyboardState[SDL_SCANCODE_W] ||
                           keyboardState[SDL_SCANCODE_UP];

        if ((player.jumpRequested || jumpPressed) && player.onGround) {
            player.vy = jumpVelocity;
            player.onGround = false;
        }
        player.jumpPressedLastFrame = jumpPressed;
        player.jumpRequested = false;

        player.vy += gravity;
        player.x += player.vx;
        player.y += player.vy;
    }

    if (player.x < 0.0f) {
        player.x = 0.0f;
    }
    if (player.x + player.width > WORLD_WIDTH) {
        player.x = static_cast<float>(WORLD_WIDTH - player.width);
    }

    if (player.y < 0.0f) {
        player.y = 0.0f;
        player.vy = 0.0f;
    }

    const float floorY = static_cast<float>(WINDOW_HEIGHT - player.height);
    if (player.y >= floorY) {
        player.y = floorY;
        player.vy = 0.0f;
        player.onGround = true;
    }

    updatePlayerAnimation(frameSeconds);

    const float cameraTarget = player.x + static_cast<float>(player.width) * 0.5f - static_cast<float>(WINDOW_WIDTH) * 0.5f;
    const float maxCameraX = static_cast<float>(WORLD_WIDTH - WINDOW_WIDTH);
    cameraX = std::clamp(cameraTarget, 0.0f, maxCameraX);

    for (auto& enemy : enemies) {
        if (enemy.isAlive && !player.isDashing) {
            float currentSpeed = enemy.speed;

            if (enemy.type == Enemy::EnemyType::Medium && enemy.isRaging) {
                currentSpeed = 5.0f;
                enemy.rageTimer -= frameSeconds;

                if (enemy.rageTimer <= 0.0f) {
                    enemy.isRaging = false;
                    enemy.rageTimer = 0.0f;
                }
            }

            const float deltaX = enemy.x - player.x;
            if (deltaX > 5.0f) {
                enemy.x -= currentSpeed;
            } else if (deltaX < -5.0f) {
                enemy.x += currentSpeed;
            }
        }
    }

    for (auto& enemy : enemies) {
        if (enemy.isAlive) {
            SDL_Rect playerRect = {
                static_cast<int>(player.x),
                static_cast<int>(player.y),
                player.width,
                player.height
            };

            SDL_Rect enemyRect = {
                static_cast<int>(enemy.x),
                static_cast<int>(enemy.y),
                enemy.width,
                enemy.height
            };

            bool overlaps = playerRect.x < enemyRect.x + enemyRect.w &&
                            playerRect.x + playerRect.w > enemyRect.x &&
                            playerRect.y < enemyRect.y + enemyRect.h &&
                            playerRect.y + playerRect.h > enemyRect.y;

            if (overlaps && player.damageCooldownTimer <= 0.0f) {
                player.health -= 10;
                player.damageCooldownTimer = 0.5f;
            }
        }
    }

    applyMeleeDamage();
    updateProjectiles();
    updateGrenades(frameSeconds);

    if (bossActive && boss.isAlive) {
        float deltaX = boss.x - player.x;

        if (deltaX > 5.0f) {
            boss.x -= 2.5f;
        } else if (deltaX < -5.0f) {
            boss.x += 2.5f;
        }
    }

    if (bossActive && boss.isAlive) {
        SDL_Rect playerRect = {
            static_cast<int>(player.x),
            static_cast<int>(player.y),
            player.width,
            player.height
        };

        SDL_Rect bossRect = {
            static_cast<int>(boss.x),
            static_cast<int>(boss.y),
            boss.width,
            boss.height
        };

        bool overlaps = playerRect.x < bossRect.x + bossRect.w &&
                        playerRect.x + playerRect.w > bossRect.x &&
                        playerRect.y < bossRect.y + bossRect.h &&
                        playerRect.y + playerRect.h > bossRect.y;

        if (overlaps && player.damageCooldownTimer <= 0.0f) {
            player.health -= 15;
            player.damageCooldownTimer = 0.5f;
        }
    }

    if (bossActive && boss.isAlive && boss.health <= 0) {
        boss.isAlive = false;
        bossActive = false;
        isVictory = true;
        isPaused = false;
        SDL_Log("YOU WIN");
        return;
    }

    if (player.health <= 0) {
        isGameOver = true;
        isPaused = false;
        return;
    }

    bool allEnemiesDead = true;

    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            allEnemiesDead = false;
            break;
        }
    }

    bool transitionedThisFrame = false;

    const bool reachedPhase2Gate = player.x >= 1400.0f;
    const bool reachedPhase3Gate = player.x >= 2600.0f;

    if (currentPhase == 1 && (reachedPhase2Gate || allEnemiesDead)) {
        currentPhase = 2;
        enemiesKilledInPhase = 0;
        player.grenadeCount += 1;
        phaseBannerTimer = 1.5f;
        controlsHintTimer = 0.0f;
        SDL_Log("Phase: %d", currentPhase);
        player.isDashing = false;
        player.isAttacking = false;
        player.dashTimer = 0.0f;
        player.attackTimer = 0.0f;
        spawnEnemiesForPhase();
        transitionedThisFrame = true;
    }

    else if (currentPhase == 2 && (reachedPhase3Gate || allEnemiesDead)) {
        currentPhase = 3;
        enemiesKilledInPhase = 0;
        phaseBannerTimer = 1.5f;
        controlsHintTimer = 0.0f;
        SDL_Log("Phase: %d", currentPhase);
        player.isDashing = false;
        player.isAttacking = false;
        player.dashTimer = 0.0f;
        player.attackTimer = 0.0f;
        spawnEnemiesForPhase();
        transitionedThisFrame = true;
    }

    bool phaseEnemiesAlive = false;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            phaseEnemiesAlive = true;
            break;
        }
    }

    if (currentPhase == 3 && !phaseEnemiesAlive && !bossSpawnTriggered && !bossActive && !transitionedThisFrame) {
        bossSpawnTriggered = true;
        bossSpawnTimer = 2.0f;
    }

    if (bossSpawnTriggered && !bossActive) {
        bossSpawnTimer -= frameSeconds;

        if (bossSpawnTimer <= 0.0f) {
            bossActive = true;
            boss.isAlive = true;
            boss.y = static_cast<float>(WINDOW_HEIGHT - boss.height);
        }
    }

    int aliveEntityCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            aliveEntityCount += 1;
        }
    }
    if (bossActive && boss.isAlive) {
        aliveEntityCount += 1;
    }
    entitiesProcessedThisFrame = aliveEntityCount;

    grenades.erase(
        std::remove_if(grenades.begin(), grenades.end(),
            [](const Grenade& g) { return !g.isActive; }),
        grenades.end()
    );
}

void Game::render() {
    auto fillRect = [&](int x, int y, int w, int h) {
        SDL_Rect r = {x, y, w, h};
        SDL_RenderFillRect(renderer, &r);
    };

    auto drawPhaseLetter = [&](char c, int x, int y, int scale) {
        const int t = 2 * scale;
        const int w = 12 * scale;
        const int h = 18 * scale;

        switch (c) {
            case 'M':
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x + w / 2 - t / 2, y + h / 4, t, h - h / 4);
                break;
            case 'D':
                fillRect(x, y, t, h);
                fillRect(x, y, w - t / 2, t);
                fillRect(x, y + h - t, w - t / 2, t);
                fillRect(x + w - t, y + t / 2, t, h - t);
                break;
            case 'B':
                fillRect(x, y, t, h);
                fillRect(x, y, w - t / 2, t);
                fillRect(x, y + h / 2 - t / 2, w - t / 2, t);
                fillRect(x, y + h - t, w - t / 2, t);
                fillRect(x + w - t, y + t / 2, t, h / 2 - t / 2);
                fillRect(x + w - t, y + h / 2, t, h / 2 - t / 2);
                break;
            case 'I':
                fillRect(x, y, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x + w / 2 - t / 2, y, t, h);
                break;
            case 'K':
                fillRect(x, y, t, h);
                fillRect(x + w / 2 - t / 2, y + h / 3, t, h / 3);
                fillRect(x + w - t, y, t, h / 2);
                fillRect(x + w - t, y + h / 2, t, h / 2);
                break;
            case 'C':
                fillRect(x, y, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x, y, t, h);
                break;
            case 'P':
                fillRect(x, y, t, h);
                fillRect(x, y, w, t);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x + w - t, y, t, h / 2);
                break;
            case 'H':
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x, y + h / 2 - t / 2, w, t);
                break;
            case 'A':
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x, y, w, t);
                fillRect(x, y + h / 2 - t / 2, w, t);
                break;
            case 'L':
                fillRect(x, y, t, h);
                fillRect(x, y + h - t, w, t);
                break;
            case 'N':
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x + w / 2 - t / 2, y + h / 5, t, h - h / 5);
                break;
            case 'O':
                fillRect(x, y, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                break;
            case 'U':
                fillRect(x, y, t, h - t);
                fillRect(x + w - t, y, t, h - t);
                fillRect(x, y + h - t, w, t);
                break;
            case 'W':
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x + w / 2 - t / 2, y + h / 2, t, h / 2);
                break;
            case 'Y':
                fillRect(x, y, t, h / 2);
                fillRect(x + w - t, y, t, h / 2);
                fillRect(x + w / 2 - t / 2, y + h / 2 - t / 2, t, h / 2 + t / 2);
                break;
            case 'Q':
                fillRect(x, y, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x, y, t, h);
                fillRect(x + w - t, y, t, h);
                fillRect(x + w / 2, y + h / 2, t, h / 2);
                break;
            case 'R':
                fillRect(x, y, t, h);
                fillRect(x, y, w, t);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x + w - t, y, t, h / 2);
                fillRect(x + w / 2 - t / 2, y + h / 2, t, h / 2);
                break;
            case 'T':
                fillRect(x, y, w, t);
                fillRect(x + w / 2 - t / 2, y, t, h);
                break;
            case 'S':
                fillRect(x, y, w, t);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x, y, t, h / 2);
                fillRect(x + w - t, y + h / 2, t, h / 2);
                break;
            case 'E':
                fillRect(x, y, t, h);
                fillRect(x, y, w, t);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x, y + h - t, w, t);
                break;
            case 'G':
                fillRect(x, y, w, t);
                fillRect(x, y + h - t, w, t);
                fillRect(x, y, t, h);
                fillRect(x + w / 2, y + h / 2 - t / 2, w / 2, t);
                fillRect(x + w - t, y + h / 2 - t / 2, t, h / 2 + t / 2);
                break;
            default:
                break;
        }
    };

    auto drawWord = [&](const char* text, int x, int y, int scale) {
        int cursor = x;
        for (int i = 0; text[i] != '\0'; ++i) {
            if (text[i] == ' ') {
                cursor += 10 * scale;
                continue;
            }
            drawPhaseLetter(text[i], cursor, y, scale);
            cursor += 16 * scale;
        }
    };

    if (showWelcomeScreen) {
        drawNightBackground(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 120, 20, 20, 120);
        SDL_Rect glow = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT / 3};
        SDL_RenderFillRect(renderer, &glow);

        SDL_SetRenderDrawColor(renderer, 230, 70, 70, 255);
        drawWord("HELLBORNE", WINDOW_WIDTH / 2 - 236, WINDOW_HEIGHT / 2 - 88, 2);

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        drawWord("PATH TO CHAOS", WINDOW_WIDTH / 2 - 112, WINDOW_HEIGHT / 2 + 10, 1);

        SDL_SetRenderDrawColor(renderer, 170, 170, 170, 255);
        drawWord("PRESS ANY KEY", WINDOW_WIDTH / 2 - 104, WINDOW_HEIGHT / 2 + 58, 1);

        SDL_RenderPresent(renderer);
        return;
    }

    auto drawPhaseDigit = [&](int digit, int x, int y, int scale) {
        const int t = 2 * scale;
        const int w = 10 * scale;
        const int h = 18 * scale;

        switch (digit) {
            case 1:
                fillRect(x + w / 2 - t / 2, y, t, h);
                break;
            case 2:
                fillRect(x, y, w, t);
                fillRect(x + w - t, y, t, h / 2);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x, y + h / 2, t, h / 2);
                fillRect(x, y + h - t, w, t);
                break;
            case 3:
                fillRect(x, y, w, t);
                fillRect(x + w - t, y, t, h);
                fillRect(x, y + h / 2 - t / 2, w, t);
                fillRect(x, y + h - t, w, t);
                break;
            default:
                break;
        }
    };

    drawNightBackground(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    renderPlayerSprite(cameraX);

    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            SDL_Rect enemyRect = {
                static_cast<int>(enemy.x - cameraX),
                static_cast<int>(enemy.y),
                enemy.width,
                enemy.height
            };

            if (enemy.type == Enemy::EnemyType::Weak) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            } else if (enemy.isRaging) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            }

            SDL_RenderFillRect(renderer, &enemyRect);
        }
    }

    if (player.isAttacking) {
        SDL_Rect attackRect = getAttackRect();
        attackRect.x -= static_cast<int>(cameraX);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &attackRect);
    }

    for (const auto& p : projectiles) {
        if (!p.isActive) {
            continue;
        }

        SDL_Rect projectileRect = {
            static_cast<int>(p.x - cameraX),
            static_cast<int>(p.y),
            p.width,
            p.height
        };
        SDL_SetRenderDrawColor(renderer, 200, 220, 255, 255);
        SDL_RenderFillRect(renderer, &projectileRect);
    }

    for (const auto& g : grenades) {
        if (!g.isActive) {
            continue;
        }

        SDL_Rect rect = {
            static_cast<int>(g.x - cameraX),
            static_cast<int>(g.y),
            10,
            10
        };

        SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (bossActive && boss.isAlive) {
        SDL_Rect bossRect = {
            static_cast<int>(boss.x - cameraX),
            static_cast<int>(boss.y),
            boss.width,
            boss.height
        };

        SDL_SetRenderDrawColor(renderer, 200, 0, 200, 255);
        SDL_RenderFillRect(renderer, &bossRect);
    }

    if (currentPhase == 3 && (bossActive || bossSpawnTriggered)) {
        SDL_Rect bossHudBack = {WINDOW_WIDTH / 2 - 170, 56, 340, 24};
        SDL_SetRenderDrawColor(renderer, 18, 18, 18, 220);
        SDL_RenderFillRect(renderer, &bossHudBack);

        SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
        drawWord("BOSS HP", WINDOW_WIDTH / 2 - 64, 14, 1);

        float healthRatio = 0.0f;
        if (boss.maxHealth > 0) {
            healthRatio = static_cast<float>(boss.health) / static_cast<float>(boss.maxHealth);
        }
        if (healthRatio < 0.0f) {
            healthRatio = 0.0f;
        }
        if (healthRatio > 1.0f) {
            healthRatio = 1.0f;
        }

        SDL_Rect bossHudFill = {WINDOW_WIDTH / 2 - 168, 58, static_cast<int>(336.0f * healthRatio), 20};
        SDL_SetRenderDrawColor(renderer, 180, 40, 200, 255);
        SDL_RenderFillRect(renderer, &bossHudFill);
    }

    SDL_Rect phaseBack = {20, 20, 180, 26};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 220);
    SDL_RenderFillRect(renderer, &phaseBack);

    SDL_Rect hpBack = {20, 52, 180, 20};
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 220);
    SDL_RenderFillRect(renderer, &hpBack);

    float hpRatio = static_cast<float>(player.health) / 100.0f;
    if (hpRatio < 0.0f) {
        hpRatio = 0.0f;
    }
    if (hpRatio > 1.0f) {
        hpRatio = 1.0f;
    }

    SDL_Rect hpFill = {22, 54, static_cast<int>(176.0f * hpRatio), 16};
    SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
    SDL_RenderFillRect(renderer, &hpFill);

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    drawWord("HP", 206, 52, 1);

    for (int i = 1; i <= 3; ++i) {
        SDL_Rect cell = {24 + (i - 1) * 58, 24, 52, 18};
        if (i <= currentPhase) {
            SDL_SetRenderDrawColor(renderer, 230, 70, 70, 255);
            SDL_RenderFillRect(renderer, &cell);
        } else {
            SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect phaseLabelBack = {WINDOW_WIDTH / 2 - 126, 16, 252, 34};
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 180);
    SDL_RenderFillRect(renderer, &phaseLabelBack);

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    const char* phaseWord = "PHASE";
    int phaseWordX = WINDOW_WIDTH / 2 - 114;
    for (int i = 0; phaseWord[i] != '\0'; ++i) {
        drawPhaseLetter(phaseWord[i], phaseWordX + i * 20, 24, 1);
    }

    SDL_SetRenderDrawColor(renderer, 230, 70, 70, 255);
    drawPhaseDigit(currentPhase, WINDOW_WIDTH / 2 + 52, 24, 1);

    SDL_Rect weaponBack = {WINDOW_WIDTH - 226, 20, 206, 56};
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 180);
    SDL_RenderFillRect(renderer, &weaponBack);

    for (int i = 0; i < 3; ++i) {
        bool unlocked = (i == 0) || (i == 1 && player.hasPistol) || (i == 2 && player.hasShotgun);

        SDL_Rect slot = {WINDOW_WIDTH - 216 + i * 66, 30, 56, 36};
        if (unlocked) {
            SDL_SetRenderDrawColor(renderer, 70, 170, 80, 220);
        } else {
            SDL_SetRenderDrawColor(renderer, 75, 75, 75, 220);
        }
        SDL_RenderFillRect(renderer, &slot);

        bool selected = (i == 0 && selectedWeapon == WeaponSelection::Melee) ||
                        (i == 1 && selectedWeapon == WeaponSelection::Pistol) ||
                        (i == 2 && selectedWeapon == WeaponSelection::Shotgun);
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 255, 240, 120, 255);
            SDL_RenderDrawRect(renderer, &slot);
        }
    }

    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    drawPhaseLetter('M', WINDOW_WIDTH - 198, 38, 1);
    drawPhaseLetter('P', WINDOW_WIDTH - 132, 38, 1);
    drawPhaseLetter('S', WINDOW_WIDTH - 66, 38, 1);

    SDL_Rect ammoBack = {WINDOW_WIDTH - 226, 80, 206, 78};
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 180);
    SDL_RenderFillRect(renderer, &ammoBack);

    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    drawWord("AMMO", WINDOW_WIDTH - 208, 84, 1);

    auto drawAmmoPips = [&](int amount, int x, int y, SDL_Color color) {
        int shown = std::min(amount, 10);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        for (int i = 0; i < shown; ++i) {
            SDL_Rect pip = {x + i * 10, y, 8, 8};
            SDL_RenderFillRect(renderer, &pip);
        }
    };

    SDL_SetRenderDrawColor(renderer, 210, 210, 210, 255);
    drawPhaseLetter('P', WINDOW_WIDTH - 208, 104, 1);
    drawPhaseLetter('S', WINDOW_WIDTH - 208, 122, 1);
    drawPhaseLetter('G', WINDOW_WIDTH - 208, 140, 1);

    drawAmmoPips(player.pistolAmmo, WINDOW_WIDTH - 188, 108, SDL_Color{180, 210, 255, 255});
    drawAmmoPips(player.shotgunAmmo, WINDOW_WIDTH - 188, 126, SDL_Color{255, 210, 120, 255});
    drawAmmoPips(player.grenadeCount, WINDOW_WIDTH - 188, 144, SDL_Color{255, 150, 0, 255});

    if (phaseBannerTimer > 0.0f) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect banner = {WINDOW_WIDTH / 2 - 220, 52, 440, 54};
        SDL_SetRenderDrawColor(renderer, 230, 70, 70, 170);
        SDL_RenderFillRect(renderer, &banner);

        SDL_Rect notch = {WINDOW_WIDTH / 2 - 20 + (currentPhase - 1) * 14, 64, 40, 30};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 220);
        SDL_RenderFillRect(renderer, &notch);
    }

    if (isPaused) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect leftBar = {WINDOW_WIDTH / 2 - 28, WINDOW_HEIGHT / 2 - 45, 18, 90};
        SDL_Rect rightBar = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT / 2 - 45, 18, 90};
        SDL_RenderFillRect(renderer, &leftBar);
        SDL_RenderFillRect(renderer, &rightBar);
    }

    if (isGameOver || isVictory) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 190);
        SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(renderer, &overlay);

        if (isVictory) {
            SDL_SetRenderDrawColor(renderer, 70, 220, 100, 255);
            drawWord("YOU WIN", WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 54, 2);
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
            drawWord("NICE WORK", WINDOW_WIDTH / 2 - 78, WINDOW_HEIGHT / 2 - 2, 1);
        } else {
            SDL_SetRenderDrawColor(renderer, 230, 70, 70, 255);
            drawWord("YOU DIED", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 54, 2);
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
            drawWord("PRESS R TO RETRY OR Q TO QUIT", WINDOW_WIDTH / 2 - 228, WINDOW_HEIGHT / 2 - 2, 1);
        }

        SDL_Rect retryButton = getRetryButtonRect();
        SDL_Rect quitButton = getQuitButtonRect();

        SDL_SetRenderDrawColor(renderer, 40, 110, 60, 230);
        SDL_RenderFillRect(renderer, &retryButton);
        SDL_SetRenderDrawColor(renderer, 255, 240, 120, 255);
        SDL_RenderDrawRect(renderer, &retryButton);
        SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
        drawWord("RETRY", retryButton.x + 34, retryButton.y + 11, 1);

        SDL_SetRenderDrawColor(renderer, 120, 40, 40, 230);
        SDL_RenderFillRect(renderer, &quitButton);
        SDL_SetRenderDrawColor(renderer, 255, 240, 120, 255);
        SDL_RenderDrawRect(renderer, &quitButton);
        SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
        drawWord("QUIT", quitButton.x + 42, quitButton.y + 11, 1);

        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        drawWord("CLICK BUTTON OR PRESS R OR Q", WINDOW_WIDTH / 2 - 184, WINDOW_HEIGHT / 2 + 148, 1);
    }

    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    if (playerIdleTexture != nullptr) {
        SDL_DestroyTexture(playerIdleTexture);
        playerIdleTexture = nullptr;
    }

    if (playerRunTexture != nullptr) {
        SDL_DestroyTexture(playerRunTexture);
        playerRunTexture = nullptr;
    }

    if (playerShootTexture != nullptr) {
        SDL_DestroyTexture(playerShootTexture);
        playerShootTexture = nullptr;
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
    isRunning = false;
}

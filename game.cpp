#include "game.h"

#include <cstdlib>
#include <algorithm>
#include <cmath>

Game::Game()
    : window(nullptr), renderer(nullptr), isRunning(false), currentPhase(1), enemiesKilledInPhase(0), bossActive(false), bossSpawnTriggered(false), bossSpawnTimer(0.0f), player{100.0f, 0.0f, 0.0f, 0.0f, 50, 50, false, false, 0.0f, 0.0f, 1, false, false, 0.0f, 0.0f, 1, 100, 0.0f, false, 0, 0, false, 2, false}, boss{900.0f, 0.0f, 120, 120, 100, false} {
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
}

void Game::spawnEnemiesForPhase() {
    const float groundY = static_cast<float>(WINDOW_HEIGHT - 50);

    enemies.clear();

    if (currentPhase == 1) {
        enemies.push_back({600.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
        enemies.push_back({800.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
        enemies.push_back({1000.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
    } else if (currentPhase == 2) {
        enemies.push_back({600.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
        enemies.push_back({800.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 2.0f, false, 0.0f});
        enemies.push_back({1000.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
    } else {
        enemies.push_back({450.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
        enemies.push_back({600.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 2.0f, false, 0.0f});
        enemies.push_back({750.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
        enemies.push_back({900.0f, groundY, 50, 50, true, Enemy::EnemyType::Medium, 2.0f, false, 0.0f});
        enemies.push_back({1050.0f, groundY, 50, 50, true, Enemy::EnemyType::Weak, 2.0f, false, 0.0f});
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

    resetPlayerStateForPhaseStart();

    bossActive = false;
    bossSpawnTriggered = false;
    bossSpawnTimer = 0.0f;
    boss = {900.0f, 0.0f, 120, 120, 50, false};

    projectiles.clear();
    grenades.clear();
    enemiesKilledInPhase = 0;
    currentPhase = 1;
    spawnEnemiesForPhase();
    
    player.hasPistol = false;
    player.hasShotgun = false;
    player.grenadeCount = 2;

    isRunning = true;
    return true;
}

void Game::run() {
    const Uint32 frameDelay = 16; // ~60 FPS target

    while (isRunning) {
        const Uint32 frameStart = SDL_GetTicks();

        handleEvents();
        update();
        render();

        const Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < frameDelay) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                isRunning = false;
            }

            if (event.key.keysym.sym == SDLK_j) {
                if (!player.isAttacking && !player.isDashing && player.attackCooldownTimer <= 0.0f) {
                    player.isAttacking = true;
                    player.attackTimer = 0.1f;
                    player.attackCooldownTimer = 0.2f;
                }
            }

            if (event.key.keysym.sym == SDLK_SPACE && event.key.repeat == 0) {
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

            if (event.key.keysym.sym == SDLK_k) {
                if (player.hasPistol && player.pistolAmmo > 0) {
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
                    SDL_Log("Pistol ammo: %d", player.pistolAmmo);
                }
            }

            if (event.key.keysym.sym == SDLK_l) {
                if (player.hasShotgun) {
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
                }
            }

            if (event.key.keysym.sym == SDLK_g) {
                if (player.grenadeCount > 0) {
                    Grenade g;
                    g.x = player.x;
                    g.y = player.y;
                    g.timer = 1.0f;
                    g.isActive = true;

                    grenades.push_back(g);

                    player.grenadeCount--;
                }
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (!player.isAttacking && !player.isDashing && player.attackCooldownTimer <= 0.0f) {
                player.isAttacking = true;
                player.attackTimer = 0.1f;
                player.attackCooldownTimer = 0.2f;
            }
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

void Game::update() {
    const float moveSpeed = 5.0f;
    const float jumpVelocity = -14.0f;
    const float gravity = 0.7f;
    const float dashSpeed = 18.0f;
    const float dashCooldown = 0.5f;
    const float frameSeconds = 1.0f / 60.0f;

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
        player.hasShotgun = true;
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

        bool jumpPressed = keyboardState[SDL_SCANCODE_SPACE];
        if ((player.jumpRequested || (jumpPressed && !player.jumpPressedLastFrame)) && player.onGround) {
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
    if (player.x + player.width > WINDOW_WIDTH) {
        player.x = static_cast<float>(WINDOW_WIDTH - player.width);
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

    for (auto& enemy : enemies) {
        if (player.isAttacking && enemy.isAlive) {
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
                if (enemy.type == Enemy::EnemyType::Medium) {
                    enemy.isRaging = true;
                    enemy.rageTimer = 1.5f;
                } else {
                    enemy.isAlive = false;
                    player.killCount += 1;
                    enemiesKilledInPhase += 1;
                    if (std::rand() % 2 == 0) {
                        player.pistolAmmo += 2;
                    }
                }
            }
        }
    }

    for (auto& p : projectiles) {
        if (!p.isActive) {
            continue;
        }

        p.x += p.speed * static_cast<float>(p.direction);

        if (p.x < 0.0f || p.x > WINDOW_WIDTH) {
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
                enemy.isAlive = false;
                p.isActive = false;
                player.killCount += 1;
                enemiesKilledInPhase += 1;
                if (std::rand() % 2 == 0) {
                    player.pistolAmmo += 2;
                }
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

    for (auto& g : grenades) {
        if (!g.isActive) {
            continue;
        }

        g.timer -= frameSeconds;

        if (g.timer <= 0.0f) {
            float radius = 100.0f;

            if (bossActive && boss.isAlive) {
                float dx = boss.x - g.x;
                float dy = boss.y - g.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance < radius) {
                    boss.health -= 5;
                }
            }

            for (auto& enemy : enemies) {
                if (!enemy.isAlive) {
                    continue;
                }

                float dx = enemy.x - g.x;
                float dy = enemy.y - g.y;
                float distance = std::sqrt(dx * dx + dy * dy);

                if (distance < radius) {
                    enemy.isAlive = false;
                    player.killCount += 1;
                    enemiesKilledInPhase += 1;
                }
            }

            g.isActive = false;
        }
    }

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
        SDL_Log("YOU WIN");
    }

    if (player.health <= 0) {
        player.x = 100.0f;
        player.y = static_cast<float>(WINDOW_HEIGHT - player.height);
        player.vx = 0.0f;
        player.vy = 0.0f;
        player.health = 100;
        player.isDashing = false;
        player.isAttacking = false;
        player.onGround = true;
        player.dashCooldownTimer = 0.0f;
        player.attackCooldownTimer = 0.0f;
        player.damageCooldownTimer = 0.0f;
        projectiles.clear();
        grenades.clear();
        currentPhase = 1;
        enemiesKilledInPhase = 0;
        bossActive = false;
        bossSpawnTriggered = false;
        bossSpawnTimer = 0.0f;
        boss = {900.0f, 0.0f, 120, 120, 100, false};
        spawnEnemiesForPhase();
    }

    bool allEnemiesDead = true;

    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            allEnemiesDead = false;
            break;
        }
    }

    if (currentPhase == 1 && allEnemiesDead) {
        currentPhase = 2;
        enemiesKilledInPhase = 0;
        player.grenadeCount += 1;
        SDL_Log("Phase: %d", currentPhase);
        resetPlayerStateForPhaseStart();
        spawnEnemiesForPhase();
    }

    if (currentPhase == 2 && allEnemiesDead) {
        currentPhase = 3;
        enemiesKilledInPhase = 0;
        SDL_Log("Phase: %d", currentPhase);
        resetPlayerStateForPhaseStart();
        spawnEnemiesForPhase();
    }

    if (currentPhase == 3 && allEnemiesDead && !bossSpawnTriggered && !bossActive) {
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

    grenades.erase(
        std::remove_if(grenades.begin(), grenades.end(),
            [](const Grenade& g) { return !g.isActive; }),
        grenades.end()
    );
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect playerRect = {
        static_cast<int>(player.x),
        static_cast<int>(player.y),
        player.width,
        player.height
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    for (const auto& enemy : enemies) {
        if (enemy.isAlive) {
            SDL_Rect enemyRect = {
                static_cast<int>(enemy.x),
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

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &attackRect);
    }

    for (const auto& p : projectiles) {
        if (!p.isActive) {
            continue;
        }

        SDL_Rect projectileRect = {
            static_cast<int>(p.x),
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
            static_cast<int>(g.x),
            static_cast<int>(g.y),
            10,
            10
        };

        SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    if (bossActive && boss.isAlive) {
        SDL_Rect bossRect = {
            static_cast<int>(boss.x),
            static_cast<int>(boss.y),
            boss.width,
            boss.height
        };

        SDL_SetRenderDrawColor(renderer, 200, 0, 200, 255);
        SDL_RenderFillRect(renderer, &bossRect);
    }

    SDL_RenderPresent(renderer);
}

void Game::cleanup() {
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    isRunning = false;
}

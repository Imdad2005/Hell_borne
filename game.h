#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <vector>

class Game {
public:
    Game();

    bool init();
    void run();
    void handleEvents();
    void update();
    void render();
    void cleanup();

private:
    struct Player {
        float x;
        float y;
        float vx;
        float vy;
        int width;
        int height;
        bool onGround;
        bool isDashing;
        float dashTimer;
        float dashCooldownTimer;
        int dashDirection;
        bool jumpPressedLastFrame;
        bool isAttacking;
        float attackTimer;
        float attackCooldownTimer;
        int facingDirection;
        int health;
        float damageCooldownTimer;
        bool hasPistol;
        int pistolAmmo;
        int killCount;
        bool hasShotgun;
        int grenadeCount;
        bool jumpRequested;
    };

    struct Enemy {
        enum class EnemyType { Weak, Medium };

        float x;
        float y;
        int width;
        int height;
        bool isAlive;
        EnemyType type;
        float speed;
        bool isRaging;
        float rageTimer;
    };

    struct Projectile {
        float x;
        float y;
        int width;
        int height;
        bool isActive;
        int direction;
        float speed;
    };

    struct Grenade {
        float x;
        float y;
        float timer;
        bool isActive;
    };

    struct Boss {
        float x;
        float y;
        int width;
        int height;
        int health;
        bool isAlive;
    };

    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr int WORLD_WIDTH = 4600;

    SDL_Rect getAttackRect();
    void resetPlayerStateForPhaseStart();
    void spawnEnemiesForPhase();

    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    int currentPhase;
    int enemiesKilledInPhase;
    bool bossActive;
    bool bossSpawnTriggered;
    float bossSpawnTimer;
    float cameraX;
    bool isPaused;
    float phaseBannerTimer;
    float controlsHintTimer;
    Player player;
    Boss boss;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    std::vector<Grenade> grenades;
};

#endif

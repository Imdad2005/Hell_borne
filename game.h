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
    enum class PlayerAnimationState { Idle, Run, Shoot };
    enum class WeaponSelection { Melee, Pistol, Shotgun };

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
        int shotgunAmmo;
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
        int health;
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
        float vx;
        float vy;
        float timer;
        bool isActive;
        int bounceCount;
    };

    struct Boss {
        float x;
        float y;
        int width;
        int height;
        int health;
        int maxHealth;
        bool isAlive;
    };

    static constexpr int WINDOW_WIDTH = 1280;
    static constexpr int WINDOW_HEIGHT = 720;
    static constexpr int WORLD_WIDTH = 4600;

    SDL_Rect getAttackRect();
    SDL_Rect getRetryButtonRect() const;
    SDL_Rect getQuitButtonRect() const;
    bool isPointInRect(int x, int y, const SDL_Rect& rect) const;
    void restartGame();
    void beginPerformanceTracking();
    void printPerformanceReport() const;
    void resetPlayerStateForPhaseStart();
    void spawnEnemiesForPhase();
    void handleWeaponInput(const SDL_Event& event);
    void performSelectedAttack();
    void applyEnemyDamage(Enemy& enemy, int damage, bool triggerRage);
    void applyMeleeDamage();
    void updateProjectiles();
    void updateGrenades(float frameSeconds);
    bool loadPlayerSpriteSheets();
    void updatePlayerAnimation(float frameSeconds);
    void renderPlayerSprite(float cameraX);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* playerIdleTexture;
    SDL_Texture* playerRunTexture;
    SDL_Texture* playerShootTexture;
    int playerIdleFrameCount;
    int playerRunFrameCount;
    int playerShootFrameCount;
    int playerCurrentFrame;
    float playerAnimationTimer;
    PlayerAnimationState playerAnimationState;
    bool isRunning;
    int currentPhase;
    int enemiesKilledInPhase;
    bool bossActive;
    bool bossSpawnTriggered;
    float bossSpawnTimer;
    float cameraX;
    bool isPaused;
    bool showWelcomeScreen;
    bool isGameOver;
    bool isVictory;
    float phaseBannerTimer;
    float controlsHintTimer;
    WeaponSelection selectedWeapon;

    Uint64 perfFreq;
    Uint64 lastInputEventCounter;
    bool hasPendingInputSample;

    double totalFrameMs;
    double totalFrameMsSq;
    double minFrameMs;
    double maxFrameMs;
    double totalWorkMs;
    double totalFixedDriftMs;
    double maxFixedDriftMs;
    double totalInputLatencyMs;
    int inputLatencySamples;
    int totalFrames;
    int droppedFrames;

    size_t prevEnemiesCapacity;
    size_t prevProjectilesCapacity;
    size_t prevGrenadesCapacity;
    int capacityChangeEvents;

    double totalEntitiesProcessed;
    double totalEntitiesProcessedSq;
    int entitySamples;
    int entitiesProcessedThisFrame;

    Player player;
    Boss boss;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    std::vector<Grenade> grenades;
};

#endif

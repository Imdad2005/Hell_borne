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
    void resetPlayerStateForPhaseStart();
    void spawnEnemiesForPhase();
    void handleWeaponInput(const SDL_Event& event);
    void performSelectedAttack();
    void applyEnemyDamage(Enemy& enemy, int damage, bool triggerRage);
    void applyMeleeDamage();
    void updateProjectiles();
    void updateGrenades(float frameSeconds);

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
    bool showWelcomeScreen;
    bool isGameOver;
    bool isVictory;
    float phaseBannerTimer;
    float controlsHintTimer;
    WeaponSelection selectedWeapon;
    Player player;
    Boss boss;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;
    std::vector<Grenade> grenades;
};

#endif

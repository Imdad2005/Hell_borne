#ifndef PLAYER_H
#define PLAYER_H

#include "../utils/Vector2.h"
#include "../utils/AABB.h"
#include "../graphics/Texture.h"
#include "../graphics/AnimationManager.h"
#include "../combat/Weapon.h"
#include "../combat/HealthComponent.h"
#include <SDL.h>
#include <memory>

// Player constants
const float PLAYER_WIDTH = 32.0f;
const float PLAYER_HEIGHT = 48.0f;
const float PLAYER_MOVE_SPEED = 200.0f;
const float PLAYER_JUMP_FORCE = -400.0f;
const float PLAYER_GRAVITY = 980.0f;
const float PLAYER_TERMINAL_VELOCITY = 600.0f;
const float PLAYER_DASH_SPEED = 400.0f;
const float PLAYER_DASH_DURATION = 0.2f;
const float PLAYER_DASH_COOLDOWN = 1.0f;
const float COYOTE_TIME = 0.15f;
const float JUMP_BUFFER_TIME = 0.1f;
const float PLAYER_FRICTION = 0.8f; // Deceleration multiplier

enum PlayerState {
    PLAYER_IDLE,
    PLAYER_RUNNING,
    PLAYER_JUMPING,
    PLAYER_FALLING,
    PLAYER_DASHING,
    PLAYER_ATTACKING,
    PLAYER_HURT,
    PLAYER_DEAD
};

class Player {
private:
    // Position and physics
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    AABB hitbox;
    
    // State
    PlayerState state;
    bool grounded;
    bool facingRight;
    
    // Jump mechanics
    bool canJump;
    bool jumpPressed;
    float coyoteTimer;
    float jumpBufferTimer;
    
    // Dash mechanics
    bool isDashing;
    float dashTimer;
    float dashCooldownTimer;
    float dashDirection; // -1 for left, 1 for right
    
    // Combat
    std::unique_ptr<Weapon> weapon;
    HealthComponent health;
    bool isAttacking;
    float attackTimer;
    AABB attackHitbox;
    
    // Rendering
    Texture sprite;
    AnimationManager animManager;
    SDL_Color fallbackColor;

public:
    Player();
    ~Player();
    
    bool init(SDL_Renderer* renderer);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer, float cameraX = 0.0f, float cameraY = 0.0f);
    
    // Movement controls (called by input handler)
    void moveLeft();
    void moveRight();
    void stopMoving();
    void jump();
    void releaseJump();
    void dash();
    void attack();
    void resetForNewRun();
    
    // Combat
    void takeDamage(int damage);
    AABB getAttackHitbox() const { return attackHitbox; }
    bool isCurrentlyAttacking() const { return isAttacking; }
    int getAttackDamage() const;
    
    // Physics
    void applyGravity(float deltaTime);
    void updatePhysics(float deltaTime);
    
    // Collision
    void checkCollision(const AABB& platform);
    void setGrounded(bool isGrounded);
    
    // Getters
    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    AABB getHitbox() const { return hitbox; }
    bool isGrounded() const { return grounded; }
    PlayerState getState() const { return state; }
    int getHealth() const { return health.getHealth(); }
    int getMaxHealth() const { return health.getMaxHealth(); }
    float getHealthPercent() const { return health.getHealthPercent(); }
    bool isDead() const { return health.isDead(); }
    
    // Setters
    void setPosition(const Vector2& pos);
};

#endif // PLAYER_H

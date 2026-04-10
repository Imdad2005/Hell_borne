#ifndef ENEMY_H
#define ENEMY_H

#include "../utils/Vector2.h"
#include "../utils/AABB.h"
#include "../combat/HealthComponent.h"
#include <SDL.h>

enum EnemyAIState {
    ENEMY_PATROL,
    ENEMY_CHASE,
    ENEMY_DEAD
};

class Enemy {
protected:
    Vector2 position;
    Vector2 velocity;
    AABB hitbox;
    HealthComponent health;
    EnemyAIState aiState;
    SDL_Color color;

    float moveSpeed;
    int contactDamage;
    float contactDamageCooldown;
    float contactDamageTimer;

public:
    Enemy(float x, float y, float width, float height, int maxHealth);
    virtual ~Enemy();

    virtual void update(float deltaTime, const Vector2& playerPos) = 0;
    virtual void render(SDL_Renderer* renderer, float cameraX = 0.0f, float cameraY = 0.0f) const;

    void applyGravity(float deltaTime);
    void updatePhysics(float deltaTime);
    void checkCollision(const AABB& platform);

    void takeDamage(int damage);
    bool isAlive() const { return !health.isDead(); }

    bool intersects(const AABB& other) const { return hitbox.intersects(other); }
    bool canDealContactDamage() const { return contactDamageTimer <= 0.0f && isAlive(); }
    int getContactDamage() const { return contactDamage; }
    void consumeContactDamage();

    Vector2 getPosition() const { return position; }
    AABB getHitbox() const { return hitbox; }
};

class SlimeEnemy : public Enemy {
private:
    float spawnX;
    float patrolDistance;
    float aggroRange;
    bool movingRight;

public:
    SlimeEnemy(float x, float y);
    ~SlimeEnemy();
    void update(float deltaTime, const Vector2& playerPos) override;
};

#endif // ENEMY_H

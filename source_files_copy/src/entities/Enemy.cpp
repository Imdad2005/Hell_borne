#include "Enemy.h"
#include "../core/globals.h"
#include <cmath>
#include <SDL_log.h>

Enemy::Enemy(float x, float y, float width, float height, int maxHealth)
    : position(x, y),
      velocity(0.0f, 0.0f),
      hitbox(x, y, width, height),
      health(maxHealth, 0.2f),
      aiState(ENEMY_PATROL),
      color({180, 40, 180, 255}),
      moveSpeed(90.0f),
      contactDamage(8),
      contactDamageCooldown(0.6f),
      contactDamageTimer(0.0f) {
}

Enemy::~Enemy() {
}

void Enemy::render(SDL_Renderer* renderer, float cameraX, float cameraY) const {
    if (!isAlive()) {
        return;
    }

    SDL_Rect rect = {
        static_cast<int>(position.x - cameraX),
        static_cast<int>(position.y - cameraY),
        static_cast<int>(hitbox.w),
        static_cast<int>(hitbox.h)
    };

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // Tiny health bar
    SDL_Rect bg = {rect.x, rect.y - 6, rect.w, 4};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bg);

    int hpW = static_cast<int>(rect.w * health.getHealthPercent());
    SDL_Rect hp = {rect.x, rect.y - 6, hpW, 4};
    SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
    SDL_RenderFillRect(renderer, &hp);
}

void Enemy::applyGravity(float deltaTime) {
    velocity.y += GRAVITY * deltaTime;
    if (velocity.y > MAX_FALL_SPEED) {
        velocity.y = MAX_FALL_SPEED;
    }
}

void Enemy::updatePhysics(float deltaTime) {
    health.update(deltaTime);

    if (contactDamageTimer > 0.0f) {
        contactDamageTimer -= deltaTime;
        if (contactDamageTimer < 0.0f) {
            contactDamageTimer = 0.0f;
        }
    }

    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;

    hitbox.x = position.x;
    hitbox.y = position.y;
}

void Enemy::checkCollision(const AABB& platform) {
    if (!isAlive()) {
        return;
    }

    if (hitbox.intersects(platform) && velocity.y >= 0.0f) {
        float enemyBottom = position.y + hitbox.h;
        if (enemyBottom >= platform.y && enemyBottom <= platform.y + 14.0f) {
            position.y = platform.y - hitbox.h;
            velocity.y = 0.0f;
            hitbox.y = position.y;
        }
    }
}

void Enemy::takeDamage(int damage) {
    if (!isAlive()) {
        return;
    }
    health.takeDamage(damage);
    if (!isAlive()) {
        aiState = ENEMY_DEAD;
        SDL_Log("Enemy defeated");
    }
}

void Enemy::consumeContactDamage() {
    contactDamageTimer = contactDamageCooldown;
}

SlimeEnemy::SlimeEnemy(float x, float y)
    : Enemy(x, y, 36.0f, 28.0f, 40),
      spawnX(x),
      patrolDistance(140.0f),
      aggroRange(220.0f),
      movingRight(true) {
    color = {120, 230, 120, 255};
    moveSpeed = 75.0f;
    contactDamage = 10;
}

SlimeEnemy::~SlimeEnemy() {
}

void SlimeEnemy::update(float deltaTime, const Vector2& playerPos) {
    if (!isAlive()) {
        velocity.x = 0.0f;
        return;
    }

    float dx = playerPos.x - position.x;
    float absDx = std::fabs(dx);

    if (absDx <= aggroRange) {
        aiState = ENEMY_CHASE;
    } else {
        aiState = ENEMY_PATROL;
    }

    if (aiState == ENEMY_CHASE) {
        velocity.x = (dx >= 0.0f) ? moveSpeed : -moveSpeed;
    } else {
        if (movingRight) {
            velocity.x = moveSpeed * 0.6f;
            if (position.x >= spawnX + patrolDistance) {
                movingRight = false;
            }
        } else {
            velocity.x = -moveSpeed * 0.6f;
            if (position.x <= spawnX - patrolDistance) {
                movingRight = true;
            }
        }
    }

    applyGravity(deltaTime);
    updatePhysics(deltaTime);
}

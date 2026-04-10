#include "EnemySpawner.h"

EnemySpawner::EnemySpawner() {
}

EnemySpawner::~EnemySpawner() {
}

void EnemySpawner::clear() {
    enemies.clear();
}

void EnemySpawner::spawnTestEnemies() {
    enemies.push_back(std::unique_ptr<Enemy>(new SlimeEnemy(620.0f, 260.0f)));
    enemies.push_back(std::unique_ptr<Enemy>(new SlimeEnemy(260.0f, 320.0f)));
    enemies.push_back(std::unique_ptr<Enemy>(new SlimeEnemy(-120.0f, 420.0f)));
}

void EnemySpawner::update(float deltaTime, const Vector2& playerPos) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i]->update(deltaTime, playerPos);
    }
}

void EnemySpawner::render(SDL_Renderer* renderer, float cameraX, float cameraY) const {
    for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i]->render(renderer, cameraX, cameraY);
    }
}

void EnemySpawner::applyPlatformCollisions(const std::vector<AABB>& platforms) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        for (size_t p = 0; p < platforms.size(); ++p) {
            enemies[i]->checkCollision(platforms[p]);
        }
    }
}

void EnemySpawner::handlePlayerAttack(const AABB& attackHitbox, int damage) {
    if (attackHitbox.w <= 0 || attackHitbox.h <= 0) {
        return;
    }

    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isAlive() && enemies[i]->intersects(attackHitbox)) {
            enemies[i]->takeDamage(damage);
        }
    }
}

int EnemySpawner::handlePlayerContactDamage(const AABB& playerHitbox) {
    int totalDamage = 0;
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isAlive() &&
            enemies[i]->canDealContactDamage() &&
            enemies[i]->intersects(playerHitbox)) {
            totalDamage += enemies[i]->getContactDamage();
            enemies[i]->consumeContactDamage();
        }
    }
    return totalDamage;
}

int EnemySpawner::getAliveCount() const {
    int count = 0;
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i]->isAlive()) {
            ++count;
        }
    }
    return count;
}

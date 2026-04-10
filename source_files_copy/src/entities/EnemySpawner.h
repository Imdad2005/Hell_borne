#ifndef ENEMY_SPAWNER_H
#define ENEMY_SPAWNER_H

#include "Enemy.h"
#include <memory>
#include <vector>

class EnemySpawner {
private:
    std::vector<std::unique_ptr<Enemy> > enemies;

public:
    EnemySpawner();
    ~EnemySpawner();

    void clear();
    void spawnTestEnemies();
    void update(float deltaTime, const Vector2& playerPos);
    void render(SDL_Renderer* renderer, float cameraX = 0.0f, float cameraY = 0.0f) const;
    void applyPlatformCollisions(const std::vector<AABB>& platforms);

    void handlePlayerAttack(const AABB& attackHitbox, int damage);
    int handlePlayerContactDamage(const AABB& playerHitbox);
    int getAliveCount() const;
};

#endif // ENEMY_SPAWNER_H

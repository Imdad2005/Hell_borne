#ifndef WEAPON_H
#define WEAPON_H

#include "../utils/AABB.h"
#include "../utils/Vector2.h"
#include <SDL.h>
#include <string>

enum WeaponType {
    WEAPON_MELEE,
    WEAPON_RANGED
};

class Weapon {
protected:
    std::string name;
    WeaponType type;
    int damage;
    float range;
    float attackCooldown;
    float cooldownTimer;

public:
    Weapon(const std::string& weaponName, WeaponType weaponType, int dmg, float rng, float cooldown);
    virtual ~Weapon();

    virtual void update(float deltaTime);
    virtual AABB getAttackHitbox(const Vector2& playerPos, bool facingRight) = 0;
    virtual bool canAttack() const;
    virtual void attack();
    
    // Getters
    std::string getName() const { return name; }
    WeaponType getType() const { return type; }
    int getDamage() const { return damage; }
    float getRange() const { return range; }
    bool isOnCooldown() const { return cooldownTimer > 0.0f; }
};

// Melee weapon (sword, axe, etc.)
class MeleeWeapon : public Weapon {
private:
    float hitboxWidth;
    float hitboxHeight;

public:
    MeleeWeapon(const std::string& name, int damage, float range, float cooldown);
    ~MeleeWeapon();

    AABB getAttackHitbox(const Vector2& playerPos, bool facingRight) override;
};

#endif // WEAPON_H

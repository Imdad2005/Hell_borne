#include "Weapon.h"

// Base Weapon class
Weapon::Weapon(const std::string& weaponName, WeaponType weaponType, int dmg, float rng, float cooldown)
    : name(weaponName), type(weaponType), damage(dmg), range(rng), 
      attackCooldown(cooldown), cooldownTimer(0.0f) {
}

Weapon::~Weapon() {
}

void Weapon::update(float deltaTime) {
    if (cooldownTimer > 0.0f) {
        cooldownTimer -= deltaTime;
        if (cooldownTimer < 0.0f) {
            cooldownTimer = 0.0f;
        }
    }
}

bool Weapon::canAttack() const {
    return cooldownTimer <= 0.0f;
}

void Weapon::attack() {
    if (canAttack()) {
        cooldownTimer = attackCooldown;
    }
}

// Melee Weapon class
MeleeWeapon::MeleeWeapon(const std::string& name, int damage, float range, float cooldown)
    : Weapon(name, WEAPON_MELEE, damage, range, cooldown),
      hitboxWidth(range), hitboxHeight(48.0f) {
}

MeleeWeapon::~MeleeWeapon() {
}

AABB MeleeWeapon::getAttackHitbox(const Vector2& playerPos, bool facingRight) {
    // Create hitbox in front of player
    float hitboxX = facingRight ? playerPos.x + 32 : playerPos.x - hitboxWidth;
    float hitboxY = playerPos.y;
    
    return AABB(hitboxX, hitboxY, hitboxWidth, hitboxHeight);
}

#include "HealthComponent.h"
#include <SDL_log.h>
#include <algorithm>

HealthComponent::HealthComponent(int maxHp, float invincDuration)
    : maxHealth(maxHp), currentHealth(maxHp), 
      invincibilityTimer(0.0f), invincibilityDuration(invincDuration),
      isAlive(true) {
}

HealthComponent::~HealthComponent() {
}

void HealthComponent::update(float deltaTime) {
    if (invincibilityTimer > 0.0f) {
        invincibilityTimer -= deltaTime;
        if (invincibilityTimer < 0.0f) {
            invincibilityTimer = 0.0f;
        }
    }
}

void HealthComponent::takeDamage(int damage) {
    if (!isAlive || invincibilityTimer > 0.0f) {
        return; // Already dead or invincible
    }
    
    currentHealth -= damage;
    
    if (currentHealth <= 0) {
        currentHealth = 0;
        isAlive = false;
        SDL_Log("Entity died!");
    } else {
        // Grant invincibility frames after taking damage
        invincibilityTimer = invincibilityDuration;
        SDL_Log("Took %d damage! Health: %d/%d", damage, currentHealth, maxHealth);
    }
}

void HealthComponent::heal(int amount) {
    if (!isAlive) return;
    
    currentHealth += amount;
    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
    SDL_Log("Healed %d! Health: %d/%d", amount, currentHealth, maxHealth);
}

void HealthComponent::reset() {
    currentHealth = maxHealth;
    invincibilityTimer = 0.0f;
    isAlive = true;
}

float HealthComponent::getHealthPercent() const {
    return static_cast<float>(currentHealth) / static_cast<float>(maxHealth);
}

void HealthComponent::setMaxHealth(int maxHp) {
    maxHealth = maxHp;
    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
}

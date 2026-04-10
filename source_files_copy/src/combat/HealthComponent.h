#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

class HealthComponent {
private:
    int maxHealth;
    int currentHealth;
    float invincibilityTimer;
    float invincibilityDuration;
    bool isAlive;

public:
    HealthComponent(int maxHp = 100, float invincDuration = 0.5f);
    ~HealthComponent();

    void update(float deltaTime);
    void takeDamage(int damage);
    void heal(int amount);
    void reset();
    
    // Getters
    int getHealth() const { return currentHealth; }
    int getMaxHealth() const { return maxHealth; }
    float getHealthPercent() const;
    bool isDead() const { return !isAlive; }
    bool isInvincible() const { return invincibilityTimer > 0.0f; }
    
    // Setters
    void setMaxHealth(int maxHp);
};

#endif // HEALTH_COMPONENT_H

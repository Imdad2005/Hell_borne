#include "Player.h"
#include <SDL_log.h>
#include <cmath>

Player::Player() 
    : position(400.0f, 300.0f), // Start in middle of screen
      velocity(0.0f, 0.0f),
      acceleration(0.0f, 0.0f),
      hitbox(position.x, position.y, PLAYER_WIDTH, PLAYER_HEIGHT),
      state(PLAYER_IDLE),
      grounded(false),
      facingRight(true),
      canJump(false),
      jumpPressed(false),
      coyoteTimer(0.0f),
      jumpBufferTimer(0.0f),
      isDashing(false),
      dashTimer(0.0f),
      dashCooldownTimer(0.0f),
      dashDirection(1.0f),
      health(100, 0.5f),
      isAttacking(false),
      attackTimer(0.0f),
      attackHitbox(0, 0, 0, 0),
      fallbackColor({0, 150, 255, 255}) { // Blue player
    
    // Create starting weapon (basic sword) - C++11 compatible unique_ptr
    weapon.reset(new MeleeWeapon("Sword", 25, 50.0f, 0.4f));
}

Player::~Player() {
    sprite.free();
}

bool Player::init(SDL_Renderer* renderer) {
    // Try to load player sprite (fallback to colored rectangle if missing)
    if (!sprite.loadFromFile(renderer, "assets/sprites/player.png")) {
        SDL_Log("Player sprite not found - using fallback rendering");
    }
    
    // Setup animations
    Animation idleAnim(1, 0.1f, true);   // 1 frame, static
    Animation runAnim(6, 0.1f, true);    // 6 frames, 10 FPS
    Animation jumpAnim(3, 0.1f, false);  // 3 frames, one-shot
    Animation fallAnim(2, 0.1f, true);   // 2 frames, looping
    Animation dashAnim(4, 0.05f, false); // 4 frames, fast, one-shot
    
    // Set frame sizes (32x48 player sprite)
    idleAnim.setFrameSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    runAnim.setFrameSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    jumpAnim.setFrameSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    fallAnim.setFrameSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    dashAnim.setFrameSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    
    // Add to animation manager
    animManager.addAnimation("idle", idleAnim);
    animManager.addAnimation("run", runAnim);
    animManager.addAnimation("jump", jumpAnim);
    animManager.addAnimation("fall", fallAnim);
    animManager.addAnimation("dash", dashAnim);
    
    SDL_Log("Player initialized at (%.1f, %.1f)", position.x, position.y);
    return true;
}

void Player::update(float deltaTime) {
    // Update health component
    health.update(deltaTime);
    
    // Update weapon
    if (weapon) {
        weapon->update(deltaTime);
    }
    
    // Update timers
    if (coyoteTimer > 0.0f) {
        coyoteTimer -= deltaTime;
    }
    
    if (jumpBufferTimer > 0.0f) {
        jumpBufferTimer -= deltaTime;
    }
    
    if (dashCooldownTimer > 0.0f) {
        dashCooldownTimer -= deltaTime;
    }
    
    // Handle attacking
    if (isAttacking) {
        attackTimer -= deltaTime;
        if (attackTimer <= 0.0f) {
            isAttacking = false;
        }
    }
    
    // Handle dashing
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0.0f) {
            isDashing = false;
            velocity.x = 0.0f; // Stop at end of dash
        }
    }
    
    // Apply physics (gravity, velocity, position update)
    if (!isDashing) {
        applyGravity(deltaTime);
    }
    updatePhysics(deltaTime);
    
    // Update hitbox position
    hitbox.x = position.x;
    hitbox.y = position.y;
    
    // Update state based on conditions
    if (health.isDead()) {
        state = PLAYER_DEAD;
    } else if (isAttacking) {
        state = PLAYER_ATTACKING;
        animManager.setAnimation("idle"); // Will add attack animation later
    } else if (isDashing) {
        state = PLAYER_DASHING;
        animManager.setAnimation("dash");
    } else if (!grounded && velocity.y < 0) {
        state = PLAYER_JUMPING;
        animManager.setAnimation("jump");
    } else if (!grounded && velocity.y > 0) {
        state = PLAYER_FALLING;
        animManager.setAnimation("fall");
    } else if (fabs(velocity.x) > 1.0f) {
        state = PLAYER_RUNNING;
        animManager.setAnimation("run");
    } else {
        state = PLAYER_IDLE;
        animManager.setAnimation("idle");
    }
    
    // Update attack hitbox if attacking
    if (isAttacking && weapon) {
        attackHitbox = weapon->getAttackHitbox(position, facingRight);
    } else {
        attackHitbox = AABB(0, 0, 0, 0); // No hitbox when not attacking
    }
    
    // Update current animation
    animManager.update(deltaTime);
}

void Player::render(SDL_Renderer* renderer, float cameraX, float cameraY) {
    SDL_Rect destRect = {
        static_cast<int>(position.x - cameraX),
        static_cast<int>(position.y - cameraY),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT)
    };
    
    if (sprite.isLoaded()) {
        // Get current animation frame
        SDL_Rect srcRect = animManager.getCurrentFrameRect();
        
        // Render sprite sheet frame using Texture helper
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        sprite.render(renderer, destRect.x, destRect.y, &srcRect, 0.0, nullptr, flip);
    } else {
        // Fallback: colored rectangle
        SDL_SetRenderDrawColor(renderer, 
            fallbackColor.r, fallbackColor.g, fallbackColor.b, fallbackColor.a);
        SDL_RenderFillRect(renderer, &destRect);
        
        // Draw direction indicator (small rectangle on facing side)
        SDL_Rect dirRect = {
            facingRight ? destRect.x + destRect.w - 4 : destRect.x,
            destRect.y + 10,
            4,
            10
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow indicator
        SDL_RenderFillRect(renderer, &dirRect);
    }
}

void Player::moveLeft() {
    if (isDashing) return; // Can't control during dash
    
    velocity.x = -PLAYER_MOVE_SPEED;
    facingRight = false;
}

void Player::moveRight() {
    if (isDashing) return; // Can't control during dash
    
    velocity.x = PLAYER_MOVE_SPEED;
    facingRight = true;
}

void Player::stopMoving() {
    if (isDashing) return; // Can't control during dash
    
    // Apply friction
    velocity.x *= PLAYER_FRICTION;
    if (fabs(velocity.x) < 1.0f) {
        velocity.x = 0.0f;
    }
}

void Player::jump() {
    jumpPressed = true;
    jumpBufferTimer = JUMP_BUFFER_TIME;
    
    // Can jump if grounded OR within coyote time
    bool canCoyoteJump = !grounded && coyoteTimer > 0.0f;
    
    if ((grounded || canCoyoteJump) && canJump) {
        velocity.y = PLAYER_JUMP_FORCE;
        grounded = false;
        canJump = false;
        coyoteTimer = 0.0f; // Consume coyote time
        SDL_Log("Player jumped!");
    }
}

void Player::releaseJump() {
    jumpPressed = false;
    
    // Variable jump height: cut upward velocity when jump released early
    if (velocity.y < 0.0f) {
        velocity.y *= 0.5f;
    }
}

void Player::dash() {
    // Can only dash if not already dashing and cooldown expired
    if (isDashing || dashCooldownTimer > 0.0f) {
        return;
    }
    
    // Dash in facing direction
    dashDirection = facingRight ? 1.0f : -1.0f;
    velocity.x = dashDirection * PLAYER_DASH_SPEED;
    velocity.y = 0.0f; // Cancel vertical velocity
    
    isDashing = true;
    dashTimer = PLAYER_DASH_DURATION;
    dashCooldownTimer = PLAYER_DASH_COOLDOWN;
    
    SDL_Log("Player dashed %s!", facingRight ? "right" : "left");
}

void Player::applyGravity(float deltaTime) {
    // Apply gravity acceleration
    velocity.y += PLAYER_GRAVITY * deltaTime;
    
    // Clamp to terminal velocity
    if (velocity.y > PLAYER_TERMINAL_VELOCITY) {
        velocity.y = PLAYER_TERMINAL_VELOCITY;
    }
}

void Player::updatePhysics(float deltaTime) {
    // Update position based on velocity
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    
    // Screen bounds (simple wrapping for now)
    if (position.x < 0) position.x = 0;
    if (position.x > 800 - PLAYER_WIDTH) position.x = 800 - PLAYER_WIDTH;
}

void Player::checkCollision(const AABB& platform) {
    // Calculate overlap on all sides
    float playerLeft = position.x;
    float playerRight = position.x + PLAYER_WIDTH;
    float playerTop = position.y;
    float playerBottom = position.y + PLAYER_HEIGHT;
    
    float platformLeft = platform.x;
    float platformRight = platform.x + platform.w;
    float platformTop = platform.y;
    float platformBottom = platform.y + platform.h;
    
    // Check if there's any overlap
    if (playerRight > platformLeft && playerLeft < platformRight &&
        playerBottom > platformTop && playerTop < platformBottom) {
        
        // Calculate overlap amounts on each axis
        float overlapLeft = playerRight - platformLeft;
        float overlapRight = platformRight - playerLeft;
        float overlapTop = playerBottom - platformTop;
        float overlapBottom = platformBottom - playerTop;
        
        // Find the smallest overlap (this is the direction to resolve)
        float minOverlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
        float minOverlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;
        
        // One-way platforms: Only resolve if coming from above (falling down)
        if (velocity.y >= 0 && overlapTop < overlapBottom && overlapTop < 15.0f) {
            // Landing on top of platform
            position.y = platformTop - PLAYER_HEIGHT;
            velocity.y = 0.0f;
            hitbox.y = position.y;
            setGrounded(true);
        }
    }
}

void Player::setGrounded(bool isGrounded) {
    bool wasGrounded = grounded;
    grounded = isGrounded;
    
    if (grounded) {
        canJump = true;
        coyoteTimer = 0.0f;
        
        // Jump buffering: if jump was pressed recently, execute it now
        if (jumpBufferTimer > 0.0f) {
            jump();
            jumpBufferTimer = 0.0f;
        }
    } else if (wasGrounded && !grounded) {
        // Just left ground - start coyote timer
        coyoteTimer = COYOTE_TIME;
    }
}

void Player::setPosition(const Vector2& pos) {
    position = pos;
    hitbox.x = position.x;
    hitbox.y = position.y;
}

void Player::attack() {
    if (!weapon || !weapon->canAttack() || isDashing || isAttacking) {
        return; // Can't attack while dashing, already attacking, or weapon on cooldown
    }
    
    weapon->attack();
    isAttacking = true;
    attackTimer = 0.3f; // Attack animation duration
    
    SDL_Log("Player attacked with %s!", weapon->getName().c_str());
}

void Player::takeDamage(int damage) {
    health.takeDamage(damage);
    
    if (health.isDead()) {
        SDL_Log("Player died!");
    }
}

int Player::getAttackDamage() const {
    return weapon ? weapon->getDamage() : 0;
}

void Player::resetForNewRun() {
    health.reset();
    isAttacking = false;
    isDashing = false;
    velocity = Vector2(0.0f, 0.0f);
    attackHitbox = AABB(0, 0, 0, 0);
    state = PLAYER_IDLE;
}


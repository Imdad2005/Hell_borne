#include "Camera.h"
#include <algorithm>

Camera::Camera() 
    : position(0.0f, 0.0f),
      targetPosition(0.0f, 0.0f),
      smoothing(5.0f),
      worldMinX(0.0f),
      worldMaxX(10000.0f),
      worldMinY(0.0f),
      worldMaxY(10000.0f),
      boundsEnabled(false) {
}

Camera::~Camera() {
}

void Camera::update(float deltaTime) {
    // Smooth lerp towards target
    float lerpFactor = 1.0f - exp(-smoothing * deltaTime);
    
    position.x += (targetPosition.x - position.x) * lerpFactor;
    position.y += (targetPosition.y - position.y) * lerpFactor;
    
    // Apply bounds if enabled
    if (boundsEnabled) {
        if (position.x < worldMinX) position.x = worldMinX;
        if (position.x + SCREEN_WIDTH > worldMaxX) position.x = worldMaxX - SCREEN_WIDTH;
        if (position.y < worldMinY) position.y = worldMinY;
        if (position.y + SCREEN_HEIGHT > worldMaxY) position.y = worldMaxY - SCREEN_HEIGHT;
    }
}

void Camera::followTarget(const Vector2& target) {
    // Center camera on target
    targetPosition.x = target.x - SCREEN_WIDTH / 2.0f;
    targetPosition.y = target.y - SCREEN_HEIGHT / 2.0f;
}

void Camera::setPosition(const Vector2& pos) {
    position = pos;
    targetPosition = pos;
}

void Camera::setBounds(float minX, float maxX, float minY, float maxY) {
    worldMinX = minX;
    worldMaxX = maxX;
    worldMinY = minY;
    worldMaxY = maxY;
    boundsEnabled = true;
}

Vector2 Camera::worldToScreen(const Vector2& worldPos) const {
    return Vector2(worldPos.x - position.x, worldPos.y - position.y);
}

Vector2 Camera::screenToWorld(const Vector2& screenPos) const {
    return Vector2(screenPos.x + position.x, screenPos.y + position.y);
}

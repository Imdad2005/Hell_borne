#ifndef CAMERA_H
#define CAMERA_H

#include "../utils/Vector2.h"
#include "../core/globals.h"

class Camera {
private:
    Vector2 position;         // Camera top-left position in world space
    Vector2 targetPosition;   // Target position for smooth following
    float smoothing;          // Camera smoothing factor (0=instant, higher=smoother)
    
    // World bounds
    float worldMinX, worldMaxX;
    float worldMinY, worldMaxY;
    bool boundsEnabled;

public:
    Camera();
    ~Camera();

    void update(float deltaTime);
    void followTarget(const Vector2& target);
    void setPosition(const Vector2& pos);
    void setBounds(float minX, float maxX, float minY, float maxY);
    void setSmoothing(float smooth) { smoothing = smooth; }
    
    // Coordinate conversion
    Vector2 worldToScreen(const Vector2& worldPos) const;
    Vector2 screenToWorld(const Vector2& screenPos) const;
    
    // Getters
    Vector2 getPosition() const { return position; }
    float getX() const { return position.x; }
    float getY() const { return position.y; }
};

#endif // CAMERA_H

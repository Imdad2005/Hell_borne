#ifndef AABB_H
#define AABB_H

#include "Vector2.h"

struct AABB {
    float x, y, w, h;

    AABB() : x(0), y(0), w(0), h(0) {}
    AABB(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

    bool intersects(const AABB& other) const {
        return !(x + w < other.x || x > other.x + other.w ||
                 y + h < other.y || y > other.y + other.h);
    }

    bool contains(float px, float py) const {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }

    bool contains(const Vector2& point) const {
        return contains(point.x, point.y);
    }

    Vector2 center() const {
        return Vector2(x + w / 2.0f, y + h / 2.0f);
    }

    float left() const { return x; }
    float right() const { return x + w; }
    float top() const { return y; }
    float bottom() const { return y + h; }
};

#endif // AABB_H

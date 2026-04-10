#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "Animation.h"
#include <string>
#include <unordered_map>

class AnimationManager {
private:
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;

public:
    AnimationManager();
    ~AnimationManager();

    // Add/manage animations
    void addAnimation(const std::string& name, const Animation& anim);
    void setAnimation(const std::string& name);
    void update(float deltaTime);
    
    // Getters
    Animation* getCurrentAnimation();
    const Animation* getCurrentAnimation() const;
    SDL_Rect getCurrentFrameRect() const;
    std::string getCurrentAnimationName() const { return currentAnimation; }
    bool hasAnimation(const std::string& name) const;
};

#endif // ANIMATION_MANAGER_H

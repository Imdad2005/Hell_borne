#include "AnimationManager.h"
#include <SDL_log.h>

AnimationManager::AnimationManager() 
    : currentAnimation("") {
}

AnimationManager::~AnimationManager() {
}

void AnimationManager::addAnimation(const std::string& name, const Animation& anim) {
    animations[name] = anim;
    
    // Set as current if this is the first animation
    if (currentAnimation.empty()) {
        currentAnimation = name;
    }
}

void AnimationManager::setAnimation(const std::string& name) {
    // Only switch if animation exists and is different
    if (animations.find(name) != animations.end() && currentAnimation != name) {
        currentAnimation = name;
        animations[currentAnimation].reset(); // Reset to first frame
    }
}

void AnimationManager::update(float deltaTime) {
    if (!currentAnimation.empty() && animations.find(currentAnimation) != animations.end()) {
        animations[currentAnimation].update(deltaTime);
    }
}

Animation* AnimationManager::getCurrentAnimation() {
    if (!currentAnimation.empty() && animations.find(currentAnimation) != animations.end()) {
        return &animations[currentAnimation];
    }
    return nullptr;
}

const Animation* AnimationManager::getCurrentAnimation() const {
    if (!currentAnimation.empty() && animations.find(currentAnimation) != animations.end()) {
        return &animations.at(currentAnimation);
    }
    return nullptr;
}

SDL_Rect AnimationManager::getCurrentFrameRect() const {
    const Animation* anim = getCurrentAnimation();
    if (anim) {
        return anim->getFrameRect();
    }
    return {0, 0, 32, 48}; // Default frame
}

bool AnimationManager::hasAnimation(const std::string& name) const {
    return animations.find(name) != animations.end();
}

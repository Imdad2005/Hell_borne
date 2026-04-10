#include "Animation.h"

Animation::Animation() 
    : frameCount(1), currentFrame(0), frameDuration(0.1f), 
      frameTimer(0.0f), looping(true), finished(false),
      frameWidth(32), frameHeight(48) {
}

Animation::Animation(int frames, float duration, bool loop)
    : frameCount(frames), currentFrame(0), frameDuration(duration),
      frameTimer(0.0f), looping(loop), finished(false),
      frameWidth(32), frameHeight(48) {
}

Animation::~Animation() {
}

void Animation::update(float deltaTime) {
    if (finished && !looping) {
        return; // Animation is done and not looping
    }
    
    frameTimer += deltaTime;
    
    if (frameTimer >= frameDuration) {
        frameTimer -= frameDuration;
        currentFrame++;
        
        if (currentFrame >= frameCount) {
            if (looping) {
                currentFrame = 0; // Loop back to start
            } else {
                currentFrame = frameCount - 1; // Stay on last frame
                finished = true;
            }
        }
    }
}

void Animation::reset() {
    currentFrame = 0;
    frameTimer = 0.0f;
    finished = false;
}

SDL_Rect Animation::getFrameRect() const {
    // Assumes horizontal sprite sheet layout
    return {
        currentFrame * frameWidth,  // x
        0,                          // y
        frameWidth,                 // w
        frameHeight                 // h
    };
}

void Animation::setFrameSize(int width, int height) {
    frameWidth = width;
    frameHeight = height;
}

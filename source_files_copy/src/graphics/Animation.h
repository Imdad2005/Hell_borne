#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <SDL.h>

class Animation {
private:
    int frameCount;
    int currentFrame;
    float frameDuration;  // Time per frame in seconds
    float frameTimer;
    bool looping;
    bool finished;
    int frameWidth;
    int frameHeight;

public:
    Animation();
    Animation(int frames, float duration, bool loop = true);
    ~Animation();

    void update(float deltaTime);
    void reset();
    
    // Getters
    int getCurrentFrame() const { return currentFrame; }
    bool isFinished() const { return finished; }
    int getFrameCount() const { return frameCount; }
    SDL_Rect getFrameRect() const;
    
    // Setters
    void setFrameSize(int width, int height);
    void setFrameDuration(float duration) { frameDuration = duration; }
    void setLooping(bool loop) { looping = loop; }
};

#endif // ANIMATION_H

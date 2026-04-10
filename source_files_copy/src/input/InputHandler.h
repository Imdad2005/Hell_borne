#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <SDL.h>
#include <unordered_map>

class InputHandler {
private:
    // Keyboard state (current and previous frame)
    std::unordered_map<SDL_Scancode, bool> currentKeys;
    std::unordered_map<SDL_Scancode, bool> previousKeys;
    
    // Mouse state
    int mouseX, mouseY;
    bool mouseLeftCurrent, mouseLeftPrevious;
    bool mouseRightCurrent, mouseRightPrevious;

public:
    InputHandler();
    ~InputHandler();

    // Call at start of each frame to update state
    void update();
    
    // Keyboard queries
    bool isKeyDown(SDL_Scancode key) const;
    bool wasKeyPressed(SDL_Scancode key) const;  // Pressed this frame
    bool wasKeyReleased(SDL_Scancode key) const; // Released this frame
    
    // Mouse queries
    bool isMouseButtonDown(int button) const; // SDL_BUTTON_LEFT or SDL_BUTTON_RIGHT
    bool wasMouseButtonPressed(int button) const;
    bool wasMouseButtonReleased(int button) const;
    void getMousePosition(int& x, int& y) const;

private:
    void updateKeyState(SDL_Scancode key, bool pressed);
    void updateMouseButton(int button, bool pressed);
};

#endif // INPUT_HANDLER_H

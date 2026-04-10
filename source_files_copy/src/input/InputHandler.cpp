#include "InputHandler.h"

InputHandler::InputHandler() 
    : mouseX(0), mouseY(0), 
      mouseLeftCurrent(false), mouseLeftPrevious(false),
      mouseRightCurrent(false), mouseRightPrevious(false) {
}

InputHandler::~InputHandler() {
}

void InputHandler::update() {
    // Save previous frame state
    previousKeys = currentKeys;
    mouseLeftPrevious = mouseLeftCurrent;
    mouseRightPrevious = mouseRightCurrent;
    
    // Clear current state (we'll rebuild it from SDL)
    currentKeys.clear();
    
    // Get current keyboard state from SDL
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Update all scancodes we care about
    // Movement keys
    currentKeys[SDL_SCANCODE_A] = keyState[SDL_SCANCODE_A];
    currentKeys[SDL_SCANCODE_D] = keyState[SDL_SCANCODE_D];
    currentKeys[SDL_SCANCODE_W] = keyState[SDL_SCANCODE_W];
    currentKeys[SDL_SCANCODE_S] = keyState[SDL_SCANCODE_S];
    currentKeys[SDL_SCANCODE_LEFT] = keyState[SDL_SCANCODE_LEFT];
    currentKeys[SDL_SCANCODE_RIGHT] = keyState[SDL_SCANCODE_RIGHT];
    currentKeys[SDL_SCANCODE_UP] = keyState[SDL_SCANCODE_UP];
    currentKeys[SDL_SCANCODE_DOWN] = keyState[SDL_SCANCODE_DOWN];
    
    // Action keys
    currentKeys[SDL_SCANCODE_SPACE] = keyState[SDL_SCANCODE_SPACE];
    currentKeys[SDL_SCANCODE_LSHIFT] = keyState[SDL_SCANCODE_LSHIFT];
    currentKeys[SDL_SCANCODE_RSHIFT] = keyState[SDL_SCANCODE_RSHIFT];
    currentKeys[SDL_SCANCODE_J] = keyState[SDL_SCANCODE_J];
    currentKeys[SDL_SCANCODE_E] = keyState[SDL_SCANCODE_E];
    currentKeys[SDL_SCANCODE_ESCAPE] = keyState[SDL_SCANCODE_ESCAPE];
    
    // Get current mouse state
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    mouseLeftCurrent = (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    mouseRightCurrent = (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
}

bool InputHandler::isKeyDown(SDL_Scancode key) const {
    auto it = currentKeys.find(key);
    return (it != currentKeys.end()) ? it->second : false;
}

bool InputHandler::wasKeyPressed(SDL_Scancode key) const {
    bool current = isKeyDown(key);
    auto it = previousKeys.find(key);
    bool previous = (it != previousKeys.end()) ? it->second : false;
    return current && !previous;
}

bool InputHandler::wasKeyReleased(SDL_Scancode key) const {
    bool current = isKeyDown(key);
    auto it = previousKeys.find(key);
    bool previous = (it != previousKeys.end()) ? it->second : false;
    return !current && previous;
}

bool InputHandler::isMouseButtonDown(int button) const {
    if (button == SDL_BUTTON_LEFT) return mouseLeftCurrent;
    if (button == SDL_BUTTON_RIGHT) return mouseRightCurrent;
    return false;
}

bool InputHandler::wasMouseButtonPressed(int button) const {
    if (button == SDL_BUTTON_LEFT) return mouseLeftCurrent && !mouseLeftPrevious;
    if (button == SDL_BUTTON_RIGHT) return mouseRightCurrent && !mouseRightPrevious;
    return false;
}

bool InputHandler::wasMouseButtonReleased(int button) const {
    if (button == SDL_BUTTON_LEFT) return !mouseLeftCurrent && mouseLeftPrevious;
    if (button == SDL_BUTTON_RIGHT) return !mouseRightCurrent && mouseRightPrevious;
    return false;
}

void InputHandler::getMousePosition(int& x, int& y) const {
    x = mouseX;
    y = mouseY;
}

void InputHandler::updateKeyState(SDL_Scancode key, bool pressed) {
    currentKeys[key] = pressed;
}

void InputHandler::updateMouseButton(int button, bool pressed) {
    if (button == SDL_BUTTON_LEFT) mouseLeftCurrent = pressed;
    if (button == SDL_BUTTON_RIGHT) mouseRightCurrent = pressed;
}

#pragma once

#include "Types.h"
#ifdef __EMSCRIPTEN__
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <unordered_map>
#include <string>

enum class InputAction {
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    RUN,
    PAUSE,
    MENU_UP,
    MENU_DOWN,
    MENU_SELECT,
    MENU_BACK
};

struct InputBinding {
    SDL_Scancode keyboard = SDL_SCANCODE_UNKNOWN;
    SDL_GameControllerButton controller = SDL_CONTROLLER_BUTTON_INVALID;
    SDL_GameControllerAxis axis = SDL_CONTROLLER_AXIS_INVALID;
    float axisThreshold = 0.5f;
    bool axisPositive = true;
};

class InputManager {
public:
    InputManager();
    ~InputManager();
    
    void handleEvent(const SDL_Event& event);
    void update();
    
    bool isActionPressed(InputAction action) const;
    bool isActionJustPressed(InputAction action) const;
    bool isActionJustReleased(InputAction action) const;
    
    float getAxisValue(InputAction action) const;
    
    void setBinding(InputAction action, const InputBinding& binding);
    InputBinding getBinding(InputAction action) const;
    
    void loadBindings();
    void saveBindings();
    
    bool hasGamepad() const { return m_controller != nullptr; }

private:
    void openController();
    void closeController();
    
    std::unordered_map<InputAction, InputBinding> m_bindings;
    std::unordered_map<InputAction, bool> m_currentState;
    std::unordered_map<InputAction, bool> m_previousState;
    
    SDL_GameController* m_controller;
    int m_controllerIndex;
    
    void initializeDefaultBindings();
};
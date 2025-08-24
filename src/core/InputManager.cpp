#include "InputManager.h"
#include <iostream>

InputManager::InputManager()
    : m_controller(nullptr)
    , m_controllerIndex(-1)
{
    initializeDefaultBindings();
    openController();
}

InputManager::~InputManager() {
    closeController();
}

void InputManager::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED:
            if (!m_controller) {
                openController();
            }
            break;
            
        case SDL_CONTROLLERDEVICEREMOVED:
            if (m_controller && event.cdevice.which == m_controllerIndex) {
                closeController();
                openController();
            }
            break;
    }
}

void InputManager::update() {
    m_previousState = m_currentState;
    
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    for (auto& pair : m_bindings) {
        InputAction action = pair.first;
        const InputBinding& binding = pair.second;
        
        bool pressed = false;
        
        if (binding.keyboard != SDL_SCANCODE_UNKNOWN) {
            pressed |= keyState[binding.keyboard];
        }
        
        if (m_controller) {
            if (binding.controller != SDL_CONTROLLER_BUTTON_INVALID) {
                pressed |= SDL_GameControllerGetButton(m_controller, binding.controller);
            }
            
            if (binding.axis != SDL_CONTROLLER_AXIS_INVALID) {
                float axisValue = SDL_GameControllerGetAxis(m_controller, binding.axis) / 32767.0f;
                if (binding.axisPositive) {
                    pressed |= (axisValue > binding.axisThreshold);
                } else {
                    pressed |= (axisValue < -binding.axisThreshold);
                }
            }
        }
        
        m_currentState[action] = pressed;
    }
}

bool InputManager::isActionPressed(InputAction action) const {
    auto it = m_currentState.find(action);
    return (it != m_currentState.end()) ? it->second : false;
}

bool InputManager::isActionJustPressed(InputAction action) const {
    auto currentIt = m_currentState.find(action);
    auto previousIt = m_previousState.find(action);
    
    bool current = (currentIt != m_currentState.end()) ? currentIt->second : false;
    bool previous = (previousIt != m_previousState.end()) ? previousIt->second : false;
    
    return current && !previous;
}

bool InputManager::isActionJustReleased(InputAction action) const {
    auto currentIt = m_currentState.find(action);
    auto previousIt = m_previousState.find(action);
    
    bool current = (currentIt != m_currentState.end()) ? currentIt->second : false;
    bool previous = (previousIt != m_previousState.end()) ? previousIt->second : false;
    
    return !current && previous;
}

float InputManager::getAxisValue(InputAction action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end() || !m_controller) {
        return 0.0f;
    }
    
    const InputBinding& binding = it->second;
    if (binding.axis == SDL_CONTROLLER_AXIS_INVALID) {
        return isActionPressed(action) ? 1.0f : 0.0f;
    }
    
    float axisValue = SDL_GameControllerGetAxis(m_controller, binding.axis) / 32767.0f;
    return binding.axisPositive ? std::max(0.0f, axisValue) : std::min(0.0f, -axisValue);
}

void InputManager::setBinding(InputAction action, const InputBinding& binding) {
    m_bindings[action] = binding;
}

InputBinding InputManager::getBinding(InputAction action) const {
    auto it = m_bindings.find(action);
    return (it != m_bindings.end()) ? it->second : InputBinding{};
}

void InputManager::loadBindings() {
}

void InputManager::saveBindings() {
}

void InputManager::openController() {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            m_controller = SDL_GameControllerOpen(i);
            if (m_controller) {
                m_controllerIndex = i;
                std::cout << "Opened controller: " << SDL_GameControllerName(m_controller) << std::endl;
                break;
            }
        }
    }
}

void InputManager::closeController() {
    if (m_controller) {
        SDL_GameControllerClose(m_controller);
        m_controller = nullptr;
        m_controllerIndex = -1;
    }
}

void InputManager::initializeDefaultBindings() {
    m_bindings[InputAction::MOVE_LEFT] = {SDL_SCANCODE_A, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_AXIS_LEFTX, 0.3f, false};
    m_bindings[InputAction::MOVE_RIGHT] = {SDL_SCANCODE_D, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_AXIS_LEFTX, 0.3f, true};
    m_bindings[InputAction::JUMP] = {SDL_SCANCODE_SPACE, SDL_CONTROLLER_BUTTON_A};
    m_bindings[InputAction::RUN] = {SDL_SCANCODE_LSHIFT, SDL_CONTROLLER_BUTTON_X};
    m_bindings[InputAction::PAUSE] = {SDL_SCANCODE_ESCAPE, SDL_CONTROLLER_BUTTON_START};
    m_bindings[InputAction::MENU_UP] = {SDL_SCANCODE_W, SDL_CONTROLLER_BUTTON_DPAD_UP};
    m_bindings[InputAction::MENU_DOWN] = {SDL_SCANCODE_S, SDL_CONTROLLER_BUTTON_DPAD_DOWN};
    m_bindings[InputAction::MENU_SELECT] = {SDL_SCANCODE_RETURN, SDL_CONTROLLER_BUTTON_A};
    m_bindings[InputAction::MENU_BACK] = {SDL_SCANCODE_ESCAPE, SDL_CONTROLLER_BUTTON_B};
}
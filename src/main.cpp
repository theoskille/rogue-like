#include "engine/core/Application.h"
#include "engine/core/EventSystem.h"
#include "engine/input/InputHandler.h"
#include "game/states/EntityTestState.h"
#include "game/states/DataTestState.h"
#include "game/states/StatsTestState.h"
#include "game/states/PositionTestState.h"
#include "game/states/UITestState.h"
#include <iostream>

// Function to handle key press events
void OnKeyPressed(const Engine::Event& event) {
    int keyCode = event.GetIntData("key_code");
    int actionId = event.GetIntData("action");
    
    // Convert action ID to enum for readability
    Engine::InputAction action = static_cast<Engine::InputAction>(actionId);
    
    // Print action information
    std::string actionName;
    switch (action) {
        case Engine::InputAction::MOVE_UP:    actionName = "MOVE_UP"; break;
        case Engine::InputAction::MOVE_DOWN:  actionName = "MOVE_DOWN"; break;
        case Engine::InputAction::MOVE_LEFT:  actionName = "MOVE_LEFT"; break;
        case Engine::InputAction::MOVE_RIGHT: actionName = "MOVE_RIGHT"; break;
        case Engine::InputAction::CONFIRM:    actionName = "CONFIRM"; break;
        case Engine::InputAction::CANCEL:     actionName = "CANCEL"; break;
        case Engine::InputAction::MENU:       actionName = "MENU"; break;
        case Engine::InputAction::ATTACK:     actionName = "ATTACK"; break;
        case Engine::InputAction::USE_ITEM:   actionName = "USE_ITEM"; break;
        default: actionName = "UNKNOWN"; break;
    }
    
    std::cout << "Key pressed: " << keyCode << " (Action: " << actionName << ")" << std::endl;
}

int main() {
    // Create and initialize the application
    Engine::Application app(800, 600, "Rogue-Like Game - UI Test");
    
    // Subscribe to events
    Engine::EventSystem::GetInstance().Subscribe(Engine::EventType::KEY_PRESSED, OnKeyPressed);
    
    if (!app.Initialize()) {
        return 1;
    }
    
    // Clear any existing states and start with the UI Test state
    while (!Engine::StateManager::GetInstance().IsEmpty()) {
        Engine::StateManager::GetInstance().PopState();
    }
    
    // Start with the UI Test state
    Engine::StateManager::GetInstance().PushState(std::make_unique<Game::UITestState>());
    
    // Run the main game loop
    app.Run();
    
    // Clean up
    app.Shutdown();
    
    return 0;
} 
#include "engine/core/Application.h"
#include "engine/core/StateManager.h"
#include "game/states/EntityTestState.h"
#include "game/states/DataTestState.h"
#include "game/states/StatsTestState.h"
#include "game/states/PositionTestState.h"
#include "game/states/UITestState.h"
#include "game/states/BattlefieldTestState.h"
#include "game/states/ActionTestState.h"
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
    Engine::Application app(1024, 768, "Rogue-Like Game - Action Test");
    
    if (!app.Initialize()) {
        std::cerr << "Failed to initialize application!" << std::endl;
        return 1;
    }
    
    // Clear any existing states (Application adds a DataTestState by default)
    Engine::StateManager& stateManager = Engine::StateManager::GetInstance();
    while (!stateManager.IsEmpty()) {
        stateManager.PopState();
    }
    
    // Push the action test state
    stateManager.PushState(std::make_unique<Game::ActionTestState>());
    
    // Run the application
    app.Run();
    
    // Clean up
    app.Shutdown();
    
    return 0;
} 
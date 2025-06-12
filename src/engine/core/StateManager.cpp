#include "StateManager.h"
#include <iostream>

namespace Engine {

// Initialize static member
StateManager* StateManager::instance = nullptr;

StateManager::StateManager() {
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple StateManager instances created!" << std::endl;
    }
}

StateManager::~StateManager() {
    // Clean up the singleton instance
    if (instance == this) {
        instance = nullptr;
    }
    
    // Clear all states
    while (!states.empty()) {
        states.pop();
    }
}

void StateManager::PushState(std::unique_ptr<GameState> state) {
    // Pause the current state if it exists
    if (!states.empty()) {
        states.top()->Pause();
    }
    
    // Log state transition
    std::cout << "Pushing state: " << state->GetStateName() << std::endl;
    
    // Push and enter the new state
    states.push(std::move(state));
    states.top()->Enter();
}

void StateManager::PopState() {
    // Exit the current state
    if (!states.empty()) {
        std::cout << "Popping state: " << states.top()->GetStateName() << std::endl;
        states.top()->Exit();
        states.pop();
    }
    
    // Resume the previous state if it exists
    if (!states.empty()) {
        std::cout << "Resuming state: " << states.top()->GetStateName() << std::endl;
        states.top()->Resume();
    }
}

void StateManager::ChangeState(std::unique_ptr<GameState> state) {
    // Exit the current state if it exists
    if (!states.empty()) {
        std::cout << "Changing from state: " << states.top()->GetStateName() << std::endl;
        states.top()->Exit();
        states.pop();
    }
    
    // Push and enter the new state
    std::cout << "Changing to state: " << state->GetStateName() << std::endl;
    states.push(std::move(state));
    states.top()->Enter();
}

GameState* StateManager::GetCurrentState() {
    if (states.empty()) {
        return nullptr;
    }
    
    return states.top().get();
}

bool StateManager::IsEmpty() const {
    return states.empty();
}

void StateManager::Update(float deltaTime) {
    // Update the current state if it exists
    if (!states.empty()) {
        states.top()->Update(deltaTime);
    }
}

void StateManager::Render() {
    // Render the current state if it exists
    if (!states.empty()) {
        states.top()->Render();
    }
}

StateManager& StateManager::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: StateManager instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static StateManager defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 
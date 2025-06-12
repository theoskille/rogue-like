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
    if (!states.empty()) {
        // Pause the current state
        states.top()->Pause();
    }
    
    // Add the new state and enter it
    std::cout << "Pushing state: " << state->GetStateName() << std::endl;
    state->Enter();
    states.push(std::move(state));
}

void StateManager::PopState() {
    if (states.empty()) {
        std::cerr << "Warning: Trying to pop from an empty state stack!" << std::endl;
        return;
    }
    
    // Exit and remove the current state
    std::cout << "Popping state: " << states.top()->GetStateName() << std::endl;
    states.top()->Exit();
    states.pop();
    
    // Resume the previous state if there is one
    if (!states.empty()) {
        std::cout << "Resuming state: " << states.top()->GetStateName() << std::endl;
        states.top()->Resume();
    }
}

void StateManager::ChangeState(std::unique_ptr<GameState> state) {
    if (!states.empty()) {
        // Exit and remove the current state
        std::cout << "Changing from state: " << states.top()->GetStateName() << std::endl;
        states.top()->Exit();
        states.pop();
    }
    
    // Add the new state and enter it
    std::cout << "Changing to state: " << state->GetStateName() << std::endl;
    state->Enter();
    states.push(std::move(state));
}

void StateManager::Update(float deltaTime) {
    if (states.empty()) {
        return;
    }
    
    // Update the current state
    states.top()->Update(deltaTime);
}

void StateManager::Render() {
    if (states.empty()) {
        return;
    }
    
    // Render the current state
    states.top()->Render();
}

bool StateManager::IsEmpty() const {
    return states.empty();
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
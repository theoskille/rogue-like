#pragma once

#include <memory>
#include <stack>
#include <string>
#include <iostream>

namespace Engine {

// Forward declarations
class StateManager;

// Base class for all game states
class GameState {
public:
    virtual ~GameState() = default;
    
    // State lifecycle methods
    virtual void Enter() = 0;
    virtual void Exit() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Pause() {}
    virtual void Resume() {}
    
    // Get the state name for debugging
    virtual std::string GetStateName() const = 0;
};

// Manages the stack of game states
class StateManager {
public:
    StateManager();
    ~StateManager();
    
    // State stack operations
    void PushState(std::unique_ptr<GameState> state);
    void PopState();
    void ChangeState(std::unique_ptr<GameState> state);
    
    // State access
    GameState* GetCurrentState();
    bool IsEmpty() const;
    
    // Update and render the current state
    void Update(float deltaTime);
    void Render();
    
    // Singleton access
    static StateManager& GetInstance();
    
private:
    std::stack<std::unique_ptr<GameState>> states;
    
    // Singleton instance
    static StateManager* instance;
};

} // namespace Engine 
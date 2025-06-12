#pragma once

#include <memory>
#include <stack>
#include <string>

namespace Engine {

// Forward declaration
class GameState;

// State Manager class
class StateManager {
public:
    StateManager();
    ~StateManager();

    // State stack operations
    void PushState(std::unique_ptr<GameState> state);
    void PopState();
    void ChangeState(std::unique_ptr<GameState> state);
    
    // Update and render the current state
    void Update(float deltaTime);
    void Render();
    
    // State queries
    bool IsEmpty() const;
    
    // Singleton access
    static StateManager& GetInstance();

private:
    // Stack of game states
    std::stack<std::unique_ptr<GameState>> states;
    
    // Singleton instance
    static StateManager* instance;
};

// Game State base class
class GameState {
public:
    virtual ~GameState() = default;
    
    // State lifecycle methods
    virtual void Enter() = 0;
    virtual void Exit() = 0;
    
    // Game loop methods
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    
    // Optional methods for states that can be paused/resumed
    virtual void Pause() {}
    virtual void Resume() {}
    
    // State identification
    virtual std::string GetStateName() const = 0;
};

} // namespace Engine 
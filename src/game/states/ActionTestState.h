#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../combat/Battlefield.h"
#include "../combat/Action.h"
#include "../entities/Entity.h"
#include <memory>
#include <vector>

namespace Game {

// Test state for action implementation
class ActionTestState : public Engine::GameState {
public:
    ActionTestState();
    ~ActionTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "ActionTestState"; }
    
private:
    // Battlefield instance
    Battlefield battlefield;
    
    // Player and enemy entities
    std::shared_ptr<Entity> player;
    std::shared_ptr<Entity> enemy;
    
    // Available actions
    std::vector<std::shared_ptr<Action>> playerActions;
    std::vector<std::shared_ptr<Action>> enemyActions;
    
    // Selected action index
    int selectedActionIndex;
    
    // UI state
    enum class UIState {
        SELECT_ACTION,  // Select an action to perform
        WAITING,        // Waiting for action to complete
        GAME_OVER       // Game is over
    };
    
    UIState currentState;
    
    // Game state
    bool playerTurn;
    bool gameOver;
    std::string gameOverMessage;
    
    // Initialize entities and battlefield
    void CreateEntities();
    
    // Load actions from JSON
    void LoadActions();
    
    // Handle player action selection
    void SelectAction(int index);
    
    // Execute selected action
    void ExecuteSelectedAction();
    
    // Execute enemy AI turn
    void ExecuteEnemyTurn();
    
    // Draw the battlefield
    void DrawBattlefield();
    
    // Draw entity stats
    void DrawEntityInfo(const Entity* entity, int x, int y, int width, int height);
    
    // Draw action list
    void DrawActionList();
    
    // Draw game state
    void DrawGameState();
};

} // namespace Game 
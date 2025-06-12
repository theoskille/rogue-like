#pragma once

#include "../../engine/core/StateManager.h"
#include "../combat/TurnManager.h"
#include "../combat/Battlefield.h"
#include <memory>
#include <vector>

namespace Game {

class TurnManagerTestState : public Engine::GameState {
public:
    TurnManagerTestState();
    ~TurnManagerTestState();
    
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Return the state name for debugging
    std::string GetStateName() const override { return "TurnManagerTestState"; }
    
private:
    // Create test entities
    void CreateEntities();
    
    // UI helper methods
    void DrawEntityInfo(const Entity* entity, int x, int y, int width, int height);
    void DrawTurnOrder();
    void DrawControls();
    
    // Combat components
    TurnManager turnManager;
    Battlefield battlefield;
    
    // Test entities
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> player;
    std::shared_ptr<Entity> fastEnemy;
    std::shared_ptr<Entity> strongEnemy;
    std::shared_ptr<Entity> tankEnemy;
    
    // Test state
    enum class TestState {
        WAITING_FOR_INPUT,
        EXECUTING_TURN,
        COMBAT_OVER
    };
    
    TestState currentState;
    std::string statusMessage;
    bool gameOver;
    std::string gameOverMessage;
};

} // namespace Game 
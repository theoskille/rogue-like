#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../combat/Battlefield.h"
#include "../entities/Entity.h"
#include <memory>
#include <vector>

namespace Game {

// Test state for battlefield implementation
class BattlefieldTestState : public Engine::GameState {
public:
    BattlefieldTestState();
    ~BattlefieldTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "BattlefieldTestState"; }
    
private:
    // Battlefield instance
    Battlefield battlefield;
    
    // Test entities
    std::vector<std::shared_ptr<Entity>> playerEntities;
    std::vector<std::shared_ptr<Entity>> enemyEntities;
    
    // Selected entity index
    int selectedEntityIndex;
    
    // Target position for movement
    int targetPosition;
    
    // Create test entities
    void CreateTestEntities();
    
    // Handle entity movement
    void MoveSelectedEntity(int newPosition);
    
    // Draw the battlefield
    void DrawBattlefield();
    
    // Draw entity stats
    void DrawEntityInfo();
    
    // Draw controls help
    void DrawControls();
};

} // namespace Game 
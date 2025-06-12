#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../entities/Entity.h"
#include <memory>
#include <vector>

namespace Game {

class PositionTestState : public Engine::GameState {
public:
    PositionTestState();
    ~PositionTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "PositionTestState"; }
    
private:
    // Test entities with position components
    std::unique_ptr<Entity> playerEntity;
    std::unique_ptr<Entity> enemyEntity;
    
    // Battlefield size
    int battlefieldSize = 8;
    
    // Tile width for rendering
    int tileWidth = 60;
    int tileHeight = 60;
    
    // Current action mode
    enum class ActionMode {
        MOVE,       // Move the player
        RANGE_TEST  // Test range between entities
    };
    
    ActionMode currentMode = ActionMode::MOVE;
    int rangeToTest = 1;  // Range to test in range test mode
    
    // Draw the battlefield
    void RenderBattlefield(int x, int y);
    
    // Draw entity information
    void RenderEntityInfo(int x, int y);
    
    // Draw controls help
    void RenderControls(int y);
    
    bool isPaused = false;
};

} // namespace Game 
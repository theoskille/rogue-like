#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../entities/Entity.h"
#include <memory>

namespace Game {

class StatsTestState : public Engine::GameState {
public:
    StatsTestState();
    ~StatsTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "StatsTestState"; }
    
private:
    // Test entity with stats
    std::unique_ptr<Entity> testEntity;
    
    // Currently selected stat to modify
    int selectedStat = 0;
    
    // Whether we're in damage test mode
    bool damageTestMode = false;
    
    // Damage amount for testing
    int testDamage = 5;
    
    // Draw stats UI
    void RenderStats(int x, int y);
    
    // Draw controls help
    void RenderControls(int y);
    
    bool isPaused = false;
};

} // namespace Game 
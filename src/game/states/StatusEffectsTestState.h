#pragma once

#include "../../engine/core/StateManager.h"
#include "../entities/Entity.h"
#include "../entities/components/StatusEffectsComponent.h"
#include <memory>
#include <vector>

namespace Game {

// Test state for the StatusEffectsComponent
class StatusEffectsTestState : public Engine::GameState {
public:
    StatusEffectsTestState();
    ~StatusEffectsTestState() override = default;
    
    // GameState lifecycle methods
    void Enter() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
    void Pause() override;
    void Resume() override;
    
    // Get state name for debugging
    std::string GetStateName() const override { return "StatusEffectsTestState"; }
    
private:
    // Test entity with status effects
    std::shared_ptr<Entity> testEntity;
    
    // Available status effects to apply
    std::vector<std::pair<std::string, StatusEffectType>> availableEffects;
    
    // Currently selected effect
    int selectedEffectIndex;
    
    // Effect duration and magnitude
    int effectDuration;
    int effectMagnitude;
    
    // UI state (selected field)
    enum class UIField {
        EFFECT_TYPE,
        DURATION,
        MAGNITUDE,
        APPLY
    };
    UIField selectedField;
    
    // State flags
    bool isPaused;
    
    // Helper methods
    void ApplySelectedEffect();
    void DrawEffectInfo();
    void DrawActiveEffects();
};

} // namespace Game 
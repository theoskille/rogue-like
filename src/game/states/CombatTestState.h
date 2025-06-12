#pragma once

#include "../../engine/core/StateManager.h"
#include "../combat/Battlefield.h"
#include "../combat/TurnManager.h"
#include "../combat/CombatSystem.h"
#include "../entities/Entity.h"
#include "../entities/components/StatsComponent.h"
#include "../entities/components/PositionComponent.h"
#include "../entities/components/StatusEffectsComponent.h"
#include "../../data/ActionDataLoader.h"
#include <memory>
#include <vector>
#include <string>

namespace Game {

// Test state for the complete combat system
class CombatTestState : public Engine::GameState {
public:
    CombatTestState();
    virtual ~CombatTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
    void Pause() override;
    void Resume() override;
    
    // Get state name for debugging
    std::string GetStateName() const override { return "CombatTestState"; }
    
private:
    // Combat components
    CombatSystem combatSystem;
    
    // Entities
    std::shared_ptr<Entity> player;
    std::vector<std::shared_ptr<Entity>> playerTeam;
    std::vector<std::shared_ptr<Entity>> enemyTeam;
    
    // Action data
    Game::ActionDataLoader& actionLoader = Game::ActionDataLoader::GetInstance();
    std::vector<std::shared_ptr<Action>> playerActions;
    
    // UI state
    enum class CombatUIState {
        SELECT_ACTION,
        SELECT_TARGET,
        EXECUTING_ACTION,
        ENEMY_TURN,
        COMBAT_OVER
    };
    CombatUIState uiState;
    
    // Selection indices
    int selectedActionIndex;
    int selectedTargetIndex;
    
    // Status message for the UI
    std::string statusMessage;
    
    // Combat result
    CombatResult combatResult;
    
    // State flags
    bool isPaused;
    
    // Helper methods
    void CreatePlayer();
    void CreateEnemies();
    void LoadActions();
    void StartCombat();
    void HandleActionSelection();
    void HandleTargetSelection();
    void ExecuteSelectedAction();
    void ProcessEnemyTurn();
    void CheckCombatResult();
    void ResetCombat();
    bool IsSelfTargetedAction(const Action* action) const;
    
    // Rendering helpers
    void RenderBattlefield();
    void RenderActionMenu();
    void RenderTargetSelection();
    void RenderEntityStats(const Entity* entity, int x, int y, bool detailed = false);
    void RenderStatusEffects(const Entity* entity, int x, int y);
    void RenderCombatResult();
};

} // namespace Game 
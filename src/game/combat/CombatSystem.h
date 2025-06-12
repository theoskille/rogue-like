#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Battlefield.h"
#include "TurnManager.h"
#include "Action.h"
#include "../entities/Entity.h"

// Forward declarations for Engine namespace
namespace Engine {
    class EventSystem;
}

namespace Game {

// Enum for combat result
enum class CombatResult {
    NONE,           // Combat still in progress
    PLAYER_VICTORY, // Player defeated all enemies
    PLAYER_DEFEAT,  // Player was defeated
    ESCAPE          // Player successfully escaped
};

// Enum for combat state
enum class CombatState {
    NOT_STARTED,    // Combat hasn't started yet
    SELECTING_ACTION, // Player is selecting action
    EXECUTING_ACTION, // Action is being executed
    ENEMY_TURN,     // Enemy is taking its turn
    ENDED           // Combat has ended
};

// Class to manage the entire combat flow
class CombatSystem {
public:
    // Constructor
    CombatSystem();
    
    // Set event system for publishing combat events
    void SetEventSystem(Engine::EventSystem* eventSystem);
    
    // Start a combat encounter
    void StartCombat(const std::vector<std::shared_ptr<Entity>>& playerTeam, 
                     const std::vector<std::shared_ptr<Entity>>& enemyTeam);
    
    // Process a turn with the given action
    bool ProcessTurn(std::shared_ptr<Action> action, std::shared_ptr<Entity> target);
    
    // Check if combat is over and determine the result
    CombatResult CheckCombatResult() const;
    
    // Reset the combat system for a new encounter
    void Reset();
    
    // Get the current state of combat
    CombatState GetState() const;
    
    // Get battlefield reference
    Battlefield& GetBattlefield();
    
    // Get turn manager reference
    TurnManager& GetTurnManager();
    
    // Get all available actions for the current entity
    std::vector<std::shared_ptr<Action>> GetAvailableActions() const;
    
    // Get valid targets for the given action
    std::vector<std::shared_ptr<Entity>> GetValidTargets(std::shared_ptr<Action> action) const;
    
    // Try to escape from combat
    bool TryEscape();
    
    // Get the current active entity
    std::shared_ptr<Entity> GetCurrentEntity() const;
    
    // Process enemy AI turn (for automated enemy decisions)
    bool ProcessEnemyTurn();

private:
    // Core components
    Battlefield battlefield;
    TurnManager turnManager;
    
    // Reference to event system for publishing events
    Engine::EventSystem* eventSystem;
    
    // Teams
    std::vector<std::shared_ptr<Entity>> playerTeam;
    std::vector<std::shared_ptr<Entity>> enemyTeam;
    
    // Combat state
    CombatState state;
    
    // Select an action for an enemy (AI)
    std::pair<std::shared_ptr<Action>, std::shared_ptr<Entity>> SelectEnemyAction(
        std::shared_ptr<Entity> enemy);
    
    // Helper method to determine if an entity is on player team
    bool IsPlayerEntity(const std::shared_ptr<Entity>& entity) const;
    
    // Check if two entities are on the same team
    bool AreAllies(const std::shared_ptr<Entity>& entity1, 
                  const std::shared_ptr<Entity>& entity2) const;
    
    // Helper method to determine if an action targets the user
    bool IsSelfTargetedAction(const Action* action) const;
};

} // namespace Game 
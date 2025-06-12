#include "CombatSystem.h"
#include "../../engine/core/EventSystem.h"
#include "../entities/components/StatsComponent.h"
#include "../entities/components/PositionComponent.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

namespace Game {

CombatSystem::CombatSystem()
    : eventSystem(nullptr),
      state(CombatState::NOT_STARTED) {
    // Seed random number generator for escape chances and other random elements
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void CombatSystem::SetEventSystem(Engine::EventSystem* eventSystem) {
    this->eventSystem = eventSystem;
}

void CombatSystem::StartCombat(const std::vector<std::shared_ptr<Entity>>& playerTeam,
                               const std::vector<std::shared_ptr<Entity>>& enemyTeam) {
    // Reset previous combat state
    Reset();
    
    // Store teams
    this->playerTeam = playerTeam;
    this->enemyTeam = enemyTeam;
    
    // Set up battlefield positions for both teams
    for (size_t i = 0; i < playerTeam.size() && i < 4; ++i) {
        auto& entity = playerTeam[i];
        if (entity && entity->HasComponent<PositionComponent>()) {
            battlefield.PlaceEntity(entity, i);
        }
    }
    
    for (size_t i = 0; i < enemyTeam.size() && i < 4; ++i) {
        auto& entity = enemyTeam[i];
        if (entity && entity->HasComponent<PositionComponent>()) {
            battlefield.PlaceEntity(entity, i + 4);
        }
    }
    
    // Initialize turn order with all entities
    std::vector<std::shared_ptr<Entity>> allEntities;
    allEntities.insert(allEntities.end(), playerTeam.begin(), playerTeam.end());
    allEntities.insert(allEntities.end(), enemyTeam.begin(), enemyTeam.end());
    turnManager.Initialize(allEntities);
    
    // Set combat state to started
    state = CombatState::SELECTING_ACTION;
    
    // Publish combat start event if event system is available
    if (eventSystem) {
        Engine::Event event(Engine::EventType::COMBAT_START);
        eventSystem->Publish(event);
    }
    
    std::cout << "Combat started with " << playerTeam.size() << " player entities and "
              << enemyTeam.size() << " enemy entities." << std::endl;
}

bool CombatSystem::ProcessTurn(std::shared_ptr<Action> action, std::shared_ptr<Entity> target) {
    auto currentEntity = turnManager.GetCurrentEntity();
    
    // Validate that an entity is active and has an action
    if (!currentEntity || !action) {
        std::cout << "No active entity or action to process" << std::endl;
        return false;
    }
    
    // Check if it's actually this entity's turn
    if (GetCurrentEntity() != currentEntity) {
        std::cout << "Not " << currentEntity->GetName() << "'s turn yet" << std::endl;
        return false;
    }
    
    // Execute the action
    state = CombatState::EXECUTING_ACTION;
    bool success = action->Execute(currentEntity.get(), target.get(), &battlefield);
    
    if (success) {
        // Publish action event if event system is available
        if (eventSystem) {
            // Note: In a full implementation, you'd add ACTION_EXECUTED to the Engine::EventType enum
            // For now, use an existing event type as a workaround
            Engine::Event event(Engine::EventType::PLAYER_MOVE);
            event.SetData("action_name", action->GetName());
            event.SetData("user_name", currentEntity->GetName());
            event.SetData("target_name", target->GetName());
            eventSystem->Publish(event);
        }
        
        // End the turn
        turnManager.EndTurn();
        
        // Check if combat is over
        if (CheckCombatResult() != CombatResult::NONE) {
            state = CombatState::ENDED;
        } else {
            // Determine next state based on whose turn it is
            auto nextEntity = turnManager.GetCurrentEntity();
            if (nextEntity && IsPlayerEntity(nextEntity)) {
                state = CombatState::SELECTING_ACTION;
            } else {
                state = CombatState::ENEMY_TURN;
            }
        }
    } else {
        // Action failed, but turn continues (player can try another action)
        state = CombatState::SELECTING_ACTION;
    }
    
    return success;
}

CombatResult CombatSystem::CheckCombatResult() const {
    // Check if all player entities are defeated
    bool allPlayerDefeated = true;
    for (const auto& entity : playerTeam) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            allPlayerDefeated = false;
            break;
        }
    }
    
    // Check if all enemy entities are defeated
    bool allEnemiesDefeated = true;
    for (const auto& entity : enemyTeam) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            allEnemiesDefeated = false;
            break;
        }
    }
    
    // Determine combat result
    if (allPlayerDefeated) {
        return CombatResult::PLAYER_DEFEAT;
    } else if (allEnemiesDefeated) {
        return CombatResult::PLAYER_VICTORY;
    }
    
    return CombatResult::NONE;
}

void CombatSystem::Reset() {
    // Clear the battlefield
    battlefield.Clear();
    
    // Reset turn manager
    turnManager.Reset();
    
    // Clear teams
    playerTeam.clear();
    enemyTeam.clear();
    
    // Reset state
    state = CombatState::NOT_STARTED;
}

CombatState CombatSystem::GetState() const {
    return state;
}

Battlefield& CombatSystem::GetBattlefield() {
    return battlefield;
}

TurnManager& CombatSystem::GetTurnManager() {
    return turnManager;
}

std::vector<std::shared_ptr<Action>> CombatSystem::GetAvailableActions() const {
    std::vector<std::shared_ptr<Action>> availableActions;
    
    auto currentEntity = turnManager.GetCurrentEntity();
    if (!currentEntity) {
        return availableActions;
    }
    
    // In a real implementation, this would retrieve actions from the entity's
    // action list, equipment, or class definition. For now, return an empty list.
    // This would be replaced with actual action retrieval logic.
    
    // Example placeholder code:
    // if (currentEntity->HasComponent<ActionComponent>()) {
    //    availableActions = currentEntity->GetComponent<ActionComponent>().GetActions();
    // }
    
    return availableActions;
}

std::vector<std::shared_ptr<Entity>> CombatSystem::GetValidTargets(std::shared_ptr<Action> action) const {
    std::vector<std::shared_ptr<Entity>> validTargets;
    
    if (!action) {
        return validTargets;
    }
    
    auto currentEntity = turnManager.GetCurrentEntity();
    if (!currentEntity) {
        return validTargets;
    }
    
    // Determine target list based on action type
    bool isOffensive = (action->GetType() == ActionType::ATTACK || 
                        action->GetType() == ActionType::DEBUFF);
    
    // For self-targeting actions, just return the current entity
    if (IsSelfTargetedAction(action.get())) {
        validTargets.push_back(currentEntity);
        return validTargets;
    }
    
    // Check all entities on the battlefield
    const std::vector<std::shared_ptr<Entity>>& potentialTargets = 
        isOffensive ? enemyTeam : playerTeam;
    
    for (const auto& target : potentialTargets) {
        // Skip defeated entities
        if (!target || (target->HasComponent<StatsComponent>() && 
                      target->GetComponent<StatsComponent>().IsDead())) {
            continue;
        }
        
        // Add entity to valid targets if the action can be used on it
        if (action->CanUse(currentEntity.get(), target.get(), &battlefield)) {
            validTargets.push_back(target);
        }
    }
    
    return validTargets;
}

bool CombatSystem::TryEscape() {
    // Basic chance-based escape system
    // Higher chance when player has higher average speed than enemies
    
    // Calculate average player speed
    int playerSpeedTotal = 0;
    int playerCount = 0;
    
    for (const auto& entity : playerTeam) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            playerSpeedTotal += entity->GetComponent<StatsComponent>().GetCurrentStat(StatType::SPEED);
            playerCount++;
        }
    }
    
    // Calculate average enemy speed
    int enemySpeedTotal = 0;
    int enemyCount = 0;
    
    for (const auto& entity : enemyTeam) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            enemySpeedTotal += entity->GetComponent<StatsComponent>().GetCurrentStat(StatType::SPEED);
            enemyCount++;
        }
    }
    
    // Calculate escape chance
    int playerAvgSpeed = (playerCount > 0) ? (playerSpeedTotal / playerCount) : 0;
    int enemyAvgSpeed = (enemyCount > 0) ? (enemySpeedTotal / enemyCount) : 0;
    
    // Base 50% chance, modified by speed difference
    int escapeChance = 50 + 5 * (playerAvgSpeed - enemyAvgSpeed);
    
    // Clamp between 10% and 90%
    escapeChance = std::max(10, std::min(90, escapeChance));
    
    // Roll for escape
    int roll = std::rand() % 100 + 1; // 1-100
    bool escaped = roll <= escapeChance;
    
    std::cout << "Escape attempt! Chance: " << escapeChance << "%, Roll: " << roll 
              << ", " << (escaped ? "Success!" : "Failed!") << std::endl;
    
    if (escaped) {
        state = CombatState::ENDED;
        
        // Publish escape event if event system is available
        if (eventSystem) {
            // Note: In a full implementation, you'd add COMBAT_ESCAPE to the Engine::EventType enum
            // For now, use an existing event type as a workaround
            Engine::Event event(Engine::EventType::PLAYER_MOVE);
            event.SetData("action", "escape");
            eventSystem->Publish(event);
        }
    } else {
        // Failed escape costs a turn
        turnManager.EndTurn();
        
        // Check whose turn is next
        auto nextEntity = turnManager.GetCurrentEntity();
        if (nextEntity && IsPlayerEntity(nextEntity)) {
            state = CombatState::SELECTING_ACTION;
        } else {
            state = CombatState::ENEMY_TURN;
        }
    }
    
    return escaped;
}

std::shared_ptr<Entity> CombatSystem::GetCurrentEntity() const {
    return turnManager.GetCurrentEntity();
}

bool CombatSystem::ProcessEnemyTurn() {
    auto enemy = turnManager.GetCurrentEntity();
    
    // Validate that it's an enemy's turn
    if (!enemy || IsPlayerEntity(enemy)) {
        return false;
    }
    
    // Select an action and target for the enemy
    auto [action, target] = SelectEnemyAction(enemy);
    
    // Process the turn with the selected action
    if (action && target) {
        std::cout << enemy->GetName() << " uses " << action->GetName() << " on " 
                  << target->GetName() << std::endl;
                  
        return ProcessTurn(action, target);
    }
    
    // If no valid action was found, just end the turn
    std::cout << enemy->GetName() << " has no valid actions." << std::endl;
    turnManager.EndTurn();
    
    // Set next state
    auto nextEntity = turnManager.GetCurrentEntity();
    if (nextEntity && IsPlayerEntity(nextEntity)) {
        state = CombatState::SELECTING_ACTION;
    } else {
        state = CombatState::ENEMY_TURN;
    }
    
    return true;
}

std::pair<std::shared_ptr<Action>, std::shared_ptr<Entity>> CombatSystem::SelectEnemyAction(
    std::shared_ptr<Entity> enemy) {
    // This is a placeholder for enemy AI decision-making
    // In a full implementation, this would consider various factors:
    // - Enemy type and predefined behavior patterns
    // - Available actions and their effectiveness
    // - Current health and status effects
    // - Positioning on the battlefield
    
    // For now, return a simple null action (enemy will just skip turn)
    return {nullptr, nullptr};
    
    // Example of a more sophisticated implementation (commented out):
    /*
    std::vector<std::shared_ptr<Action>> actions = GetAvailableActions();
    
    // First priority: healing if low health
    if (enemy->GetComponent<StatsComponent>().GetHealthPercentage() < 0.3) {
        // Look for healing actions
        for (auto& action : actions) {
            if (action->GetType() == ActionType::HEAL) {
                return {action, enemy}; // Self-heal
            }
        }
    }
    
    // Second priority: attack player with highest damage action
    std::shared_ptr<Action> bestAttack = nullptr;
    int highestDamage = 0;
    
    for (auto& action : actions) {
        if (action->GetType() == ActionType::ATTACK) {
            int damage = action->GetDamage();
            if (damage > highestDamage) {
                highestDamage = damage;
                bestAttack = action;
            }
        }
    }
    
    if (bestAttack) {
        // Find most vulnerable player (lowest health percentage)
        std::shared_ptr<Entity> bestTarget = nullptr;
        float lowestHealth = 1.0f;
        
        for (auto& target : playerTeam) {
            if (!target->GetComponent<StatsComponent>().IsDead()) {
                float healthPct = target->GetComponent<StatsComponent>().GetHealthPercentage();
                if (healthPct < lowestHealth) {
                    lowestHealth = healthPct;
                    bestTarget = target;
                }
            }
        }
        
        if (bestTarget) {
            return {bestAttack, bestTarget};
        }
    }
    
    return {nullptr, nullptr};
    */
}

bool CombatSystem::IsPlayerEntity(const std::shared_ptr<Entity>& entity) const {
    return std::find(playerTeam.begin(), playerTeam.end(), entity) != playerTeam.end();
}

bool CombatSystem::AreAllies(const std::shared_ptr<Entity>& entity1, 
                             const std::shared_ptr<Entity>& entity2) const {
    bool entity1IsPlayer = IsPlayerEntity(entity1);
    bool entity2IsPlayer = IsPlayerEntity(entity2);
    
    // Both are on player team or both are on enemy team
    return entity1IsPlayer == entity2IsPlayer;
}

// Helper method to determine if an action targets the user
bool CombatSystem::IsSelfTargetedAction(const Action* action) const {
    if (!action) return false;
    
    // These action types typically target self
    if (action->GetType() == ActionType::BUFF || 
        action->GetType() == ActionType::MOVEMENT) {
        return true;
    }
    
    // Check for specific self_only property
    if (action->GetProperty("self_only") > 0) {
        return true;
    }
    
    return false;
}

} // namespace Game 
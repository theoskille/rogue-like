#include "TurnManager.h"
#include "Battlefield.h"
#include "../entities/components/StatsComponent.h"
#include "../entities/components/PositionComponent.h"
#include <iostream>
#include <algorithm>

namespace Game {

TurnManager::TurnManager() 
    : currentEntity(nullptr),
      currentRound(0) {
}

void TurnManager::Initialize(const std::vector<std::shared_ptr<Entity>>& entities) {
    // Clear any existing turns
    while (!turnQueue.empty()) {
        turnQueue.pop();
    }
    
    // Clear tracking data
    entitiesInCurrentRound.clear();
    
    currentEntity = nullptr;
    currentRound = 1;
    
    std::cout << "------- Starting Round " << currentRound << " -------" << std::endl;
    
    // Add all active entities to the queue based on speed
    for (const auto& entity : entities) {
        if (entity && entity->HasComponent<StatsComponent>()) {
            // Skip defeated entities
            if (entity->GetComponent<StatsComponent>().IsDead()) {
                continue;
            }
            
            // Use speed directly as initiative (higher speed = higher priority)
            int speed = entity->GetComponent<StatsComponent>().GetCurrentStat(StatType::SPEED);
            turnQueue.push(Turn(entity, speed));
            
            // Add to list of entities in current round
            entitiesInCurrentRound.push_back(entity);
        }
    }
    
    // Set the current entity to the first in queue
    if (!turnQueue.empty()) {
        Turn nextTurn = turnQueue.top();
        turnQueue.pop();
        currentEntity = nextTurn.entity;
        
        std::cout << "Turn begins for " << currentEntity->GetName() 
                  << " (Speed: " << nextTurn.initiative << ")" << std::endl;
    }
}

std::shared_ptr<Entity> TurnManager::GetNextEntity() {
    // If there's no current entity, get the next one from the queue
    if (!currentEntity && !turnQueue.empty()) {
        Turn nextTurn = turnQueue.top();
        turnQueue.pop();
        currentEntity = nextTurn.entity;
        
        std::cout << "Turn begins for " << currentEntity->GetName() 
                  << " (Speed: " << nextTurn.initiative << ")" << std::endl;
        
        // Check if we've completed a round (all entities have acted)
        if (turnQueue.empty()) {
            PrepareNextRound();
        }
    }
    
    return currentEntity;
}

void TurnManager::EndTurn() {
    if (!currentEntity) {
        return;
    }
    
    if (currentEntity->HasComponent<StatsComponent>()) {
        auto& stats = currentEntity->GetComponent<StatsComponent>();
        
        // Only proceed if entity is still alive
        if (!stats.IsDead()) {
            std::cout << currentEntity->GetName() << "'s turn ends." << std::endl;
        } else {
            std::cout << currentEntity->GetName() << " is defeated and removed from turn order." << std::endl;
            
            // Remove from entities in current round
            auto it = std::find(entitiesInCurrentRound.begin(), entitiesInCurrentRound.end(), currentEntity);
            if (it != entitiesInCurrentRound.end()) {
                entitiesInCurrentRound.erase(it);
            }
        }
    }
    
    // Clear the current entity
    currentEntity = nullptr;
    
    // Get the next entity
    GetNextEntity();
}

bool TurnManager::IsCombatOver(Battlefield* battlefield) const {
    if (!battlefield) {
        return false;
    }
    
    // Get entities from both sides
    auto playerSideEntities = battlefield->GetPlayerSideEntities();
    auto enemySideEntities = battlefield->GetEnemySideEntities();
    
    // Check if all entities on one side are defeated
    bool playerSideDefeated = true;
    for (const auto& entity : playerSideEntities) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            playerSideDefeated = false;
            break;
        }
    }
    
    bool enemySideDefeated = true;
    for (const auto& entity : enemySideEntities) {
        if (entity && entity->HasComponent<StatsComponent>() && 
            !entity->GetComponent<StatsComponent>().IsDead()) {
            enemySideDefeated = false;
            break;
        }
    }
    
    return playerSideDefeated || enemySideDefeated;
}

void TurnManager::Reset() {
    // Clear the queue
    while (!turnQueue.empty()) {
        turnQueue.pop();
    }
    
    // Clear tracking data
    entitiesInCurrentRound.clear();
    
    currentEntity = nullptr;
    currentRound = 0;
}

std::shared_ptr<Entity> TurnManager::GetCurrentEntity() const {
    return currentEntity;
}

size_t TurnManager::GetQueueSize() const {
    return turnQueue.size();
}

std::vector<std::shared_ptr<Entity>> TurnManager::GetTurnOrder() const {
    std::vector<std::shared_ptr<Entity>> result;
    
    // Since priority_queue doesn't allow iteration, we need to make a copy
    auto queueCopy = turnQueue;
    
    // Add current entity if it exists
    if (currentEntity) {
        result.push_back(currentEntity);
    }
    
    // Extract entities from the queue in order
    while (!queueCopy.empty()) {
        result.push_back(queueCopy.top().entity);
        queueCopy.pop();
    }
    
    return result;
}

bool TurnManager::IsEntityOnPlayerSide(const Entity* entity) const {
    if (!entity || !entity->HasComponent<PositionComponent>()) {
        return false;
    }
    
    // Player side is positions 0-3, enemy side is 4-7
    // Need to const_cast since GetComponent isn't const
    return const_cast<Entity*>(entity)->GetComponent<PositionComponent>().GetPosition() < 4;
}

void TurnManager::PrepareNextRound() {
    currentRound++;
    std::cout << "------- Round " << currentRound << " begins -------" << std::endl;
    
    // Re-add all active entities to the queue for the next round
    for (const auto& entity : entitiesInCurrentRound) {
        if (entity && entity->HasComponent<StatsComponent>() && !entity->GetComponent<StatsComponent>().IsDead()) {
            int speed = entity->GetComponent<StatsComponent>().GetCurrentStat(StatType::SPEED);
            turnQueue.push(Turn(entity, speed));
        }
    }
}

} // namespace Game 
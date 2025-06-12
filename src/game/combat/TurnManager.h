#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <unordered_map>
#include "../entities/Entity.h"

namespace Game {

// Forward declarations
class Battlefield;
class Action;

// Struct to represent a turn in the queue
struct Turn {
    std::shared_ptr<Entity> entity;
    int initiative;  // Higher initiative goes first (based directly on speed)
    
    // Constructor
    Turn(std::shared_ptr<Entity> e, int init)
        : entity(e), initiative(init) {}
    
    // Comparison operator for priority queue (higher initiative goes first)
    bool operator<(const Turn& other) const {
        return initiative < other.initiative;
    }
};

// Class to manage the turn-based combat system
class TurnManager {
public:
    // Constructor
    TurnManager();
    
    // Initialize the turn order with the given entities
    void Initialize(const std::vector<std::shared_ptr<Entity>>& entities);
    
    // Get the next entity whose turn it is
    std::shared_ptr<Entity> GetNextEntity();
    
    // End the current entity's turn and calculate next
    void EndTurn();
    
    // Check if the combat is over (all entities on one side defeated)
    bool IsCombatOver(Battlefield* battlefield) const;
    
    // Reset the turn manager
    void Reset();
    
    // Get the current entity
    std::shared_ptr<Entity> GetCurrentEntity() const;
    
    // Get the queue size
    size_t GetQueueSize() const;
    
    // Get all entities in the turn order for display
    std::vector<std::shared_ptr<Entity>> GetTurnOrder() const;
    
private:
    // Check if an entity is on the player's side
    bool IsEntityOnPlayerSide(const Entity* entity) const;
    
    // Prepare the next round by re-adding entities to the queue
    void PrepareNextRound();
    
    // Priority queue to manage turn order (max heap - highest initiative first)
    std::priority_queue<Turn> turnQueue;
    
    // Current entity taking its turn
    std::shared_ptr<Entity> currentEntity;
    
    // Track all entities in the current combat
    std::vector<std::shared_ptr<Entity>> entitiesInCurrentRound;
    
    // Current round number
    int currentRound;
};

} // namespace Game 
#pragma once

#include <vector>
#include <memory>
#include "../entities/Entity.h"
#include "../entities/components/PositionComponent.h"

namespace Game {

// Represents the 8-tile linear strip battlefield
class Battlefield {
public:
    // Constants
    static const int MAX_TILES = 8;
    
    // Constructor
    Battlefield();
    
    // Place an entity on the battlefield
    bool PlaceEntity(std::shared_ptr<Entity> entity, int position);
    
    // Move an entity to a new position
    bool MoveEntity(Entity* entity, int newPosition);
    
    // Get entity at a specific position
    Entity* GetEntityAtPosition(int position) const;
    
    // Check if a position is valid
    bool IsValidPosition(int position) const;
    
    // Check if a position is occupied
    bool IsPositionOccupied(int position) const;
    
    // Check if an entity can move to a position
    bool CanMoveTo(Entity* entity, int newPosition) const;
    
    // Get all entities on the battlefield
    const std::vector<std::shared_ptr<Entity>>& GetEntities() const;
    
    // Get player side entities (positions 0-3)
    std::vector<Entity*> GetPlayerSideEntities() const;
    
    // Get enemy side entities (positions 4-7)
    std::vector<Entity*> GetEnemySideEntities() const;
    
    // Check if entity is on player side
    bool IsOnPlayerSide(const Entity* entity) const;
    
    // Check if entity is on enemy side
    bool IsOnEnemySide(const Entity* entity) const;
    
    // Calculate distance between two positions
    int GetDistance(int positionA, int positionB) const;
    
    // Clear the battlefield
    void Clear();
    
private:
    // List of all entities on the battlefield
    std::vector<std::shared_ptr<Entity>> entities;
    
    // Array of positions, storing the index of the entity at each position
    // -1 means empty
    int positions[MAX_TILES];
    
    // Get the index of an entity in the entities vector
    int GetEntityIndex(const Entity* entity) const;
};

} // namespace Game 
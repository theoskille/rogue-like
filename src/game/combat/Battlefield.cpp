#include "Battlefield.h"
#include <algorithm>
#include <iostream>

namespace Game {

Battlefield::Battlefield() {
    // Initialize all positions as empty (-1)
    for (int i = 0; i < MAX_TILES; i++) {
        positions[i] = -1;
    }
}

bool Battlefield::PlaceEntity(std::shared_ptr<Entity> entity, int position) {
    // Check if the position is valid
    if (!IsValidPosition(position)) {
        std::cout << "Invalid position: " << position << std::endl;
        return false;
    }
    
    // Check if the position is already occupied
    if (IsPositionOccupied(position)) {
        std::cout << "Position " << position << " is already occupied" << std::endl;
        return false;
    }
    
    // Make sure the entity has a position component
    if (!entity->HasComponent<PositionComponent>()) {
        std::cout << "Entity does not have a PositionComponent, adding one" << std::endl;
        // Create a position component first, then add it
        PositionComponent posComp;
        posComp.SetPosition(position);
        entity->AddComponent<PositionComponent>() = posComp;
    } else {
        entity->GetComponent<PositionComponent>().SetPosition(position);
    }
    
    // Add entity to list and update position array
    entities.push_back(entity);
    positions[position] = entities.size() - 1;
    
    return true;
}

bool Battlefield::MoveEntity(Entity* entity, int newPosition) {
    // Check if the entity exists on the battlefield
    int entityIndex = GetEntityIndex(entity);
    if (entityIndex == -1) {
        std::cout << "Entity not found on battlefield" << std::endl;
        return false;
    }
    
    // Check if the new position is valid
    if (!IsValidPosition(newPosition)) {
        std::cout << "Invalid new position: " << newPosition << std::endl;
        return false;
    }
    
    // Check if the new position is already occupied
    if (IsPositionOccupied(newPosition)) {
        std::cout << "New position " << newPosition << " is already occupied" << std::endl;
        return false;
    }
    
    // Get the entity's position component
    if (!entity->HasComponent<PositionComponent>()) {
        std::cout << "Entity does not have a PositionComponent" << std::endl;
        return false;
    }
    
    // Get the current position
    int currentPosition = entity->GetComponent<PositionComponent>().GetPosition();
    
    // Update the position component
    entity->GetComponent<PositionComponent>().SetPosition(newPosition);
    
    // Update the position array
    positions[currentPosition] = -1;
    positions[newPosition] = entityIndex;
    
    return true;
}

Entity* Battlefield::GetEntityAtPosition(int position) const {
    if (!IsValidPosition(position)) {
        return nullptr;
    }
    
    int entityIndex = positions[position];
    if (entityIndex == -1) {
        return nullptr;
    }
    
    return entities[entityIndex].get();
}

bool Battlefield::IsValidPosition(int position) const {
    return position >= 0 && position < MAX_TILES;
}

bool Battlefield::IsPositionOccupied(int position) const {
    if (!IsValidPosition(position)) {
        return false;
    }
    
    return positions[position] != -1;
}

bool Battlefield::CanMoveTo(Entity* entity, int newPosition) const {
    // Check if the entity exists on the battlefield
    int entityIndex = GetEntityIndex(entity);
    if (entityIndex == -1) {
        return false;
    }
    
    // Check if the new position is valid
    if (!IsValidPosition(newPosition)) {
        return false;
    }
    
    // Check if the new position is occupied
    if (IsPositionOccupied(newPosition)) {
        return false;
    }
    
    // Get the entity's position component
    if (!entity->HasComponent<PositionComponent>()) {
        return false;
    }
    
    // We no longer need to check the current position for path obstruction
    // as movement is now allowed as long as the destination is valid and unoccupied
    
    return true;
}

const std::vector<std::shared_ptr<Entity>>& Battlefield::GetEntities() const {
    return entities;
}

std::vector<Entity*> Battlefield::GetPlayerSideEntities() const {
    std::vector<Entity*> result;
    
    for (int i = 0; i < 4; i++) {
        Entity* entity = GetEntityAtPosition(i);
        if (entity) {
            result.push_back(entity);
        }
    }
    
    return result;
}

std::vector<Entity*> Battlefield::GetEnemySideEntities() const {
    std::vector<Entity*> result;
    
    for (int i = 4; i < MAX_TILES; i++) {
        Entity* entity = GetEntityAtPosition(i);
        if (entity) {
            result.push_back(entity);
        }
    }
    
    return result;
}

bool Battlefield::IsOnPlayerSide(const Entity* entity) const {
    int entityIndex = GetEntityIndex(entity);
    if (entityIndex == -1) {
        return false;
    }
    
    if (!entity->HasComponent<PositionComponent>()) {
        return false;
    }
    
    // Use const_cast since Entity::GetComponent isn't const-qualified
    Entity* nonConstEntity = const_cast<Entity*>(entity);
    int position = nonConstEntity->GetComponent<PositionComponent>().GetPosition();
    return position >= 0 && position < 4;
}

bool Battlefield::IsOnEnemySide(const Entity* entity) const {
    int entityIndex = GetEntityIndex(entity);
    if (entityIndex == -1) {
        return false;
    }
    
    if (!entity->HasComponent<PositionComponent>()) {
        return false;
    }
    
    // Use const_cast since Entity::GetComponent isn't const-qualified
    Entity* nonConstEntity = const_cast<Entity*>(entity);
    int position = nonConstEntity->GetComponent<PositionComponent>().GetPosition();
    return position >= 4 && position < MAX_TILES;
}

int Battlefield::GetDistance(int positionA, int positionB) const {
    if (!IsValidPosition(positionA) || !IsValidPosition(positionB)) {
        return -1;
    }
    
    return std::abs(positionA - positionB);
}

void Battlefield::Clear() {
    entities.clear();
    
    for (int i = 0; i < MAX_TILES; i++) {
        positions[i] = -1;
    }
}

int Battlefield::GetEntityIndex(const Entity* entity) const {
    for (size_t i = 0; i < entities.size(); i++) {
        if (entities[i].get() == entity) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

} // namespace Game 
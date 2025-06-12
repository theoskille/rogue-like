#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace Game {

// Forward declarations
class Encounter;

// Enum for room types
enum class RoomType {
    NORMAL,     // Standard room with combat encounter
    TREASURE,   // Room with loot
    BOSS,       // Room with boss fight
    ENTRANCE,   // Starting room
    EXIT        // Exit to next floor
};

/**
 * Represents a single room in the dungeon
 */
class Room {
public:
    Room(int id, RoomType type);
    ~Room();
    
    // Getters
    int GetId() const { return id; }
    RoomType GetType() const { return type; }
    const std::string& GetDescription() const { return description; }
    bool IsVisited() const { return visited; }
    bool IsCleared() const { return cleared; }
    
    // Connected rooms
    void AddConnection(std::shared_ptr<Room> room);
    void RemoveConnection(int roomId);
    bool IsConnectedTo(int roomId) const;
    const std::vector<std::shared_ptr<Room>>& GetConnections() const;
    
    // Visit and clear state
    void Visit();
    void Clear();
    
    // Encounter management
    void SetEncounter(std::shared_ptr<Encounter> encounter);
    std::shared_ptr<Encounter> GetEncounter() const;
    
    // Properties
    void SetProperty(const std::string& key, const std::string& value);
    std::string GetProperty(const std::string& key) const;
    bool HasProperty(const std::string& key) const;
    
    // Set room description
    void SetDescription(const std::string& desc) { description = desc; }
    
    // Position in the dungeon layout (for visualization)
    void SetPosition(int x, int y) { posX = x; posY = y; }
    int GetPositionX() const { return posX; }
    int GetPositionY() const { return posY; }
    
private:
    int id;                     // Unique identifier
    RoomType type;              // Type of room
    std::string description;    // Room description
    bool visited;               // Has player visited
    bool cleared;               // Has encounter been cleared
    
    // Position in dungeon layout
    int posX;
    int posY;
    
    // Connected rooms
    std::vector<std::shared_ptr<Room>> connections;
    
    // Room encounter
    std::shared_ptr<Encounter> encounter;
    
    // Custom properties
    std::unordered_map<std::string, std::string> properties;
};

} // namespace Game 
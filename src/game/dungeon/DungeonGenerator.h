#pragma once

#include "Room.h"
#include <memory>
#include <vector>
#include <random>
#include <unordered_map>
#include <functional>

namespace Game {

/**
 * Parameters for dungeon generation
 */
struct DungeonGenerationParams {
    int width = 5;               // Width of the grid
    int height = 5;              // Height of the grid
    int numRooms = 12;           // Number of rooms to generate
    int minRoomsPerFloor = 10;   // Minimum number of rooms
    int maxRoomsPerFloor = 20;   // Maximum number of rooms
    int numTreasureRooms = 2;    // Number of treasure rooms
    bool hasBossRoom = true;     // Whether to include a boss room
    int difficulty = 1;          // Overall difficulty level
    float loopChance = 0.2f;     // Chance to create loops in the dungeon (0.0 - 1.0)
    
    // Constructor with defaults
    DungeonGenerationParams() = default;
    
    // Constructor with custom parameters
    DungeonGenerationParams(int width, int height, int numRooms, int treasureRooms, 
                          bool hasBoss, int difficulty, float loopChance)
        : width(width), height(height), numRooms(numRooms), 
          numTreasureRooms(treasureRooms), hasBossRoom(hasBoss),
          difficulty(difficulty), loopChance(loopChance) {
        
        // Ensure valid bounds
        if (width < 3) width = 3;
        if (height < 3) height = 3;
        if (numRooms < 5) numRooms = 5;
        if (numTreasureRooms > numRooms / 3) numTreasureRooms = numRooms / 3;
        if (difficulty < 1) difficulty = 1;
        if (loopChance < 0.0f) loopChance = 0.0f;
        if (loopChance > 1.0f) loopChance = 1.0f;
    }
};

/**
 * Manages procedural generation of dungeon floors
 */
class DungeonGenerator {
public:
    // Get singleton instance
    static DungeonGenerator& GetInstance();
    
    // Generate a new dungeon floor
    std::vector<std::shared_ptr<Room>> GenerateFloor(const DungeonGenerationParams& params = DungeonGenerationParams());
    
    // Clear all dungeon data
    void Clear();
    
    // Get the entrance room
    std::shared_ptr<Room> GetEntranceRoom() const;
    
    // Get the exit room
    std::shared_ptr<Room> GetExitRoom() const;
    
    // Get all rooms
    const std::vector<std::shared_ptr<Room>>& GetAllRooms() const;
    
    // Get specific room by ID
    std::shared_ptr<Room> GetRoom(int id) const;
    
private:
    // Private constructor for singleton
    DungeonGenerator();
    
    // Random engine
    std::mt19937 rng;
    
    // Current dungeon state
    std::vector<std::shared_ptr<Room>> rooms;
    std::shared_ptr<Room> entranceRoom;
    std::shared_ptr<Room> exitRoom;
    std::shared_ptr<Room> bossRoom;
    
    // Map for room placement grid
    using RoomGrid = std::vector<std::vector<std::shared_ptr<Room>>>;
    RoomGrid grid;
    
    // Helper methods
    void InitializeGrid(int width, int height);
    void PlaceRoomAt(int x, int y, std::shared_ptr<Room> room);
    std::shared_ptr<Room> CreateRoom(int id, RoomType type, int x, int y);
    void ConnectRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2);
    bool TryConnectAdjacentRooms(int x, int y);
    void CreateRandomLoops(float loopChance);
    void AssignEncounters(int difficulty);
    void ValidateDungeon();
};

} // namespace Game 
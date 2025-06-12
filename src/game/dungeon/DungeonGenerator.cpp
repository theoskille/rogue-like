#include "DungeonGenerator.h"
#include "encounters/CombatEncounter.h"
#include "encounters/TreasureEncounter.h"
#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>
#include <random>
#include <chrono>
#include <sstream>

namespace Game {

// Initialize singleton instance
DungeonGenerator& DungeonGenerator::GetInstance() {
    static DungeonGenerator instance;
    return instance;
}

DungeonGenerator::DungeonGenerator()
    : rng(std::random_device()()) {
    // Seed random generator with current time
    rng.seed(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
}

void DungeonGenerator::Clear() {
    rooms.clear();
    entranceRoom = nullptr;
    exitRoom = nullptr;
    bossRoom = nullptr;
    grid.clear();
}

std::vector<std::shared_ptr<Room>> DungeonGenerator::GenerateFloor(const DungeonGenerationParams& params) {
    // Clear any existing dungeon data
    Clear();
    
    std::cout << "Generating dungeon floor with parameters:" << std::endl;
    std::cout << "  - Width: " << params.width << std::endl;
    std::cout << "  - Height: " << params.height << std::endl;
    std::cout << "  - Number of rooms: " << params.numRooms << std::endl;
    std::cout << "  - Treasure rooms: " << params.numTreasureRooms << std::endl;
    std::cout << "  - Boss room: " << (params.hasBossRoom ? "Yes" : "No") << std::endl;
    std::cout << "  - Difficulty: " << params.difficulty << std::endl;
    std::cout << "  - Loop chance: " << params.loopChance << std::endl;
    
    // Initialize the grid
    InitializeGrid(params.width, params.height);
    
    // Calculate number of rooms to generate
    int numRooms = params.numRooms;
    
    // Ensure we don't exceed grid capacity
    int maxPossibleRooms = params.width * params.height;
    if (numRooms > maxPossibleRooms) {
        numRooms = maxPossibleRooms;
    }
    
    // Ensure we meet minimum requirements
    if (numRooms < 5) numRooms = 5; // Need at least entrance, exit, and a few normal rooms
    
    // Step 1: Place the entrance room
    int entranceX = 0;
    int entranceY = params.height / 2;
    entranceRoom = CreateRoom(0, RoomType::ENTRANCE, entranceX, entranceY);
    PlaceRoomAt(entranceX, entranceY, entranceRoom);
    rooms.push_back(entranceRoom);
    
    // Step 2: Generate a path from entrance to exit using a random walk
    std::stack<std::pair<int, int>> path;
    path.push(std::make_pair(entranceX, entranceY));
    
    // Current position
    int currentX = entranceX;
    int currentY = entranceY;
    
    // Target position for exit (far side of map)
    int exitX = params.width - 1;
    int exitY = params.height / 2;
    
    // If we need a boss room, we'll place it before the exit
    int bossX = exitX - 1;
    int bossY = exitY;
    
    // Track placed rooms (excluding entrance which is already placed)
    int placedRooms = 1;
    
    // Generate main path with random walk
    while (placedRooms < numRooms && !path.empty()) {
        // Get current position
        currentX = path.top().first;
        currentY = path.top().second;
        
        // Possible directions (right, down, left, up)
        std::vector<std::pair<int, int>> directions = {
            {1, 0}, {0, 1}, {-1, 0}, {0, -1}
        };
        
        // Shuffle directions for randomness
        std::shuffle(directions.begin(), directions.end(), rng);
        
        bool movedToNewRoom = false;
        
        // Try each direction
        for (const auto& dir : directions) {
            int newX = currentX + dir.first;
            int newY = currentY + dir.second;
            
            // Check if valid position
            if (newX >= 0 && newX < params.width && newY >= 0 && newY < params.height) {
                // Check if position is empty
                if (!grid[newY][newX]) {
                    // Determine room type
                    RoomType roomType = RoomType::NORMAL;
                    
                    // Check if we're at the exit position
                    if (newX == exitX && newY == exitY) {
                        roomType = RoomType::EXIT;
                    } 
                    // Check if we're at the boss position and boss rooms are enabled
                    else if (params.hasBossRoom && newX == bossX && newY == bossY) {
                        roomType = RoomType::BOSS;
                    }
                    
                    // Create and place the room
                    auto room = CreateRoom(rooms.size(), roomType, newX, newY);
                    PlaceRoomAt(newX, newY, room);
                    rooms.push_back(room);
                    
                    // Connect to the previous room
                    ConnectRooms(grid[currentY][currentX], room);
                    
                    // Update special room pointers
                    if (roomType == RoomType::EXIT) {
                        exitRoom = room;
                    } else if (roomType == RoomType::BOSS) {
                        bossRoom = room;
                    }
                    
                    // Push new position onto the path
                    path.push(std::make_pair(newX, newY));
                    
                    // Update counters
                    placedRooms++;
                    movedToNewRoom = true;
                    break;
                }
            }
        }
        
        // If we couldn't move to a new room, backtrack
        if (!movedToNewRoom) {
            path.pop();
        }
        
        // If we've placed enough rooms, stop
        if (placedRooms >= numRooms) {
            break;
        }
    }
    
    // If we didn't place an exit room yet, make sure to place one
    if (!exitRoom) {
        // Find the farthest room from the entrance to make it the exit
        int maxDistance = 0;
        std::shared_ptr<Room> farthestRoom = nullptr;
        
        for (int y = 0; y < params.height; y++) {
            for (int x = 0; x < params.width; x++) {
                if (grid[y][x]) {
                    int distance = std::abs(x - entranceX) + std::abs(y - entranceY);
                    if (distance > maxDistance && grid[y][x]->GetType() == RoomType::NORMAL) {
                        maxDistance = distance;
                        farthestRoom = grid[y][x];
                    }
                }
            }
        }
        
        // Convert the farthest room to an exit
        if (farthestRoom) {
            farthestRoom = CreateRoom(farthestRoom->GetId(), RoomType::EXIT, 
                                     farthestRoom->GetPositionX(), farthestRoom->GetPositionY());
            PlaceRoomAt(farthestRoom->GetPositionX(), farthestRoom->GetPositionY(), farthestRoom);
            
            // Update the rooms vector
            for (size_t i = 0; i < rooms.size(); i++) {
                if (rooms[i]->GetId() == farthestRoom->GetId()) {
                    rooms[i] = farthestRoom;
                    break;
                }
            }
            
            exitRoom = farthestRoom;
        }
    }
    
    // If boss rooms are enabled but we didn't place one, convert a room near the exit
    if (params.hasBossRoom && !bossRoom && exitRoom) {
        // Find a room adjacent to the exit
        int exitX = exitRoom->GetPositionX();
        int exitY = exitRoom->GetPositionY();
        
        std::vector<std::pair<int, int>> directions = {
            {1, 0}, {0, 1}, {-1, 0}, {0, -1}
        };
        
        for (const auto& dir : directions) {
            int adjX = exitX + dir.first;
            int adjY = exitY + dir.second;
            
            if (adjX >= 0 && adjX < params.width && adjY >= 0 && adjY < params.height) {
                if (grid[adjY][adjX] && grid[adjY][adjX]->GetType() == RoomType::NORMAL) {
                    // Convert to boss room
                    auto oldRoom = grid[adjY][adjX];
                    auto bossRoom = CreateRoom(oldRoom->GetId(), RoomType::BOSS, adjX, adjY);
                    PlaceRoomAt(adjX, adjY, bossRoom);
                    
                    // Update the rooms vector
                    for (size_t i = 0; i < rooms.size(); i++) {
                        if (rooms[i]->GetId() == oldRoom->GetId()) {
                            rooms[i] = bossRoom;
                            break;
                        }
                    }
                    
                    this->bossRoom = bossRoom;
                    break;
                }
            }
        }
    }
    
    // Step 3: Add additional connections to create loops
    CreateRandomLoops(params.loopChance);
    
    // Step 4: Convert some normal rooms to treasure rooms
    int treasureRoomsToCreate = std::min(params.numTreasureRooms, 
                                       static_cast<int>(rooms.size() - 3)); // -3 for entrance, exit, and boss
    
    std::vector<int> normalRoomIndices;
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->GetType() == RoomType::NORMAL) {
            normalRoomIndices.push_back(i);
        }
    }
    
    // Shuffle to randomize selection
    std::shuffle(normalRoomIndices.begin(), normalRoomIndices.end(), rng);
    
    // Convert normal rooms to treasure rooms
    for (int i = 0; i < treasureRoomsToCreate && i < static_cast<int>(normalRoomIndices.size()); i++) {
        int roomIndex = normalRoomIndices[i];
        auto oldRoom = rooms[roomIndex];
        int x = oldRoom->GetPositionX();
        int y = oldRoom->GetPositionY();
        
        auto treasureRoom = CreateRoom(oldRoom->GetId(), RoomType::TREASURE, x, y);
        PlaceRoomAt(x, y, treasureRoom);
        rooms[roomIndex] = treasureRoom;
    }
    
    // Step 5: Assign encounters to rooms based on difficulty
    AssignEncounters(params.difficulty);
    
    // Step 6: Validate the dungeon to ensure it's playable
    ValidateDungeon();
    
    std::cout << "Dungeon generation complete! Generated " << rooms.size() << " rooms." << std::endl;
    return rooms;
}

void DungeonGenerator::InitializeGrid(int width, int height) {
    grid.clear();
    grid.resize(height, std::vector<std::shared_ptr<Room>>(width, nullptr));
}

void DungeonGenerator::PlaceRoomAt(int x, int y, std::shared_ptr<Room> room) {
    if (x >= 0 && x < static_cast<int>(grid[0].size()) && 
        y >= 0 && y < static_cast<int>(grid.size())) {
        grid[y][x] = room;
        room->SetPosition(x, y);
    }
}

std::shared_ptr<Room> DungeonGenerator::CreateRoom(int id, RoomType type, int x, int y) {
    auto room = std::make_shared<Room>(id, type);
    room->SetPosition(x, y);
    
    // Set custom descriptions based on room type
    switch (type) {
        case RoomType::ENTRANCE:
            room->SetDescription("The entrance to the dungeon floor. A cold draft blows from deeper within.");
            break;
        case RoomType::EXIT:
            room->SetDescription("A staircase leading to the next floor of the dungeon awaits.");
            break;
        case RoomType::BOSS:
            room->SetDescription("An imposing chamber with strange markings. Something powerful lurks here.");
            break;
        case RoomType::TREASURE:
            room->SetDescription("A room filled with glittering gold and valuable treasures. What riches await?");
            break;
        case RoomType::NORMAL:
            // Random descriptions for normal rooms
            {
                std::uniform_int_distribution<int> dist(0, 5);
                int descType = dist(rng);
                
                switch (descType) {
                    case 0:
                        room->SetDescription("A damp chamber with water dripping from the ceiling.");
                        break;
                    case 1:
                        room->SetDescription("Ancient runes cover the walls of this mysterious room.");
                        break;
                    case 2:
                        room->SetDescription("Cobwebs fill the corners of this neglected area.");
                        break;
                    case 3:
                        room->SetDescription("The remnants of a camp suggest others have passed through recently.");
                        break;
                    case 4:
                        room->SetDescription("Broken furniture and debris litter this once-inhabited room.");
                        break;
                    default:
                        room->SetDescription("A standard dungeon chamber with stone walls and flickering torches.");
                        break;
                }
            }
            break;
    }
    
    return room;
}

void DungeonGenerator::ConnectRooms(std::shared_ptr<Room> room1, std::shared_ptr<Room> room2) {
    if (room1 && room2) {
        room1->AddConnection(room2);
    }
}

bool DungeonGenerator::TryConnectAdjacentRooms(int x, int y) {
    if (x < 0 || y < 0 || x >= static_cast<int>(grid[0].size()) || y >= static_cast<int>(grid.size())) {
        return false;
    }
    
    if (!grid[y][x]) {
        return false;
    }
    
    bool madeConnection = false;
    
    // Possible directions (right, down, left, up)
    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}
    };
    
    // Try to connect to each adjacent room
    for (const auto& dir : directions) {
        int adjX = x + dir.first;
        int adjY = y + dir.second;
        
        if (adjX >= 0 && adjX < static_cast<int>(grid[0].size()) && 
            adjY >= 0 && adjY < static_cast<int>(grid.size())) {
            
            if (grid[adjY][adjX]) {
                // Connect if not already connected
                if (!grid[y][x]->IsConnectedTo(grid[adjY][adjX]->GetId())) {
                    ConnectRooms(grid[y][x], grid[adjY][adjX]);
                    madeConnection = true;
                }
            }
        }
    }
    
    return madeConnection;
}

void DungeonGenerator::CreateRandomLoops(float loopChance) {
    // Only create loops if the chance is positive
    if (loopChance <= 0.0f) {
        return;
    }
    
    int width = grid[0].size();
    int height = grid.size();
    
    // Try to connect adjacent rooms that aren't already connected
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x]) {
                // For each room, consider connecting to adjacent rooms
                std::uniform_real_distribution<float> dist(0.0f, 1.0f);
                if (dist(rng) < loopChance) {
                    TryConnectAdjacentRooms(x, y);
                }
            }
        }
    }
}

void DungeonGenerator::AssignEncounters(int difficulty) {
    // Assign encounters to each room based on type
    for (auto& room : rooms) {
        switch (room->GetType()) {
            case RoomType::NORMAL: {
                // Create combat encounter with difficulty proportional to distance from entrance
                int distFromEntrance = std::abs(room->GetPositionX() - entranceRoom->GetPositionX()) + 
                                    std::abs(room->GetPositionY() - entranceRoom->GetPositionY());
                
                // Scale encounter difficulty from 1 to difficulty+2
                int encounterDifficulty = 1 + (distFromEntrance * difficulty) / 
                                        (grid.size() + grid[0].size());
                
                // Cap difficulty
                encounterDifficulty = std::min(encounterDifficulty, difficulty + 2);
                
                // Create a unique name for the encounter
                std::stringstream ss;
                ss << "Combat Encounter " << room->GetId();
                
                auto encounter = std::make_shared<CombatEncounter>(ss.str(), encounterDifficulty);
                room->SetEncounter(encounter);
                break;
            }
            
            case RoomType::TREASURE: {
                // Create treasure encounter with quality proportional to difficulty
                int quality = 1 + difficulty / 2;
                
                // Create a unique name for the encounter
                std::stringstream ss;
                ss << "Treasure Chest " << room->GetId();
                
                auto encounter = std::make_shared<TreasureEncounter>(ss.str(), quality);
                room->SetEncounter(encounter);
                break;
            }
            
            case RoomType::BOSS: {
                // Create high-difficulty combat encounter
                int bossDifficulty = difficulty + 3;
                
                // Create a unique name for the encounter
                std::stringstream ss;
                ss << "Boss Encounter " << room->GetId();
                
                auto encounter = std::make_shared<CombatEncounter>(ss.str(), bossDifficulty);
                room->SetEncounter(encounter);
                break;
            }
            
            // Entrance and exit rooms don't have encounters
            default:
                break;
        }
    }
}

void DungeonGenerator::ValidateDungeon() {
    // Check if we have an entrance and exit
    if (!entranceRoom) {
        std::cerr << "ERROR: Dungeon has no entrance room!" << std::endl;
    }
    
    if (!exitRoom) {
        std::cerr << "ERROR: Dungeon has no exit room!" << std::endl;
    }
    
    // Verify connectivity using BFS
    if (entranceRoom && exitRoom) {
        // Set all rooms as unvisited
        std::unordered_map<int, bool> visited;
        for (const auto& room : rooms) {
            visited[room->GetId()] = false;
        }
        
        // BFS from entrance
        std::queue<std::shared_ptr<Room>> queue;
        queue.push(entranceRoom);
        visited[entranceRoom->GetId()] = true;
        
        while (!queue.empty()) {
            auto current = queue.front();
            queue.pop();
            
            for (const auto& neighbor : current->GetConnections()) {
                if (!visited[neighbor->GetId()]) {
                    visited[neighbor->GetId()] = true;
                    queue.push(neighbor);
                }
            }
        }
        
        // Check if exit is reachable
        if (!visited[exitRoom->GetId()]) {
            std::cerr << "ERROR: Exit is not reachable from entrance!" << std::endl;
            
            // Find closest room to exit that is reachable
            std::shared_ptr<Room> closestRoom = nullptr;
            int minDistance = std::numeric_limits<int>::max();
            
            for (const auto& room : rooms) {
                if (visited[room->GetId()]) {
                    int distance = std::abs(room->GetPositionX() - exitRoom->GetPositionX()) + 
                                std::abs(room->GetPositionY() - exitRoom->GetPositionY());
                    
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestRoom = room;
                    }
                }
            }
            
            // Connect closest reachable room to exit
            if (closestRoom) {
                std::cout << "Fixing dungeon: Connecting room " << closestRoom->GetId() 
                         << " to exit (room " << exitRoom->GetId() << ")" << std::endl;
                ConnectRooms(closestRoom, exitRoom);
            }
        }
        
        // Check if any rooms are unreachable
        for (const auto& room : rooms) {
            if (!visited[room->GetId()]) {
                std::cout << "Warning: Room " << room->GetId() << " is not reachable from entrance." << std::endl;
            }
        }
    }
}

std::shared_ptr<Room> DungeonGenerator::GetEntranceRoom() const {
    return entranceRoom;
}

std::shared_ptr<Room> DungeonGenerator::GetExitRoom() const {
    return exitRoom;
}

const std::vector<std::shared_ptr<Room>>& DungeonGenerator::GetAllRooms() const {
    return rooms;
}

std::shared_ptr<Room> DungeonGenerator::GetRoom(int id) const {
    auto it = std::find_if(rooms.begin(), rooms.end(),
                         [id](const std::shared_ptr<Room>& room) {
                             return room->GetId() == id;
                         });
    
    if (it != rooms.end()) {
        return *it;
    }
    
    return nullptr;
}

} // namespace Game 
#include "Room.h"
#include <algorithm>
#include <iostream>

namespace Game {

Room::Room(int id, RoomType type)
    : id(id),
      type(type),
      visited(false),
      cleared(false),
      posX(0),
      posY(0),
      encounter(nullptr) {
    
    // Set default description based on room type
    switch (type) {
        case RoomType::NORMAL:
            description = "A standard dungeon room with stone walls and dim lighting.";
            break;
        case RoomType::TREASURE:
            description = "A room filled with glittering treasures and valuable items.";
            break;
        case RoomType::BOSS:
            description = "A large chamber with ominous decorations, perfect for a powerful foe.";
            break;
        case RoomType::ENTRANCE:
            description = "The entrance to this floor of the dungeon.";
            break;
        case RoomType::EXIT:
            description = "A room with stairs leading to the next level of the dungeon.";
            break;
        default:
            description = "An unremarkable room in the dungeon.";
            break;
    }
    
    std::cout << "Created room " << id << " of type " << static_cast<int>(type) << std::endl;
}

Room::~Room() {
    std::cout << "Destroyed room " << id << std::endl;
    // Clear connections to avoid circular references
    connections.clear();
}

void Room::AddConnection(std::shared_ptr<Room> room) {
    // Check if already connected
    if (!room || IsConnectedTo(room->GetId())) {
        return;
    }
    
    // Add connection
    connections.push_back(room);
    
    // Make connection bidirectional
    if (!room->IsConnectedTo(id)) {
        room->AddConnection(std::shared_ptr<Room>(this, [](Room*){})); // Non-owning pointer
    }
    
    std::cout << "Connected room " << id << " to room " << room->GetId() << std::endl;
}

void Room::RemoveConnection(int roomId) {
    // Find and remove the connection
    auto it = std::find_if(connections.begin(), connections.end(),
                          [roomId](const std::shared_ptr<Room>& room) {
                              return room->GetId() == roomId;
                          });
    
    if (it != connections.end()) {
        // Get the room before removing the connection
        auto room = *it;
        
        // Remove the connection
        connections.erase(it);
        
        // Remove the reverse connection
        if (room && room->IsConnectedTo(id)) {
            room->RemoveConnection(id);
        }
        
        std::cout << "Removed connection between room " << id << " and room " << roomId << std::endl;
    }
}

bool Room::IsConnectedTo(int roomId) const {
    return std::any_of(connections.begin(), connections.end(),
                      [roomId](const std::shared_ptr<Room>& room) {
                          return room->GetId() == roomId;
                      });
}

const std::vector<std::shared_ptr<Room>>& Room::GetConnections() const {
    return connections;
}

void Room::Visit() {
    if (!visited) {
        visited = true;
        std::cout << "Room " << id << " has been visited" << std::endl;
    }
}

void Room::Clear() {
    if (!cleared) {
        cleared = true;
        std::cout << "Room " << id << " has been cleared" << std::endl;
    }
}

void Room::SetEncounter(std::shared_ptr<Encounter> newEncounter) {
    encounter = newEncounter;
}

std::shared_ptr<Encounter> Room::GetEncounter() const {
    return encounter;
}

void Room::SetProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

std::string Room::GetProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return "";
}

bool Room::HasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

} // namespace Game 
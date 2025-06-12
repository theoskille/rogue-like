#include "RoomTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include <iostream>
#include <sstream>
#include <cmath>

namespace Game {

RoomTestState::RoomTestState()
    : uiState(UIState::MAIN_MENU),
      selectedRoomIndex(0),
      targetRoomIndex(0),
      selectedPropertyIndex(0),
      isEditing(false),
      isPaused(false) {
}

RoomTestState::~RoomTestState() {
    std::cout << "RoomTestState destroyed" << std::endl;
}

void RoomTestState::Enter() {
    std::cout << "Entering Room Test State" << std::endl;
    
    // Create a few initial test rooms
    CreateTestRoom(RoomType::ENTRANCE);
    CreateTestRoom(RoomType::NORMAL);
    CreateTestRoom(RoomType::TREASURE);
    CreateTestRoom(RoomType::BOSS);
    CreateTestRoom(RoomType::EXIT);
    
    // Set positions for visualization
    if (!rooms.empty()) {
        rooms[0]->SetPosition(0, 2); // ENTRANCE
        
        if (rooms.size() > 1) {
            rooms[1]->SetPosition(1, 1); // NORMAL
            
            if (rooms.size() > 2) {
                rooms[2]->SetPosition(2, 2); // TREASURE
                
                if (rooms.size() > 3) {
                    rooms[3]->SetPosition(3, 1); // BOSS
                    
                    if (rooms.size() > 4) {
                        rooms[4]->SetPosition(4, 2); // EXIT
                    }
                }
            }
        }
    }
    
    // Connect initial rooms to form a path
    for (size_t i = 0; i < rooms.size() - 1; ++i) {
        ConnectRooms(rooms[i]->GetId(), rooms[i + 1]->GetId());
    }
}

void RoomTestState::Exit() {
    std::cout << "Exiting Room Test State" << std::endl;
    
    // Clear rooms
    rooms.clear();
}

void RoomTestState::Pause() {
    std::cout << "Pausing Room Test State" << std::endl;
    isPaused = true;
}

void RoomTestState::Resume() {
    std::cout << "Resuming Room Test State" << std::endl;
    isPaused = false;
}

void RoomTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update based on current UI state
    switch (uiState) {
        case UIState::MAIN_MENU:
            UpdateMainMenu();
            break;
        case UIState::CREATE_ROOM:
            UpdateCreateRoom();
            break;
        case UIState::CONNECT_ROOMS:
            UpdateConnectRooms();
            break;
        case UIState::VIEW_ROOM:
            UpdateViewRoom();
            break;
        case UIState::VIEW_CONNECTIONS:
            UpdateViewConnections();
            break;
    }
    
    // Check for escape key to exit
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        if (uiState == UIState::MAIN_MENU) {
            // Return to previous state
            Engine::StateManager::GetInstance().PopState();
        } else {
            // Return to main menu
            uiState = UIState::MAIN_MENU;
        }
    }
}

void RoomTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawText("ROOM TEST STATE", 50, 30, 24, BLACK);
    
    // Draw room grid
    DrawRoomGrid();
    
    // Render based on current UI state
    switch (uiState) {
        case UIState::MAIN_MENU:
            RenderMainMenu();
            break;
        case UIState::CREATE_ROOM:
            RenderCreateRoom();
            break;
        case UIState::CONNECT_ROOMS:
            RenderConnectRooms();
            break;
        case UIState::VIEW_ROOM:
            RenderViewRoom();
            break;
        case UIState::VIEW_CONNECTIONS:
            RenderViewConnections();
            break;
    }
    
    // Draw help text
    renderer.DrawText("ESC: Back/Exit", 50, 700, 16, DARKGRAY);
}

void RoomTestState::CreateTestRoom(RoomType type) {
    // Generate a unique ID (simply use the current size)
    int id = static_cast<int>(rooms.size());
    
    // Create the room
    auto room = std::make_shared<Room>(id, type);
    
    // Set a random position for visualization
    room->SetPosition(id % 5, id / 5);
    
    // Add to room list
    rooms.push_back(room);
    
    std::cout << "Created test room with ID " << id << " of type " << RoomTypeToString(type) << std::endl;
}

void RoomTestState::ConnectRooms(int roomId1, int roomId2) {
    // Find rooms with the given IDs
    auto it1 = std::find_if(rooms.begin(), rooms.end(),
                           [roomId1](const std::shared_ptr<Room>& room) {
                               return room->GetId() == roomId1;
                           });
    
    auto it2 = std::find_if(rooms.begin(), rooms.end(),
                           [roomId2](const std::shared_ptr<Room>& room) {
                               return room->GetId() == roomId2;
                           });
    
    // Connect the rooms if found
    if (it1 != rooms.end() && it2 != rooms.end()) {
        (*it1)->AddConnection(*it2);
    } else {
        std::cout << "Failed to connect rooms: Room " << roomId1 << " or " << roomId2 << " not found" << std::endl;
    }
}

void RoomTestState::RemoveConnection(int roomId1, int roomId2) {
    // Find the first room
    auto it = std::find_if(rooms.begin(), rooms.end(),
                          [roomId1](const std::shared_ptr<Room>& room) {
                              return room->GetId() == roomId1;
                          });
    
    // Remove the connection if room found
    if (it != rooms.end()) {
        (*it)->RemoveConnection(roomId2);
    } else {
        std::cout << "Failed to remove connection: Room " << roomId1 << " not found" << std::endl;
    }
}

void RoomTestState::UpdateMainMenu() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Handle menu navigation
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedRoomIndex = (selectedRoomIndex - 1 + 5) % 5;
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedRoomIndex = (selectedRoomIndex + 1) % 5;
    }
    
    // Handle menu selection
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        switch (selectedRoomIndex) {
            case 0: // Create Room
                uiState = UIState::CREATE_ROOM;
                selectedRoomIndex = 0; // Reset selection
                break;
            case 1: // Connect Rooms
                if (rooms.size() >= 2) {
                    uiState = UIState::CONNECT_ROOMS;
                    selectedRoomIndex = 0;
                    targetRoomIndex = 1;
                }
                break;
            case 2: // View Room
                if (!rooms.empty()) {
                    uiState = UIState::VIEW_ROOM;
                    selectedRoomIndex = 0;
                }
                break;
            case 3: // View Connections
                if (!rooms.empty()) {
                    uiState = UIState::VIEW_CONNECTIONS;
                    selectedRoomIndex = 0;
                }
                break;
            case 4: // Exit
                Engine::StateManager::GetInstance().PopState();
                break;
        }
    }
}

void RoomTestState::UpdateCreateRoom() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Handle menu navigation
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedRoomIndex = (selectedRoomIndex - 1 + 5) % 5;
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedRoomIndex = (selectedRoomIndex + 1) % 5;
    }
    
    // Handle menu selection
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        // Create a room of the selected type
        RoomType type;
        switch (selectedRoomIndex) {
            case 0: type = RoomType::NORMAL; break;
            case 1: type = RoomType::TREASURE; break;
            case 2: type = RoomType::BOSS; break;
            case 3: type = RoomType::ENTRANCE; break;
            case 4: type = RoomType::EXIT; break;
            default: type = RoomType::NORMAL; break;
        }
        
        CreateTestRoom(type);
        uiState = UIState::MAIN_MENU;
    }
}

void RoomTestState::UpdateConnectRooms() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    static bool selectingTarget = false;
    
    if (!selectingTarget) {
        // Selecting source room
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP) || 
            input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
            selectedRoomIndex = (selectedRoomIndex - 1 + rooms.size()) % rooms.size();
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN) || 
            input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
            selectedRoomIndex = (selectedRoomIndex + 1) % rooms.size();
        }
        
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
            selectingTarget = true;
            targetRoomIndex = (selectedRoomIndex + 1) % rooms.size();
        }
    } else {
        // Selecting target room
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP) || 
            input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
            targetRoomIndex = (targetRoomIndex - 1 + rooms.size()) % rooms.size();
            if (targetRoomIndex == selectedRoomIndex) {
                targetRoomIndex = (targetRoomIndex - 1 + rooms.size()) % rooms.size();
            }
        }
        if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN) || 
            input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
            targetRoomIndex = (targetRoomIndex + 1) % rooms.size();
            if (targetRoomIndex == selectedRoomIndex) {
                targetRoomIndex = (targetRoomIndex + 1) % rooms.size();
            }
        }
        
        if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
            // Check if rooms are already connected
            auto sourceRoom = rooms[selectedRoomIndex];
            auto targetRoom = rooms[targetRoomIndex];
            
            if (sourceRoom->IsConnectedTo(targetRoom->GetId())) {
                // Disconnect rooms
                sourceRoom->RemoveConnection(targetRoom->GetId());
            } else {
                // Connect rooms
                sourceRoom->AddConnection(targetRoom);
            }
            
            selectingTarget = false;
            uiState = UIState::MAIN_MENU;
        }
    }
}

void RoomTestState::UpdateViewRoom() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Navigate between rooms
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
        selectedRoomIndex = (selectedRoomIndex - 1 + rooms.size()) % rooms.size();
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
        selectedRoomIndex = (selectedRoomIndex + 1) % rooms.size();
    }
    
    // Toggle visited/cleared state
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        if (selectedRoomIndex < static_cast<int>(rooms.size())) {
            rooms[selectedRoomIndex]->Visit();
        }
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        if (selectedRoomIndex < static_cast<int>(rooms.size())) {
            rooms[selectedRoomIndex]->Clear();
        }
    }
}

void RoomTestState::UpdateViewConnections() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Navigate between rooms
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
        selectedRoomIndex = (selectedRoomIndex - 1 + rooms.size()) % rooms.size();
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
        selectedRoomIndex = (selectedRoomIndex + 1) % rooms.size();
    }
}

void RoomTestState::RenderMainMenu() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 300;
    int menuHeight = 250;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("Main Menu", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw menu options
    std::vector<std::string> options = {
        "Create Room",
        "Connect Rooms",
        "View Room",
        "View Connections",
        "Exit"
    };
    
    int y = menuY + 50;
    for (size_t i = 0; i < options.size(); ++i) {
        Engine::RColor color = (i == static_cast<size_t>(selectedRoomIndex)) ? RED : BLACK;
        renderer.DrawText(options[i].c_str(), menuX + 20, y, 18, color);
        y += 30;
    }
    
    // Draw room count
    std::stringstream ss;
    ss << "Total Rooms: " << rooms.size();
    renderer.DrawText(ss.str().c_str(), menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

void RoomTestState::RenderCreateRoom() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 300;
    int menuHeight = 250;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("Create Room", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw room type options
    std::vector<std::string> options = {
        "Normal Room",
        "Treasure Room",
        "Boss Room",
        "Entrance Room",
        "Exit Room"
    };
    
    int y = menuY + 50;
    for (size_t i = 0; i < options.size(); ++i) {
        Engine::RColor color = (i == static_cast<size_t>(selectedRoomIndex)) ? RED : BLACK;
        renderer.DrawText(options[i].c_str(), menuX + 20, y, 18, color);
        y += 30;
    }
    
    // Draw instructions
    renderer.DrawText("Select room type and press SPACE", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

void RoomTestState::RenderConnectRooms() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 400;
    int menuHeight = 300;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("Connect Rooms", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw source room selection
    renderer.DrawText("Source Room:", menuX + 20, menuY + 50, 18, BLACK);
    
    if (selectedRoomIndex < static_cast<int>(rooms.size())) {
        auto room = rooms[selectedRoomIndex];
        std::stringstream ss;
        ss << "Room " << room->GetId() << " (" << RoomTypeToString(room->GetType()) << ")";
        renderer.DrawText(ss.str().c_str(), menuX + 150, menuY + 50, 18, RED);
    }
    
    // Draw target room selection
    renderer.DrawText("Target Room:", menuX + 20, menuY + 80, 18, BLACK);
    
    if (targetRoomIndex < static_cast<int>(rooms.size())) {
        auto room = rooms[targetRoomIndex];
        std::stringstream ss;
        ss << "Room " << room->GetId() << " (" << RoomTypeToString(room->GetType()) << ")";
        renderer.DrawText(ss.str().c_str(), menuX + 150, menuY + 80, 18, BLUE);
    }
    
    // Draw connection status
    if (selectedRoomIndex < static_cast<int>(rooms.size()) && 
        targetRoomIndex < static_cast<int>(rooms.size())) {
        
        auto sourceRoom = rooms[selectedRoomIndex];
        auto targetRoom = rooms[targetRoomIndex];
        
        std::string connectionStatus = sourceRoom->IsConnectedTo(targetRoom->GetId()) ?
            "Connected" : "Not Connected";
        
        renderer.DrawText("Status:", menuX + 20, menuY + 110, 18, BLACK);
        renderer.DrawText(connectionStatus.c_str(), menuX + 150, menuY + 110, 18, 
                         sourceRoom->IsConnectedTo(targetRoom->GetId()) ? GREEN : RED);
    }
    
    // Draw instructions
    renderer.DrawText("SPACE: Select Source/Target Room", menuX + 20, menuY + menuHeight - 60, 16, DARKGRAY);
    renderer.DrawText("SPACE when Target selected will toggle connection", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

void RoomTestState::RenderViewRoom() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 400;
    int menuHeight = 350;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("View Room", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw room information
    if (selectedRoomIndex < static_cast<int>(rooms.size())) {
        auto room = rooms[selectedRoomIndex];
        
        // Basic info
        std::stringstream idss;
        idss << "Room ID: " << room->GetId();
        renderer.DrawText(idss.str().c_str(), menuX + 20, menuY + 50, 18, BLACK);
        
        std::stringstream typess;
        typess << "Type: " << RoomTypeToString(room->GetType());
        renderer.DrawText(typess.str().c_str(), menuX + 20, menuY + 80, 18, BLACK);
        
        // Description
        renderer.DrawText("Description:", menuX + 20, menuY + 110, 18, BLACK);
        renderer.DrawText(room->GetDescription().c_str(), menuX + 20, menuY + 140, 14, DARKGRAY);
        
        // Position
        std::stringstream posss;
        posss << "Position: (" << room->GetPositionX() << ", " << room->GetPositionY() << ")";
        renderer.DrawText(posss.str().c_str(), menuX + 20, menuY + 180, 18, BLACK);
        
        // State
        std::stringstream statess;
        statess << "Visited: " << (room->IsVisited() ? "Yes" : "No");
        renderer.DrawText(statess.str().c_str(), menuX + 20, menuY + 210, 18, BLACK);
        
        std::stringstream clearss;
        clearss << "Cleared: " << (room->IsCleared() ? "Yes" : "No");
        renderer.DrawText(clearss.str().c_str(), menuX + 20, menuY + 240, 18, BLACK);
        
        // Connections
        std::stringstream connss;
        connss << "Connections: " << room->GetConnections().size();
        renderer.DrawText(connss.str().c_str(), menuX + 20, menuY + 270, 18, BLACK);
    }
    
    // Draw navigation instructions
    renderer.DrawText("<- -> : Navigate Rooms", menuX + 20, menuY + menuHeight - 60, 16, DARKGRAY);
    renderer.DrawText("UP: Visit Room    DOWN: Clear Room", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

void RoomTestState::RenderViewConnections() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 400;
    int menuHeight = 300;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("View Connections", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw selected room
    if (selectedRoomIndex < static_cast<int>(rooms.size())) {
        auto room = rooms[selectedRoomIndex];
        
        std::stringstream ss;
        ss << "Room " << room->GetId() << " (" << RoomTypeToString(room->GetType()) << ")";
        renderer.DrawText(ss.str().c_str(), menuX + 20, menuY + 50, 18, RED);
        
        // Draw connections
        const auto& connections = room->GetConnections();
        
        if (connections.empty()) {
            renderer.DrawText("No connections", menuX + 20, menuY + 90, 16, DARKGRAY);
        } else {
            renderer.DrawText("Connected to:", menuX + 20, menuY + 90, 18, BLACK);
            
            int y = menuY + 120;
            for (const auto& conn : connections) {
                std::stringstream connss;
                connss << "Room " << conn->GetId() << " (" << RoomTypeToString(conn->GetType()) << ")";
                renderer.DrawText(connss.str().c_str(), menuX + 40, y, 16, BLUE);
                y += 25;
            }
        }
    }
    
    // Draw navigation instructions
    renderer.DrawText("<- -> : Navigate Rooms", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

std::string RoomTestState::RoomTypeToString(RoomType type) const {
    switch (type) {
        case RoomType::NORMAL: return "Normal";
        case RoomType::TREASURE: return "Treasure";
        case RoomType::BOSS: return "Boss";
        case RoomType::ENTRANCE: return "Entrance";
        case RoomType::EXIT: return "Exit";
        default: return "Unknown";
    }
}

void RoomTestState::DrawRoomGrid() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Grid parameters
    int gridX = 500;
    int gridY = 120;
    int cellSize = 60;
    int gridWidth = 5;
    int gridHeight = 5;
    
    // Draw grid
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            int cellX = gridX + x * cellSize;
            int cellY = gridY + y * cellSize;
            
            // Draw cell
            renderer.DrawRect(cellX, cellY, cellSize, cellSize, LIGHTGRAY);
            renderer.DrawRectLines(cellX, cellY, cellSize, cellSize, DARKGRAY);
            
            // Find room at this position
            auto it = std::find_if(rooms.begin(), rooms.end(),
                                 [x, y](const std::shared_ptr<Room>& room) {
                                     return room->GetPositionX() == x && room->GetPositionY() == y;
                                 });
            
            // Draw room if found
            if (it != rooms.end()) {
                auto room = *it;
                
                // Choose color based on room type
                Engine::RColor color;
                switch (room->GetType()) {
                    case RoomType::NORMAL: color = BLUE; break;
                    case RoomType::TREASURE: color = GOLD; break;
                    case RoomType::BOSS: color = RED; break;
                    case RoomType::ENTRANCE: color = GREEN; break;
                    case RoomType::EXIT: color = PURPLE; break;
                    default: color = GRAY; break;
                }
                
                // Draw room
                renderer.DrawRect(cellX + 5, cellY + 5, cellSize - 10, cellSize - 10, color);
                
                // Draw room ID
                std::string idStr = std::to_string(room->GetId());
                renderer.DrawText(idStr.c_str(), cellX + cellSize / 2 - 5, cellY + cellSize / 2 - 8, 16, WHITE);
                
                // Draw connections
                for (const auto& conn : room->GetConnections()) {
                    int targetX = conn->GetPositionX();
                    int targetY = conn->GetPositionY();
                    
                    // Calculate center points
                    int startX = cellX + cellSize / 2;
                    int startY = cellY + cellSize / 2;
                    int endX = gridX + targetX * cellSize + cellSize / 2;
                    int endY = gridY + targetY * cellSize + cellSize / 2;
                    
                    // Draw connection line (using rectangles since DrawLine may not exist)
                    // Calculate direction vector
                    float dx = endX - startX;
                    float dy = endY - startY;
                    float length = std::sqrt(dx*dx + dy*dy);
                    float angle = std::atan2(dy, dx) * 180.0f / 3.14159f;
                    
                    // Draw a thin rectangle as a line
                    float lineThickness = 2.0f;
                    float halfThickness = lineThickness / 2.0f;
                    
                    // Draw a thin rectangle from start to end
                    for (float t = 0; t <= 1.0f; t += 0.05f) {
                        float x = startX + dx * t;
                        float y = startY + dy * t;
                        renderer.DrawRect(x - halfThickness, y - halfThickness, 
                                         lineThickness, lineThickness, BLACK);
                    }
                }
            }
        }
    }
    
    // Draw legend
    int legendX = gridX;
    int legendY = gridY + gridHeight * cellSize + 20;
    int legendWidth = gridWidth * cellSize;
    int legendHeight = 120;
    
    renderer.DrawRect(legendX, legendY, legendWidth, legendHeight, LIGHTGRAY);
    renderer.DrawRectLines(legendX, legendY, legendWidth, legendHeight, BLACK);
    
    renderer.DrawText("Legend:", legendX + 10, legendY + 10, 18, BLACK);
    
    int y = legendY + 40;
    renderer.DrawRect(legendX + 20, y, 20, 20, BLUE);
    renderer.DrawText("Normal", legendX + 50, y + 2, 16, BLACK);
    
    y += 25;
    renderer.DrawRect(legendX + 20, y, 20, 20, GOLD);
    renderer.DrawText("Treasure", legendX + 50, y + 2, 16, BLACK);
    
    y = legendY + 40;
    renderer.DrawRect(legendX + 170, y, 20, 20, RED);
    renderer.DrawText("Boss", legendX + 200, y + 2, 16, BLACK);
    
    y += 25;
    renderer.DrawRect(legendX + 170, y, 20, 20, GREEN);
    renderer.DrawText("Entrance", legendX + 200, y + 2, 16, BLACK);
    
    y += 25;
    renderer.DrawRect(legendX + 20, y, 20, 20, PURPLE);
    renderer.DrawText("Exit", legendX + 50, y + 2, 16, BLACK);
}

} // namespace Game 
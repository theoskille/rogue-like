#include "DungeonTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include "../dungeon/encounters/Encounter.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Game {

DungeonTestState::DungeonTestState()
    : uiState(UIState::MAIN_MENU),
      selectedRoomIndex(0),
      selectedMenuOption(0),
      gridOffsetX(0),
      gridOffsetY(0),
      zoomLevel(1.0f),
      showRoomIDs(true),
      showRoomTypes(true),
      showEncounters(true),
      isPaused(false) {
    
    // Set default generation parameters
    generationParams.width = 8;
    generationParams.height = 8;
    generationParams.numRooms = 15;
    generationParams.numTreasureRooms = 3;
    generationParams.hasBossRoom = true;
    generationParams.difficulty = 1;
    generationParams.loopChance = 0.2f;
}

DungeonTestState::~DungeonTestState() {
    std::cout << "DungeonTestState destroyed" << std::endl;
}

void DungeonTestState::Enter() {
    std::cout << "Entering Dungeon Test State" << std::endl;
    
    // Clear any existing dungeon
    currentDungeon.clear();
    
    // Generate a dungeon with default parameters
    GenerateDungeon();
}

void DungeonTestState::Exit() {
    std::cout << "Exiting Dungeon Test State" << std::endl;
    
    // Clear the dungeon
    currentDungeon.clear();
}

void DungeonTestState::Pause() {
    std::cout << "Pausing Dungeon Test State" << std::endl;
    isPaused = true;
}

void DungeonTestState::Resume() {
    std::cout << "Resuming Dungeon Test State" << std::endl;
    isPaused = false;
}

void DungeonTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    // Update based on current UI state
    switch (uiState) {
        case UIState::MAIN_MENU:
            UpdateMainMenu();
            break;
        case UIState::GENERATE_DUNGEON:
            UpdateGenerateDungeon();
            break;
        case UIState::VIEW_DUNGEON:
            UpdateViewDungeon();
            break;
        case UIState::VIEW_ROOM:
            UpdateViewRoom();
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

void DungeonTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawText("DUNGEON GENERATOR TEST", 50, 30, 24, BLACK);
    
    // Render based on current UI state
    switch (uiState) {
        case UIState::MAIN_MENU:
            RenderMainMenu();
            break;
        case UIState::GENERATE_DUNGEON:
            RenderGenerateDungeon();
            break;
        case UIState::VIEW_DUNGEON:
            RenderViewDungeon();
            break;
        case UIState::VIEW_ROOM:
            RenderViewRoom();
            break;
    }
    
    // Draw help text
    renderer.DrawText("ESC: Back/Exit", 50, 700, 16, DARKGRAY);
}

void DungeonTestState::UpdateMainMenu() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Handle menu navigation
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedMenuOption = (selectedMenuOption - 1 + 3) % 3;
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedMenuOption = (selectedMenuOption + 1) % 3;
    }
    
    // Handle menu selection
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        switch (selectedMenuOption) {
            case 0: // Generate Dungeon
                uiState = UIState::GENERATE_DUNGEON;
                break;
            case 1: // View Dungeon
                if (!currentDungeon.empty()) {
                    uiState = UIState::VIEW_DUNGEON;
                }
                break;
            case 2: // Exit
                Engine::StateManager::GetInstance().PopState();
                break;
        }
    }
}

void DungeonTestState::UpdateGenerateDungeon() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Navigate through generation parameters
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedMenuOption = (selectedMenuOption - 1 + 7) % 7;
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedMenuOption = (selectedMenuOption + 1) % 7;
    }
    
    // Modify parameter values
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
        switch (selectedMenuOption) {
            case 0: // Width
                if (generationParams.width > 3) {
                    generationParams.width--;
                }
                break;
            case 1: // Height
                if (generationParams.height > 3) {
                    generationParams.height--;
                }
                break;
            case 2: // Number of rooms
                if (generationParams.numRooms > 5) {
                    generationParams.numRooms--;
                }
                break;
            case 3: // Treasure rooms
                if (generationParams.numTreasureRooms > 0) {
                    generationParams.numTreasureRooms--;
                }
                break;
            case 4: // Boss room
                generationParams.hasBossRoom = !generationParams.hasBossRoom;
                break;
            case 5: // Difficulty
                if (generationParams.difficulty > 1) {
                    generationParams.difficulty--;
                }
                break;
            case 6: // Loop chance
                if (generationParams.loopChance >= 0.1f) {
                    generationParams.loopChance -= 0.1f;
                }
                break;
        }
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
        switch (selectedMenuOption) {
            case 0: // Width
                if (generationParams.width < 15) {
                    generationParams.width++;
                }
                break;
            case 1: // Height
                if (generationParams.height < 15) {
                    generationParams.height++;
                }
                break;
            case 2: // Number of rooms
                if (generationParams.numRooms < generationParams.width * generationParams.height) {
                    generationParams.numRooms++;
                }
                break;
            case 3: // Treasure rooms
                if (generationParams.numTreasureRooms < generationParams.numRooms / 3) {
                    generationParams.numTreasureRooms++;
                }
                break;
            case 4: // Boss room
                generationParams.hasBossRoom = !generationParams.hasBossRoom;
                break;
            case 5: // Difficulty
                if (generationParams.difficulty < 10) {
                    generationParams.difficulty++;
                }
                break;
            case 6: // Loop chance
                if (generationParams.loopChance <= 0.9f) {
                    generationParams.loopChance += 0.1f;
                }
                break;
        }
    }
    
    // Generate dungeon with current parameters
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        GenerateDungeon();
    }
}

void DungeonTestState::UpdateViewDungeon() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Pan the view
    if (input.IsActionPressed(Engine::InputAction::MOVE_UP)) {
        gridOffsetY += 10.0f / zoomLevel;
    }
    if (input.IsActionPressed(Engine::InputAction::MOVE_DOWN)) {
        gridOffsetY -= 10.0f / zoomLevel;
    }
    if (input.IsActionPressed(Engine::InputAction::MOVE_LEFT)) {
        gridOffsetX += 10.0f / zoomLevel;
    }
    if (input.IsActionPressed(Engine::InputAction::MOVE_RIGHT)) {
        gridOffsetX -= 10.0f / zoomLevel;
    }
    
    // Toggle display options
    if (input.IsActionJustPressed(Engine::InputAction::ATTACK)) {
        showRoomIDs = !showRoomIDs;
    }
    
    if (input.IsActionJustPressed(Engine::InputAction::USE_ITEM)) {
        showRoomTypes = !showRoomTypes;
    }
    
    if (input.IsActionJustPressed(Engine::InputAction::MENU)) {
        showEncounters = !showEncounters;
    }
    
    // Zoom in/out
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        zoomLevel = std::min(zoomLevel + 0.25f, 2.0f);
    }
    
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        zoomLevel = std::max(zoomLevel - 0.25f, 0.5f);
        return; // Don't go back to main menu when zooming out
    }
    
    // Simple room cycling with number keys is not available
    // We'll just use a simpler approach for testing
    if (input.IsActionJustPressed(Engine::InputAction::ATTACK) && 
        input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        // Cycle to next room when pressing attack+confirm together
        if (!currentDungeon.empty()) {
            selectedRoomIndex = (selectedRoomIndex + 1) % currentDungeon.size();
            uiState = UIState::VIEW_ROOM;
        }
    }
}

void DungeonTestState::UpdateViewRoom() {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Find the selected room
    auto selectedRoom = std::find_if(currentDungeon.begin(), currentDungeon.end(),
                                  [this](const std::shared_ptr<Room>& room) {
                                      return room->GetId() == selectedRoomIndex;
                                  });
    
    if (selectedRoom == currentDungeon.end()) {
        // Room not found, go back to dungeon view
        uiState = UIState::VIEW_DUNGEON;
        return;
    }
    
    // Navigate to connected rooms
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP) || 
        input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN) || 
        input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT) || 
        input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
        
        // Get the connections
        const auto& connections = (*selectedRoom)->GetConnections();
        
        if (!connections.empty()) {
            // Find the closest room in the direction pressed
            auto currentRoom = *selectedRoom;
            int currentX = currentRoom->GetPositionX();
            int currentY = currentRoom->GetPositionY();
            
            std::shared_ptr<Room> closestRoom = nullptr;
            float closestDistance = std::numeric_limits<float>::max();
            
            for (const auto& conn : connections) {
                int connX = conn->GetPositionX();
                int connY = conn->GetPositionY();
                
                bool isInDirection = false;
                
                if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
                    isInDirection = connY < currentY;
                } else if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
                    isInDirection = connY > currentY;
                } else if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
                    isInDirection = connX < currentX;
                } else if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
                    isInDirection = connX > currentX;
                }
                
                if (isInDirection) {
                    float distance = std::sqrt(
                        std::pow(connX - currentX, 2) + 
                        std::pow(connY - currentY, 2)
                    );
                    
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestRoom = conn;
                    }
                }
            }
            
            // If we found a room in that direction, select it
            if (closestRoom) {
                selectedRoomIndex = closestRoom->GetId();
            }
        }
    }
    
    // Visit/clear room
    if (input.IsActionJustPressed(Engine::InputAction::ATTACK)) {
        (*selectedRoom)->Visit();
    }
    
    if (input.IsActionJustPressed(Engine::InputAction::USE_ITEM)) {
        (*selectedRoom)->Clear();
    }
    
    // Return to dungeon view
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
        uiState = UIState::VIEW_DUNGEON;
    }
}

void DungeonTestState::GenerateDungeon() {
    // Use the DungeonGenerator to create a new dungeon
    DungeonGenerator& generator = DungeonGenerator::GetInstance();
    
    // Generate the dungeon
    currentDungeon = generator.GenerateFloor(generationParams);
    
    // If we successfully generated a dungeon
    if (!currentDungeon.empty()) {
        std::cout << "Generated a dungeon with " << currentDungeon.size() << " rooms" << std::endl;
        
        // Center the view on the dungeon
        gridOffsetX = 0;
        gridOffsetY = 0;
        zoomLevel = 1.0f;
        
        // Set the initial selected room to the entrance
        auto entranceRoom = generator.GetEntranceRoom();
        if (entranceRoom) {
            selectedRoomIndex = entranceRoom->GetId();
        } else {
            selectedRoomIndex = 0;
        }
        
        // Switch to the view dungeon state
        uiState = UIState::VIEW_DUNGEON;
    } else {
        std::cerr << "Failed to generate dungeon!" << std::endl;
    }
}

std::string DungeonTestState::RoomTypeToString(RoomType type) const {
    switch (type) {
        case RoomType::NORMAL: return "Normal";
        case RoomType::TREASURE: return "Treasure";
        case RoomType::BOSS: return "Boss";
        case RoomType::ENTRANCE: return "Entrance";
        case RoomType::EXIT: return "Exit";
        default: return "Unknown";
    }
}

Color DungeonTestState::GetRoomColor(RoomType type) const {
    switch (type) {
        case RoomType::NORMAL: return BLUE;
        case RoomType::TREASURE: return GOLD;
        case RoomType::BOSS: return RED;
        case RoomType::ENTRANCE: return GREEN;
        case RoomType::EXIT: return PURPLE;
        default: return GRAY;
    }
}

void DungeonTestState::RenderMainMenu() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 300;
    int menuHeight = 200;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("Main Menu", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw menu options
    std::vector<std::string> options = {
        "Generate Dungeon",
        "View Current Dungeon",
        "Exit"
    };
    
    int y = menuY + 50;
    for (size_t i = 0; i < options.size(); ++i) {
        Color color = (i == static_cast<size_t>(selectedMenuOption)) ? RED : BLACK;
        renderer.DrawText(options[i].c_str(), menuX + 20, y, 18, color);
        y += 30;
    }
    
    // Draw current dungeon info
    if (!currentDungeon.empty()) {
        std::stringstream ss;
        ss << "Current Dungeon: " << currentDungeon.size() << " rooms";
        renderer.DrawText(ss.str().c_str(), menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
    } else {
        renderer.DrawText("No dungeon generated yet", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
    }
    
    // Draw mini dungeon preview if available
    if (!currentDungeon.empty()) {
        int previewX = menuX + menuWidth + 50;
        int previewY = menuY;
        int previewSize = 250;
        
        renderer.DrawRect(previewX, previewY, previewSize, previewSize, LIGHTGRAY);
        renderer.DrawRectLines(previewX, previewY, previewSize, previewSize, BLACK);
        
        // Draw a scaled-down version of the dungeon
        for (const auto& room : currentDungeon) {
            int x = room->GetPositionX();
            int y = room->GetPositionY();
            
            // Scale to fit in preview
            int roomX = previewX + (x * previewSize) / generationParams.width;
            int roomY = previewY + (y * previewSize) / generationParams.height;
            int roomSize = previewSize / std::max(generationParams.width, generationParams.height);
            
            // Draw room
            Color roomColor = GetRoomColor(room->GetType());
            renderer.DrawRect(roomX, roomY, roomSize, roomSize, roomColor);
        }
    }
}

void DungeonTestState::RenderGenerateDungeon() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw menu background
    int menuX = 50;
    int menuY = 120;
    int menuWidth = 400;
    int menuHeight = 350;
    
    renderer.DrawRect(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    renderer.DrawRectLines(menuX, menuY, menuWidth, menuHeight, BLACK);
    
    // Draw menu title
    renderer.DrawText("Generate Dungeon", menuX + 10, menuY + 10, 20, BLACK);
    
    // Draw parameters
    int y = menuY + 50;
    int spacing = 30;
    
    // Parameter labels and values
    std::vector<std::pair<std::string, std::string>> params = {
        {"Width:", std::to_string(generationParams.width)},
        {"Height:", std::to_string(generationParams.height)},
        {"Number of Rooms:", std::to_string(generationParams.numRooms)},
        {"Treasure Rooms:", std::to_string(generationParams.numTreasureRooms)},
        {"Boss Room:", generationParams.hasBossRoom ? "Yes" : "No"},
        {"Difficulty:", std::to_string(generationParams.difficulty)},
        {"Loop Chance:", std::to_string(static_cast<int>(generationParams.loopChance * 100)) + "%"}
    };
    
    for (size_t i = 0; i < params.size(); ++i) {
        Color color = (i == static_cast<size_t>(selectedMenuOption)) ? RED : BLACK;
        renderer.DrawText(params[i].first.c_str(), menuX + 20, y, 18, color);
        renderer.DrawText(params[i].second.c_str(), menuX + 250, y, 18, color);
        y += spacing;
    }
    
    // Draw instructions
    renderer.DrawText("Arrow Keys: Navigate & Change Values", menuX + 20, menuY + menuHeight - 60, 16, DARKGRAY);
    renderer.DrawText("SPACE: Generate Dungeon", menuX + 20, menuY + menuHeight - 30, 16, DARKGRAY);
}

void DungeonTestState::RenderViewDungeon() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw the dungeon grid
    DrawDungeonGrid(gridOffsetX, gridOffsetY, 60.0f * zoomLevel);
    
    // Draw controls help
    int helpX = 50;
    int helpY = 550;
    int helpWidth = 400;
    int helpHeight = 140;
    
    renderer.DrawRect(helpX, helpY, helpWidth, helpHeight, LIGHTGRAY);
    renderer.DrawRectLines(helpX, helpY, helpWidth, helpHeight, BLACK);
    
    renderer.DrawText("Controls:", helpX + 10, helpY + 10, 18, BLACK);
    renderer.DrawText("Arrow Keys: Pan View", helpX + 20, helpY + 40, 16, DARKGRAY);
    renderer.DrawText("SPACE: Zoom In", helpX + 20, helpY + 60, 16, DARKGRAY);
    renderer.DrawText("ESC: Zoom Out", helpX + 20, helpY + 80, 16, DARKGRAY);
    renderer.DrawText("A: Toggle Room IDs", helpX + 20, helpY + 100, 16, DARKGRAY);
    renderer.DrawText("I: Toggle Room Types", helpX + 20, helpY + 120, 16, DARKGRAY);
    
    // Draw legend
    int legendX = helpX + helpWidth + 20;
    int legendY = helpY;
    int legendWidth = 200;
    int legendHeight = helpHeight;
    
    renderer.DrawRect(legendX, legendY, legendWidth, legendHeight, LIGHTGRAY);
    renderer.DrawRectLines(legendX, legendY, legendWidth, legendHeight, BLACK);
    
    renderer.DrawText("Legend:", legendX + 10, legendY + 10, 18, BLACK);
    
    int ly = legendY + 40;
    int spacing = 20;
    
    renderer.DrawRect(legendX + 20, ly, 15, 15, GetRoomColor(RoomType::NORMAL));
    renderer.DrawText("Normal", legendX + 45, ly, 14, BLACK);
    ly += spacing;
    
    renderer.DrawRect(legendX + 20, ly, 15, 15, GetRoomColor(RoomType::TREASURE));
    renderer.DrawText("Treasure", legendX + 45, ly, 14, BLACK);
    ly += spacing;
    
    renderer.DrawRect(legendX + 20, ly, 15, 15, GetRoomColor(RoomType::BOSS));
    renderer.DrawText("Boss", legendX + 45, ly, 14, BLACK);
    ly += spacing;
    
    renderer.DrawRect(legendX + 20, ly, 15, 15, GetRoomColor(RoomType::ENTRANCE));
    renderer.DrawText("Entrance", legendX + 45, ly, 14, BLACK);
    ly += spacing;
    
    renderer.DrawRect(legendX + 20, ly, 15, 15, GetRoomColor(RoomType::EXIT));
    renderer.DrawText("Exit", legendX + 45, ly, 14, BLACK);
}

void DungeonTestState::RenderViewRoom() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Find the selected room
    auto selectedRoom = std::find_if(currentDungeon.begin(), currentDungeon.end(),
                                  [this](const std::shared_ptr<Room>& room) {
                                      return room->GetId() == selectedRoomIndex;
                                  });
    
    if (selectedRoom == currentDungeon.end()) {
        // Room not found, go back to dungeon view
        uiState = UIState::VIEW_DUNGEON;
        return;
    }
    
    // Draw room details panel
    int panelX = 50;
    int panelY = 120;
    int panelWidth = 400;
    int panelHeight = 350;
    
    renderer.DrawRect(panelX, panelY, panelWidth, panelHeight, LIGHTGRAY);
    renderer.DrawRectLines(panelX, panelY, panelWidth, panelHeight, BLACK);
    
    // Draw room title
    std::stringstream titleSs;
    titleSs << "Room " << (*selectedRoom)->GetId() << " (" << RoomTypeToString((*selectedRoom)->GetType()) << ")";
    renderer.DrawText(titleSs.str().c_str(), panelX + 10, panelY + 10, 20, BLACK);
    
    // Draw room details
    int y = panelY + 50;
    int spacing = 25;
    
    // Position
    std::stringstream posSs;
    posSs << "Position: (" << (*selectedRoom)->GetPositionX() << ", " << (*selectedRoom)->GetPositionY() << ")";
    renderer.DrawText(posSs.str().c_str(), panelX + 20, y, 18, BLACK);
    y += spacing;
    
    // Description
    renderer.DrawText("Description:", panelX + 20, y, 18, BLACK);
    y += spacing;
    
    // Wrap description text
    std::string desc = (*selectedRoom)->GetDescription();
    int maxWidth = panelWidth - 40;
    int fontSize = 16;
    int charWidth = 10; // Approximate width of each character
    int charsPerLine = maxWidth / charWidth;
    
    for (size_t i = 0; i < desc.length(); i += charsPerLine) {
        std::string line = desc.substr(i, charsPerLine);
        renderer.DrawText(line.c_str(), panelX + 20, y, fontSize, DARKGRAY);
        y += spacing;
    }
    
    // Status
    std::stringstream statSs;
    statSs << "Visited: " << ((*selectedRoom)->IsVisited() ? "Yes" : "No") 
          << "   Cleared: " << ((*selectedRoom)->IsCleared() ? "Yes" : "No");
    renderer.DrawText(statSs.str().c_str(), panelX + 20, y, 18, BLACK);
    y += spacing * 2;
    
    // Connections
    renderer.DrawText("Connections:", panelX + 20, y, 18, BLACK);
    y += spacing;
    
    const auto& connections = (*selectedRoom)->GetConnections();
    if (connections.empty()) {
        renderer.DrawText("No connections", panelX + 40, y, 16, DARKGRAY);
    } else {
        for (const auto& conn : connections) {
            std::stringstream connSs;
            connSs << "Room " << conn->GetId() << " (" << RoomTypeToString(conn->GetType()) << ")";
            renderer.DrawText(connSs.str().c_str(), panelX + 40, y, 16, DARKGRAY);
            y += spacing;
        }
    }
    
    // Draw encounter info if present
    auto encounter = (*selectedRoom)->GetEncounter();
    if (encounter) {
        y += spacing;
        std::stringstream encSs;
        encSs << "Encounter: " << encounter->GetName();
        renderer.DrawText(encSs.str().c_str(), panelX + 20, y, 18, BLACK);
        y += spacing;
        
        renderer.DrawText(encounter->GetDescription().c_str(), panelX + 40, y, 16, DARKGRAY);
    }
    
    // Draw help text
    renderer.DrawText("Arrow Keys: Navigate to Connected Rooms", panelX + 20, panelY + panelHeight - 60, 16, DARKGRAY);
    renderer.DrawText("A: Visit Room    I: Clear Room", panelX + 20, panelY + panelHeight - 30, 16, DARKGRAY);
    
    // Draw a mini visualization of the room and its connections
    int vizX = panelX + panelWidth + 50;
    int vizY = panelY;
    int vizWidth = 300;
    int vizHeight = 300;
    
    renderer.DrawRect(vizX, vizY, vizWidth, vizHeight, LIGHTGRAY);
    renderer.DrawRectLines(vizX, vizY, vizWidth, vizHeight, BLACK);
    
    // Center the room in the visualization
    int centerX = vizX + vizWidth / 2;
    int centerY = vizY + vizHeight / 2;
    int roomSize = 40;
    
    // Draw the selected room in the center
    Color roomColor = GetRoomColor((*selectedRoom)->GetType());
    renderer.DrawRect(centerX - roomSize/2, centerY - roomSize/2, roomSize, roomSize, roomColor);
    
    // Draw the room ID
    std::string idStr = std::to_string((*selectedRoom)->GetId());
    renderer.DrawText(idStr.c_str(), centerX - 5, centerY - 8, 16, WHITE);
    
    // Draw connections radiating outward
    for (const auto& conn : connections) {
        // Calculate angle based on connection position relative to selected room
        int dx = conn->GetPositionX() - (*selectedRoom)->GetPositionX();
        int dy = conn->GetPositionY() - (*selectedRoom)->GetPositionY();
        
        float angle = atan2f(dy, dx);
        float distance = 100.0f;
        
        // Calculate connection room position
        int connX = centerX + static_cast<int>(cos(angle) * distance);
        int connY = centerY + static_cast<int>(sin(angle) * distance);
        
        // Draw line from center to connection (using small rectangles as a line)
        float lineLength = std::sqrt(std::pow(connX - centerX, 2) + std::pow(connY - centerY, 2));
        int numSegments = static_cast<int>(lineLength / 5); // Draw a segment every 5 pixels
        
        for (int i = 0; i < numSegments; i++) {
            float t = static_cast<float>(i) / numSegments;
            float x = centerX + t * (connX - centerX);
            float y = centerY + t * (connY - centerY);
            
            renderer.DrawRect(x - 1, y - 1, 2, 2, BLACK);
        }
        
        // Draw the connected room
        Color connColor = GetRoomColor(conn->GetType());
        renderer.DrawRect(connX - roomSize/2, connY - roomSize/2, roomSize, roomSize, connColor);
        
        // Draw the room ID
        std::string connIdStr = std::to_string(conn->GetId());
        renderer.DrawText(connIdStr.c_str(), connX - 5, connY - 8, 16, WHITE);
    }
}

void DungeonTestState::DrawDungeonGrid(float offsetX, float offsetY, float cellSize) {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Grid parameters
    float gridX = 500.0f + offsetX * zoomLevel;
    float gridY = 120.0f + offsetY * zoomLevel;
    
    // Draw grid background
    int gridWidth = static_cast<int>(generationParams.width * cellSize);
    int gridHeight = static_cast<int>(generationParams.height * cellSize);
    renderer.DrawRect(gridX, gridY, gridWidth, gridHeight, RAYWHITE);
    renderer.DrawRectLines(gridX, gridY, gridWidth, gridHeight, DARKGRAY);
    
    // Draw grid lines
    for (int x = 1; x < generationParams.width; x++) {
        float lineX = gridX + x * cellSize;
        // Draw a vertical line as a series of small rectangles
        for (int y = 0; y < gridHeight; y += 2) {
            renderer.DrawRect(lineX, gridY + y, 1, 1, LIGHTGRAY);
        }
    }
    
    for (int y = 1; y < generationParams.height; y++) {
        float lineY = gridY + y * cellSize;
        // Draw a horizontal line as a series of small rectangles
        for (int x = 0; x < gridWidth; x += 2) {
            renderer.DrawRect(gridX + x, lineY, 1, 1, LIGHTGRAY);
        }
    }
    
    // Draw connections between rooms first (so they appear behind rooms)
    for (const auto& room : currentDungeon) {
        int x = room->GetPositionX();
        int y = room->GetPositionY();
        
        float roomCenterX = gridX + (x + 0.5f) * cellSize;
        float roomCenterY = gridY + (y + 0.5f) * cellSize;
        
        // Draw connections
        for (const auto& conn : room->GetConnections()) {
            int connX = conn->GetPositionX();
            int connY = conn->GetPositionY();
            
            float connCenterX = gridX + (connX + 0.5f) * cellSize;
            float connCenterY = gridY + (connY + 0.5f) * cellSize;
            
            // Draw a line between centers using small rectangles
            float dx = connCenterX - roomCenterX;
            float dy = connCenterY - roomCenterY;
            float length = std::sqrt(dx*dx + dy*dy);
            int numSegments = static_cast<int>(length / 2); // Draw a segment every 2 pixels
            
            for (int i = 0; i < numSegments; i++) {
                float t = static_cast<float>(i) / numSegments;
                float x = roomCenterX + t * dx;
                float y = roomCenterY + t * dy;
                
                renderer.DrawRect(x - 1, y - 1, 2, 2, DARKGRAY);
            }
        }
    }
    
    // Draw rooms
    for (const auto& room : currentDungeon) {
        int x = room->GetPositionX();
        int y = room->GetPositionY();
        
        float roomX = gridX + x * cellSize;
        float roomY = gridY + y * cellSize;
        
        // Draw room
        Color roomColor = GetRoomColor(room->GetType());
        
        // Make selected room brighter
        if (room->GetId() == selectedRoomIndex) {
            roomColor = WHITE;
            renderer.DrawRectLines(roomX - 2, roomY - 2, cellSize + 4, cellSize + 4, RED);
        }
        
        // Draw room with slightly smaller size to show grid
        float padding = cellSize * 0.1f;
        renderer.DrawRect(roomX + padding, roomY + padding, 
                        cellSize - 2*padding, cellSize - 2*padding, roomColor);
        
        // Draw visited/cleared indicators
        if (room->IsVisited()) {
            renderer.DrawRect(roomX + padding, roomY + padding, 10, 10, GREEN);
        }
        
        if (room->IsCleared()) {
            renderer.DrawRect(roomX + padding + 15, roomY + padding, 10, 10, PURPLE);
        }
        
        // Draw room ID if enabled
        if (showRoomIDs) {
            std::string idStr = std::to_string(room->GetId());
            renderer.DrawText(idStr.c_str(), roomX + cellSize/2 - 5, roomY + cellSize/2 - 8, 16, BLACK);
        }
        
        // Draw room type if enabled
        if (showRoomTypes) {
            std::string typeStr = RoomTypeToString(room->GetType()).substr(0, 1); // First letter only
            renderer.DrawText(typeStr.c_str(), roomX + cellSize - 15, roomY + 5, 16, BLACK);
        }
        
        // Draw encounter indicator if enabled
        if (showEncounters && room->GetEncounter()) {
            auto encounter = room->GetEncounter();
            std::string encTypeStr;
            
            if (encounter->GetType() == EncounterType::COMBAT) {
                encTypeStr = "C";
            } else if (encounter->GetType() == EncounterType::TREASURE) {
                encTypeStr = "T";
            } else {
                encTypeStr = "?";
            }
            
            renderer.DrawText(encTypeStr.c_str(), roomX + 5, roomY + cellSize - 20, 16, BLACK);
        }
    }
    
    // Draw grid coordinates
    renderer.DrawText("Dungeon Grid", gridX, gridY - 30, 20, BLACK);
    
    std::stringstream coordSs;
    coordSs << "Offset: (" << offsetX << ", " << offsetY << ") Zoom: " << std::fixed << std::setprecision(2) << zoomLevel;
    renderer.DrawText(coordSs.str().c_str(), gridX, gridY - 30 + 20, 16, DARKGRAY);
}

} // namespace Game 
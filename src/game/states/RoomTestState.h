#pragma once

#include "../../engine/core/StateManager.h"
#include "../dungeon/Room.h"
#include <memory>
#include <vector>
#include <string>

namespace Game {

// Test state for the Room class
class RoomTestState : public Engine::GameState {
public:
    RoomTestState();
    ~RoomTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
    void Pause() override;
    void Resume() override;
    
    // Get state name for debugging
    std::string GetStateName() const override { return "RoomTestState"; }
    
private:
    // Test rooms
    std::vector<std::shared_ptr<Room>> rooms;
    
    // UI state
    enum class UIState {
        MAIN_MENU,
        CREATE_ROOM,
        CONNECT_ROOMS,
        VIEW_ROOM,
        VIEW_CONNECTIONS
    };
    UIState uiState;
    
    // Selection indices
    int selectedRoomIndex;
    int targetRoomIndex;
    int selectedPropertyIndex;
    
    // Input fields
    std::string inputText;
    bool isEditing;
    
    // State flags
    bool isPaused;
    
    // Helper methods
    void CreateTestRoom(RoomType type);
    void ConnectRooms(int roomId1, int roomId2);
    void RemoveConnection(int roomId1, int roomId2);
    void UpdateMainMenu();
    void UpdateCreateRoom();
    void UpdateConnectRooms();
    void UpdateViewRoom();
    void UpdateViewConnections();
    
    // Rendering helpers
    void RenderMainMenu();
    void RenderCreateRoom();
    void RenderConnectRooms();
    void RenderViewRoom();
    void RenderViewConnections();
    std::string RoomTypeToString(RoomType type) const;
    void DrawRoomGrid();
};

} // namespace Game 
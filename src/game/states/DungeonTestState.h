#pragma once

#include "../../engine/core/StateManager.h"
#include "../dungeon/DungeonGenerator.h"
#include "../../engine/rendering/Renderer.h"
#include <memory>
#include <vector>
#include <string>

namespace Game {

// Test state for the DungeonGenerator class
class DungeonTestState : public Engine::GameState {
public:
    DungeonTestState();
    ~DungeonTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Exit() override;
    void Pause() override;
    void Resume() override;
    
    // Get state name for debugging
    std::string GetStateName() const override { return "DungeonTestState"; }
    
private:
    // Current dungeon state
    std::vector<std::shared_ptr<Room>> currentDungeon;
    DungeonGenerationParams generationParams;
    
    // UI state
    enum class UIState {
        MAIN_MENU,
        GENERATE_DUNGEON,
        VIEW_DUNGEON,
        VIEW_ROOM
    };
    UIState uiState;
    
    // Selection and navigation
    int selectedRoomIndex;
    int selectedMenuOption;
    int gridOffsetX;
    int gridOffsetY;
    float zoomLevel;
    
    // Visualization settings
    bool showRoomIDs;
    bool showRoomTypes;
    bool showEncounters;
    
    // State flags
    bool isPaused;
    
    // Helper methods
    void GenerateDungeon();
    
    // UI update methods
    void UpdateMainMenu();
    void UpdateGenerateDungeon();
    void UpdateViewDungeon();
    void UpdateViewRoom();
    
    // Rendering helpers
    void RenderMainMenu();
    void RenderGenerateDungeon();
    void RenderViewDungeon();
    void RenderViewRoom();
    void DrawDungeonGrid(float offsetX, float offsetY, float cellSize);
    std::string RoomTypeToString(RoomType type) const;
    Color GetRoomColor(RoomType type) const;
};

} // namespace Game 
#pragma once

#include "../../engine/core/StateManager.h"
#include "../../engine/rendering/Renderer.h"
#include "../../data/schemas/ItemData.h"
#include <memory>
#include <vector>

namespace Game {

class DataTestState : public Engine::GameState {
public:
    DataTestState();
    ~DataTestState() override;
    
    // GameState lifecycle methods
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    void Pause() override;
    void Resume() override;
    
    // Get the state name
    std::string GetStateName() const override { return "DataTestState"; }
    
private:
    // Item data loader
    ItemLoader itemLoader;
    
    // Selected item index
    int selectedItem = 0;
    
    // List of item keys for navigation
    std::vector<std::string> itemKeys;
    
    // Display a specific item
    void RenderItemDetails(const std::string& key, int x, int y);
    
    // Load item data
    bool LoadItemData();
    
    bool isPaused = false;
};

} // namespace Game 
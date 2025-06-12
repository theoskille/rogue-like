#include "DataTestState.h"
#include "../../engine/input/InputHandler.h"
#include <iostream>

namespace Game {

DataTestState::DataTestState() {
    std::cout << "DataTestState created" << std::endl;
}

DataTestState::~DataTestState() {
    std::cout << "DataTestState destroyed" << std::endl;
}

void DataTestState::Enter() {
    std::cout << "Entering Data Test State" << std::endl;
    isPaused = false;
    
    // Print debug info
    std::cout << "Loading item data..." << std::endl;
    
    // Load item data
    if (!LoadItemData()) {
        std::cerr << "Failed to load item data!" << std::endl;
    } else {
        std::cout << "Successfully loaded " << itemKeys.size() << " items" << std::endl;
    }
    
    // Initialize selected item
    selectedItem = 0;
}

void DataTestState::Exit() {
    std::cout << "Exiting Data Test State" << std::endl;
}

void DataTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Navigate item list
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedItem = (selectedItem > 0) ? selectedItem - 1 : itemKeys.size() - 1;
    }
    
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedItem = (selectedItem < static_cast<int>(itemKeys.size()) - 1) ? selectedItem + 1 : 0;
    }
    
    // Exit on cancel
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void DataTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear the background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("DATA LOADING TEST", 
                             renderer.GetScreenWidth() / 2, 
                             30, 
                             30, 
                             BLACK);
    
    // Draw item list
    renderer.DrawText("Item List:", 50, 100, 20, BLACK);
    
    if (itemKeys.empty()) {
        renderer.DrawText("No items loaded!", 70, 130, 16, RED);
    } else {
        int yPos = 130;
        for (size_t i = 0; i < itemKeys.size(); i++) {
            Engine::RColor color = (static_cast<int>(i) == selectedItem) ? RED : DARKGRAY;
            renderer.DrawText(itemKeys[i].c_str(), 70, yPos, 16, color);
            yPos += 30;
        }
        
        // Draw selected item details if available
        if (!itemKeys.empty() && selectedItem >= 0 && selectedItem < static_cast<int>(itemKeys.size())) {
            RenderItemDetails(itemKeys[selectedItem], renderer.GetScreenWidth() / 2, 130);
        }
    }
    
    // Draw instructions
    renderer.DrawTextCentered("Use UP/DOWN to navigate, ESC to exit", 
                             renderer.GetScreenWidth() / 2, 
                             renderer.GetScreenHeight() - 30, 
                             16, 
                             DARKGRAY);
    
    // Draw pause message if paused
    if (isPaused) {
        renderer.DrawTextCentered("PAUSED", 
                                 renderer.GetScreenWidth() / 2, 
                                 renderer.GetScreenHeight() / 2, 
                                 40, 
                                 RED);
    }
}

void DataTestState::Pause() {
    std::cout << "Pausing Data Test State" << std::endl;
    isPaused = true;
}

void DataTestState::Resume() {
    std::cout << "Resuming Data Test State" << std::endl;
    isPaused = false;
}

bool DataTestState::LoadItemData() {
    // Try different paths
    const std::string paths[] = {
        "src/data/schemas/items.json",
        "./src/data/schemas/items.json",
        "../src/data/schemas/items.json",
        "../../src/data/schemas/items.json",
        "/Users/theoskille/Desktop/projects/rogue-like/src/data/schemas/items.json"
    };
    
    for (const auto& path : paths) {
        std::cout << "Trying to load from: " << path << std::endl;
        if (itemLoader.LoadFromFile(path)) {
            std::cout << "Successfully loaded from: " << path << std::endl;
            
            // Get item keys
            itemKeys.clear();
            for (const auto& [key, item] : itemLoader.GetItems()) {
                itemKeys.push_back(key);
                std::cout << "Loaded item: " << key << " - " << item->GetName() << std::endl;
            }
            
            return !itemKeys.empty();
        }
    }
    
    return false;
}

void DataTestState::RenderItemDetails(const std::string& key, int x, int y) {
    auto item = itemLoader.GetItem(key);
    if (!item) return;
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Item name as title
    renderer.DrawTextCentered(item->GetName().c_str(), x, y, 24, MAROON);
    
    // Draw details
    int lineY = y + 40;
    renderer.DrawText("Description:", x, lineY, 16, BLACK);
    lineY += 25;
    renderer.DrawText(item->GetDescription().c_str(), x, lineY, 14, DARKGRAY);
    lineY += 30;
    
    // Basic stats
    renderer.DrawText(("Value: " + std::to_string(item->GetValue()) + " gold").c_str(), x, lineY, 16, BLACK);
    lineY += 25;
    renderer.DrawText(("Weight: " + std::to_string(item->GetWeight()) + " units").c_str(), x, lineY, 16, BLACK);
    lineY += 25;
    
    // Equipment details if applicable
    if (item->IsEquippable()) {
        renderer.DrawText(("Equip Slot: " + item->GetEquipSlot()).c_str(), x, lineY, 16, BLACK);
        lineY += 25;
        
        // Show stat bonuses
        renderer.DrawText("Stat Bonuses:", x, lineY, 16, BLACK);
        lineY += 25;
        
        // List each non-zero bonus
        if (item->GetStrBonus() != 0) {
            renderer.DrawText(("STR: +" + std::to_string(item->GetStrBonus())).c_str(), x + 20, lineY, 14, DARKGREEN);
            lineY += 20;
        }
        if (item->GetDexBonus() != 0) {
            renderer.DrawText(("DEX: +" + std::to_string(item->GetDexBonus())).c_str(), x + 20, lineY, 14, DARKGREEN);
            lineY += 20;
        }
        if (item->GetConBonus() != 0) {
            renderer.DrawText(("CON: +" + std::to_string(item->GetConBonus())).c_str(), x + 20, lineY, 14, DARKGREEN);
            lineY += 20;
        }
        if (item->GetIntBonus() != 0) {
            renderer.DrawText(("INT: +" + std::to_string(item->GetIntBonus())).c_str(), x + 20, lineY, 14, DARKGREEN);
            lineY += 20;
        }
        if (item->GetSpdBonus() != 0) {
            renderer.DrawText(("SPD: +" + std::to_string(item->GetSpdBonus())).c_str(), x + 20, lineY, 14, DARKGREEN);
            lineY += 20;
        }
    } else {
        renderer.DrawText("Not equippable", x, lineY, 16, DARKGRAY);
    }
}

} // namespace Game 
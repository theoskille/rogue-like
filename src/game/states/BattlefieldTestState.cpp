#include "BattlefieldTestState.h"
#include "../../engine/input/InputHandler.h"
#include "../entities/components/PositionComponent.h"
#include "../entities/components/StatsComponent.h"
#include <iostream>

namespace Game {

BattlefieldTestState::BattlefieldTestState()
    : selectedEntityIndex(0), targetPosition(-1) {
    std::cout << "BattlefieldTestState created" << std::endl;
}

BattlefieldTestState::~BattlefieldTestState() {
    std::cout << "BattlefieldTestState destroyed" << std::endl;
}

void BattlefieldTestState::Enter() {
    std::cout << "Entering Battlefield Test State" << std::endl;
    
    // Create test entities and set up battlefield
    CreateTestEntities();
}

void BattlefieldTestState::Exit() {
    std::cout << "Exiting Battlefield Test State" << std::endl;
    
    // Clear battlefield
    battlefield.Clear();
    
    // Clear entity vectors
    playerEntities.clear();
    enemyEntities.clear();
}

void BattlefieldTestState::Update(float deltaTime) {
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Switch between entities with up/down
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        selectedEntityIndex = (selectedEntityIndex - 1 + static_cast<int>(playerEntities.size() + enemyEntities.size())) % 
                             static_cast<int>(playerEntities.size() + enemyEntities.size());
    }
    else if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        selectedEntityIndex = (selectedEntityIndex + 1) % 
                             static_cast<int>(playerEntities.size() + enemyEntities.size());
    }
    
    // Set target position with left/right
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
        targetPosition = std::max(0, targetPosition - 1);
    }
    else if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
        targetPosition = std::min(Battlefield::MAX_TILES - 1, targetPosition + 1);
    }
    
    // Confirm movement with space
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM) && targetPosition >= 0) {
        // Try to move entity
        MoveSelectedEntity(targetPosition);
    }
    
    // Exit on ESC
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void BattlefieldTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Clear background
    renderer.ClearBackground(RAYWHITE);
    
    // Draw title
    renderer.DrawTextCentered("BATTLEFIELD TEST", 
                             renderer.GetScreenWidth() / 2, 
                             50, 
                             30, 
                             BLACK);
    
    // Draw battlefield
    DrawBattlefield();
    
    // Draw entity info
    DrawEntityInfo();
    
    // Draw controls
    DrawControls();
}

void BattlefieldTestState::Pause() {
    std::cout << "Pausing Battlefield Test State" << std::endl;
}

void BattlefieldTestState::Resume() {
    std::cout << "Resuming Battlefield Test State" << std::endl;
}

void BattlefieldTestState::CreateTestEntities() {
    // Clear any existing entities
    battlefield.Clear();
    playerEntities.clear();
    enemyEntities.clear();
    
    // Create player entities
    for (int i = 0; i < 2; i++) {
        auto entity = std::make_shared<Entity>();
        
        // Add position component and set position
        auto& posComp = entity->AddComponent<PositionComponent>();
        posComp.SetPosition(i);
        
        auto& stats = entity->AddComponent<StatsComponent>();
        stats.SetBaseStat(StatType::STRENGTH, 10 + i * 2);
        stats.SetBaseStat(StatType::DEXTERITY, 8 + i);
        stats.SetBaseStat(StatType::CONSTITUTION, 12 - i);
        
        // Add to battlefield
        battlefield.PlaceEntity(entity, i);
        
        // Add to player entities
        playerEntities.push_back(entity);
    }
    
    // Create enemy entities
    for (int i = 0; i < 2; i++) {
        auto entity = std::make_shared<Entity>();
        
        // Add position component and set position
        int pos = Battlefield::MAX_TILES - 1 - i;
        auto& posComp = entity->AddComponent<PositionComponent>();
        posComp.SetPosition(pos);
        
        auto& stats = entity->AddComponent<StatsComponent>();
        stats.SetBaseStat(StatType::STRENGTH, 8 + i);
        stats.SetBaseStat(StatType::DEXTERITY, 10 - i);
        stats.SetBaseStat(StatType::CONSTITUTION, 9 + i * 2);
        
        // Add to battlefield
        battlefield.PlaceEntity(entity, pos);
        
        // Add to enemy entities
        enemyEntities.push_back(entity);
    }
    
    // Set initial selected entity and target position
    selectedEntityIndex = 0;
    
    // Set target position to the selected entity's position
    if (!playerEntities.empty()) {
        targetPosition = playerEntities[0]->GetComponent<PositionComponent>().GetPosition();
    } else {
        targetPosition = 0;
    }
}

void BattlefieldTestState::MoveSelectedEntity(int newPosition) {
    // Get selected entity
    Entity* entity = nullptr;
    if (selectedEntityIndex < static_cast<int>(playerEntities.size())) {
        entity = playerEntities[selectedEntityIndex].get();
    }
    else {
        entity = enemyEntities[selectedEntityIndex - playerEntities.size()].get();
    }
    
    // Try to move entity
    if (entity) {
        if (battlefield.CanMoveTo(entity, newPosition)) {
            battlefield.MoveEntity(entity, newPosition);
            std::cout << "Entity moved to position " << newPosition << std::endl;
        }
        else {
            std::cout << "Cannot move entity to position " << newPosition << std::endl;
        }
    }
}

void BattlefieldTestState::DrawBattlefield() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int tileSize = 60;
    int tileSpacing = 10;
    int totalWidth = Battlefield::MAX_TILES * (tileSize + tileSpacing) - tileSpacing;
    int startX = (renderer.GetScreenWidth() - totalWidth) / 2;
    int startY = renderer.GetScreenHeight() / 2 - tileSize / 2;
    
    // Draw battlefield divider (middle of the 8 tiles)
    int dividerX = startX + 4 * (tileSize + tileSpacing) - tileSpacing / 2;
    renderer.DrawRect(dividerX - 2, startY - 20, 4, tileSize + 40, DARKGRAY);
    
    // Draw battlefield tiles
    for (int i = 0; i < Battlefield::MAX_TILES; i++) {
        int x = startX + i * (tileSize + tileSpacing);
        
        // Determine color based on side (player/enemy)
        Color tileColor = (i < 4) ? SKYBLUE : PINK;
        
        // Highlight target position
        if (i == targetPosition) {
            tileColor = YELLOW;
        }
        
        // Draw tile
        renderer.DrawRect(x, startY, tileSize, tileSize, tileColor);
        
        // Draw position number
        renderer.DrawTextCentered(std::to_string(i).c_str(), 
                                 x + tileSize / 2, 
                                 startY - 15, 
                                 16, 
                                 BLACK);
        
        // Draw entity if present
        Entity* entity = battlefield.GetEntityAtPosition(i);
        if (entity) {
            // Determine if this is the selected entity
            bool isSelected = false;
            int entityIndex = -1;
            
            // Check in player entities
            for (size_t j = 0; j < playerEntities.size(); j++) {
                if (playerEntities[j].get() == entity) {
                    entityIndex = static_cast<int>(j);
                    isSelected = (selectedEntityIndex == entityIndex);
                    break;
                }
            }
            
            // Check in enemy entities
            if (entityIndex == -1) {
                for (size_t j = 0; j < enemyEntities.size(); j++) {
                    if (enemyEntities[j].get() == entity) {
                        entityIndex = static_cast<int>(j) + playerEntities.size();
                        isSelected = (selectedEntityIndex == entityIndex);
                        break;
                    }
                }
            }
            
            // Draw entity circle
            Color entityColor = (i < 4) ? BLUE : RED;
            if (isSelected) {
                entityColor = GREEN;
            }
            
            renderer.DrawCircle(x + tileSize / 2, startY + tileSize / 2, tileSize / 3, entityColor);
            
            // Draw entity index
            renderer.DrawTextCentered(std::to_string(entityIndex).c_str(), 
                                     x + tileSize / 2, 
                                     startY + tileSize / 2, 
                                     20, 
                                     WHITE);
        }
    }
    
    // Draw side labels
    renderer.DrawText("PLAYER SIDE", startX, startY + tileSize + 10, 20, DARKBLUE);
    renderer.DrawText("ENEMY SIDE", startX + 4 * (tileSize + tileSpacing), startY + tileSize + 10, 20, MAROON);
}

void BattlefieldTestState::DrawEntityInfo() {
    if (selectedEntityIndex < 0 || 
        selectedEntityIndex >= static_cast<int>(playerEntities.size() + enemyEntities.size())) {
        return;
    }
    
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Get selected entity
    Entity* entity = nullptr;
    std::string entityType;
    
    if (selectedEntityIndex < static_cast<int>(playerEntities.size())) {
        entity = playerEntities[selectedEntityIndex].get();
        entityType = "Player";
    }
    else {
        entity = enemyEntities[selectedEntityIndex - playerEntities.size()].get();
        entityType = "Enemy";
    }
    
    if (!entity) return;
    
    // Get entity components
    auto& posComponent = entity->GetComponent<PositionComponent>();
    auto& statsComponent = entity->GetComponent<StatsComponent>();
    
    // Draw info box
    int infoX = 50;
    int infoY = 100;
    int infoWidth = 200;
    int infoHeight = 150;
    
    renderer.DrawRect(infoX, infoY, infoWidth, infoHeight, LIGHTGRAY);
    renderer.DrawRectLines(infoX, infoY, infoWidth, infoHeight, BLACK);
    
    // Draw entity info
    renderer.DrawText("SELECTED ENTITY", infoX + 10, infoY + 10, 16, BLACK);
    renderer.DrawText(("Type: " + entityType).c_str(), infoX + 10, infoY + 35, 14, BLACK);
    renderer.DrawText(("Position: " + std::to_string(posComponent.GetPosition())).c_str(), 
                      infoX + 10, infoY + 55, 14, BLACK);
    
    // Draw stats
    renderer.DrawText(("STR: " + std::to_string(statsComponent.GetCurrentStat(StatType::STRENGTH))).c_str(), 
                      infoX + 10, infoY + 75, 14, BLACK);
    renderer.DrawText(("DEX: " + std::to_string(statsComponent.GetCurrentStat(StatType::DEXTERITY))).c_str(), 
                      infoX + 10, infoY + 95, 14, BLACK);
    renderer.DrawText(("CON: " + std::to_string(statsComponent.GetCurrentStat(StatType::CONSTITUTION))).c_str(), 
                      infoX + 10, infoY + 115, 14, BLACK);
}

void BattlefieldTestState::DrawControls() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int controlsX = renderer.GetScreenWidth() - 250;
    int controlsY = 100;
    int controlsWidth = 200;
    int controlsHeight = 200;
    
    renderer.DrawRect(controlsX, controlsY, controlsWidth, controlsHeight, LIGHTGRAY);
    renderer.DrawRectLines(controlsX, controlsY, controlsWidth, controlsHeight, BLACK);
    
    // Draw controls info
    renderer.DrawText("CONTROLS", controlsX + 10, controlsY + 10, 16, BLACK);
    renderer.DrawText("UP/DOWN: Select entity", controlsX + 10, controlsY + 35, 14, BLACK);
    renderer.DrawText("LEFT/RIGHT: Select position", controlsX + 10, controlsY + 55, 14, BLACK);
    renderer.DrawText("SPACE: Move entity", controlsX + 10, controlsY + 75, 14, BLACK);
    renderer.DrawText("ESC: Exit test", controlsX + 10, controlsY + 95, 14, BLACK);
    
    // Draw target position info
    std::string targetText = "Target: ";
    targetText += (targetPosition >= 0) ? std::to_string(targetPosition) : "None";
    renderer.DrawText(targetText.c_str(), controlsX + 10, controlsY + 125, 14, BLACK);
    
    // Draw movement validity info
    if (targetPosition >= 0 && selectedEntityIndex >= 0 &&
        selectedEntityIndex < static_cast<int>(playerEntities.size() + enemyEntities.size())) {
        
        Entity* entity = nullptr;
        if (selectedEntityIndex < static_cast<int>(playerEntities.size())) {
            entity = playerEntities[selectedEntityIndex].get();
        } else {
            entity = enemyEntities[selectedEntityIndex - playerEntities.size()].get();
        }
        
        if (entity) {
            bool canMove = battlefield.CanMoveTo(entity, targetPosition);
            std::string moveText = "Can move: ";
            moveText += canMove ? "Yes" : "No";
            renderer.DrawText(moveText.c_str(), controlsX + 10, controlsY + 145, 14, 
                             canMove ? GREEN : RED);
        }
    }
}

} // namespace Game 
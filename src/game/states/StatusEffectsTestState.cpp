#include "StatusEffectsTestState.h"
#include "../../engine/rendering/Renderer.h"
#include "../../engine/input/InputHandler.h"
#include "../entities/components/StatsComponent.h"
#include <iostream>

namespace Game {

StatusEffectsTestState::StatusEffectsTestState()
    : testEntity(nullptr),
      selectedEffectIndex(0),
      effectDuration(3),
      effectMagnitude(5),
      selectedField(UIField::EFFECT_TYPE),
      isPaused(false) {
    
    // Initialize available status effects
    availableEffects = {
        {"Poison", StatusEffectType::POISON},
        {"Stun", StatusEffectType::STUN},
        {"Strength Buff", StatusEffectType::BUFF},
        {"Strength Debuff", StatusEffectType::DEBUFF}
    };
}

void StatusEffectsTestState::Enter() {
    std::cout << "Entering StatusEffectsTestState" << std::endl;
    
    // Create test entity
    testEntity = std::make_shared<Entity>("Test Entity");
    
    // Add stats component
    auto& stats = testEntity->AddComponent<StatsComponent>();
    stats.Initialize(10, 10, 10, 10, 10, 10, 10);
    
    // Add status effects component
    testEntity->AddComponent<StatusEffectsComponent>();
    
    std::cout << "Entity created with Stats and StatusEffects components" << std::endl;
}

void StatusEffectsTestState::Update(float deltaTime) {
    if (isPaused) return;
    
    Engine::InputHandler& input = Engine::InputHandler::GetInstance();
    
    // Switch between UI fields
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_DOWN)) {
        int nextField = static_cast<int>(selectedField) + 1;
        if (nextField > static_cast<int>(UIField::APPLY)) {
            nextField = 0;
        }
        selectedField = static_cast<UIField>(nextField);
    }
    if (input.IsActionJustPressed(Engine::InputAction::MOVE_UP)) {
        int nextField = static_cast<int>(selectedField) - 1;
        if (nextField < 0) {
            nextField = static_cast<int>(UIField::APPLY);
        }
        selectedField = static_cast<UIField>(nextField);
    }
    
    // Handle input based on selected field
    switch (selectedField) {
        case UIField::EFFECT_TYPE:
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
                selectedEffectIndex = (selectedEffectIndex + 1) % availableEffects.size();
            }
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
                selectedEffectIndex = (selectedEffectIndex - 1 + availableEffects.size()) % availableEffects.size();
            }
            break;
            
        case UIField::DURATION:
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
                effectDuration++;
            }
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
                if (effectDuration > 1) effectDuration--;
            }
            break;
            
        case UIField::MAGNITUDE:
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_RIGHT)) {
                effectMagnitude++;
            }
            if (input.IsActionJustPressed(Engine::InputAction::MOVE_LEFT)) {
                if (effectMagnitude > 1) effectMagnitude--;
            }
            break;
            
        case UIField::APPLY:
            if (input.IsActionJustPressed(Engine::InputAction::CONFIRM)) {
                ApplySelectedEffect();
            }
            break;
    }
    
    // Process turn events with spacebar
    if (input.IsActionJustPressed(Engine::InputAction::CONFIRM) && 
        selectedField != UIField::APPLY) {
        
        // Simulate a turn
        auto& statusEffects = testEntity->GetComponent<StatusEffectsComponent>();
        
        std::cout << "--- Processing Turn ---" << std::endl;
        
        // Process start of turn
        statusEffects.ProcessTurnStart();
        
        // Check if entity can take a turn
        bool canTakeTurn = statusEffects.ProcessNewTurn();
        if (canTakeTurn) {
            std::cout << testEntity->GetName() << " takes a turn." << std::endl;
        } else {
            std::cout << testEntity->GetName() << " cannot take a turn." << std::endl;
        }
        
        // Process end of turn
        statusEffects.ProcessTurnEnd();
    }
    
    // Check for exit
    if (input.IsActionJustPressed(Engine::InputAction::CANCEL)) {
        Engine::StateManager::GetInstance().PopState();
    }
}

void StatusEffectsTestState::Render() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    // Draw title
    renderer.DrawText("Status Effects Test", 50, 50, 24, BLACK);
    
    // Draw entity stats
    auto& stats = testEntity->GetComponent<StatsComponent>();
    renderer.DrawText("Entity Stats:", 50, 100, 20, DARKGRAY);
    
    renderer.DrawText(("HP: " + std::to_string(stats.GetCurrentHealth()) + "/" + 
                      std::to_string(stats.GetMaxHealth())).c_str(), 
                     50, 130, 16, BLACK);
    
    int y = 160;
    for (int i = 0; i < 7; i++) {
        StatType type = static_cast<StatType>(i);
        std::string name = StatsComponent::GetStatName(type);
        int baseValue = stats.GetBaseStat(type);
        int currentValue = stats.GetCurrentStat(type);
        
        // Draw stat name and value
        renderer.DrawText((name + ":").c_str(), 50, y, 16, BLACK);
        
        // Show both base and current value if they differ
        if (baseValue != currentValue) {
            renderer.DrawText((std::to_string(baseValue) + " (" + 
                              std::to_string(currentValue) + ")").c_str(), 
                             200, y, 16, DARKBLUE);
        } else {
            renderer.DrawText(std::to_string(baseValue).c_str(), 
                             200, y, 16, BLACK);
        }
        
        y += 25;
    }
    
    // Draw active effects
    DrawActiveEffects();
    
    // Draw effect selection UI
    DrawEffectInfo();
    
    // Draw instructions
    renderer.DrawText("Controls:", 50, 500, 18, BLACK);
    renderer.DrawText("Up/Down: Navigate fields", 50, 530, 16, DARKGRAY);
    renderer.DrawText("Left/Right: Change values", 50, 550, 16, DARKGRAY);
    renderer.DrawText("Space: Apply effect / Process turn", 50, 570, 16, DARKGRAY);
}

void StatusEffectsTestState::Exit() {
    std::cout << "Exiting StatusEffectsTestState" << std::endl;
    testEntity = nullptr;
}

void StatusEffectsTestState::ApplySelectedEffect() {
    if (selectedEffectIndex >= 0 && selectedEffectIndex < static_cast<int>(availableEffects.size())) {
        auto& statusEffects = testEntity->GetComponent<StatusEffectsComponent>();
        
        // Get selected effect
        const auto& [effectName, effectType] = availableEffects[selectedEffectIndex];
        
        // Create and apply the effect
        auto effect = CreateStatusEffect(effectType, effectDuration, effectMagnitude);
        if (effect) {
            statusEffects.AddEffect(std::move(effect));
        }
    }
}

void StatusEffectsTestState::DrawEffectInfo() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int x = 400;
    int y = 100;
    
    renderer.DrawText("Add Status Effect:", x, y, 20, BLACK);
    y += 40;
    
    // Effect Type
    Engine::RColor effectTypeColor = (selectedField == UIField::EFFECT_TYPE) ? RED : BLACK;
    renderer.DrawText("Effect Type:", x, y, 16, effectTypeColor);
    renderer.DrawText(availableEffects[selectedEffectIndex].first.c_str(), 
                     x + 150, y, 16, effectTypeColor);
    y += 30;
    
    // Duration
    Engine::RColor durationColor = (selectedField == UIField::DURATION) ? RED : BLACK;
    renderer.DrawText("Duration:", x, y, 16, durationColor);
    renderer.DrawText(std::to_string(effectDuration).c_str(), 
                     x + 150, y, 16, durationColor);
    y += 30;
    
    // Magnitude
    Engine::RColor magnitudeColor = (selectedField == UIField::MAGNITUDE) ? RED : BLACK;
    renderer.DrawText("Magnitude:", x, y, 16, magnitudeColor);
    renderer.DrawText(std::to_string(effectMagnitude).c_str(), 
                     x + 150, y, 16, magnitudeColor);
    y += 40;
    
    // Apply Button
    Engine::RColor applyColor = (selectedField == UIField::APPLY) ? RED : BLACK;
    renderer.DrawRect(x, y, 100, 30, applyColor);
    renderer.DrawRect(x + 2, y + 2, 96, 26, WHITE);
    renderer.DrawText("Apply", x + 25, y + 7, 16, applyColor);
}

void StatusEffectsTestState::DrawActiveEffects() {
    Engine::Renderer& renderer = Engine::Renderer::GetInstance();
    
    int x = 400;
    int y = 300;
    
    renderer.DrawText("Active Effects:", x, y, 20, BLACK);
    y += 30;
    
    auto& statusEffects = testEntity->GetComponent<StatusEffectsComponent>();
    const auto& effects = statusEffects.GetEffects();
    
    if (effects.empty()) {
        renderer.DrawText("No active effects", x, y, 16, GRAY);
        return;
    }
    
    for (const auto& effect : effects) {
        std::string effectInfo = effect->GetName() + " (" + 
                               std::to_string(effect->GetDuration()) + " turns)";
        
        renderer.DrawText(effectInfo.c_str(), x, y, 16, DARKGREEN);
        y += 20;
        
        renderer.DrawText(effect->GetDescription().c_str(), x + 20, y, 14, GRAY);
        y += 30;
    }
}

void StatusEffectsTestState::Pause() {
    std::cout << "Pausing StatusEffectsTestState" << std::endl;
    isPaused = true;
}

void StatusEffectsTestState::Resume() {
    std::cout << "Resuming StatusEffectsTestState" << std::endl;
    isPaused = false;
}

} // namespace Game 
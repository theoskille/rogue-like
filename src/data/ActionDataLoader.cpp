#include "ActionDataLoader.h"
#include "json.hpp"
#include <fstream>
#include <iostream>

namespace Game {

// Initialize static instance
ActionDataLoader* ActionDataLoader::instance = nullptr;

ActionDataLoader& ActionDataLoader::GetInstance() {
    if (instance == nullptr) {
        instance = new ActionDataLoader();
    }
    return *instance;
}

bool ActionDataLoader::LoadActions(const std::string& filepath) {
    try {
        // Clear previous actions
        actions.clear();
        
        // Open and parse JSON file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open actions file: " << filepath << std::endl;
            return false;
        }
        
        std::cout << "Loading actions from: " << filepath << std::endl;
        
        // Parse JSON
        nlohmann::json actionsJson;
        file >> actionsJson;
        
        // Process each action
        for (auto it = actionsJson.begin(); it != actionsJson.end(); ++it) {
            const std::string& id = it.key();
            const auto& actionData = it.value();
            
            // Get basic properties
            std::string name = actionData["name"];
            std::string typeStr = actionData["type"];
            ActionType type = StringToActionType(typeStr);
            
            // Create action
            auto action = std::make_shared<Action>(id, name, type);
            
            // Set description if available
            if (actionData.contains("description")) {
                action->SetDescription(actionData["description"]);
            }
            
            // Set numeric properties
            if (actionData.contains("accuracy")) {
                action->SetAccuracy(actionData["accuracy"]);
            }
            
            if (actionData.contains("damage")) {
                action->SetDamage(actionData["damage"]);
            }
            
            if (actionData.contains("range")) {
                action->SetRange(actionData["range"]);
            }
            
            if (actionData.contains("cooldown")) {
                action->SetCooldown(actionData["cooldown"]);
            }
            
            // Set additional properties
            if (actionData.contains("properties") && actionData["properties"].is_object()) {
                const auto& props = actionData["properties"];
                for (auto propIt = props.begin(); propIt != props.end(); ++propIt) {
                    action->SetProperty(propIt.key(), propIt.value());
                }
            }
            
            // Store the action
            actions[id] = action;
            
            std::cout << "Loaded action: " << id << " - " << name << std::endl;
        }
        
        std::cout << "Successfully loaded " << actions.size() << " actions" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading actions: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Action> ActionDataLoader::GetAction(const std::string& id) const {
    auto it = actions.find(id);
    if (it != actions.end()) {
        return it->second;
    }
    return nullptr;
}

bool ActionDataLoader::HasAction(const std::string& id) const {
    return actions.find(id) != actions.end();
}

const std::unordered_map<std::string, std::shared_ptr<Action>>& ActionDataLoader::GetAllActions() const {
    return actions;
}

ActionType ActionDataLoader::StringToActionType(const std::string& typeStr) const {
    if (typeStr == "ATTACK") return ActionType::ATTACK;
    if (typeStr == "BUFF") return ActionType::BUFF;
    if (typeStr == "DEBUFF") return ActionType::DEBUFF;
    if (typeStr == "HEAL") return ActionType::HEAL;
    if (typeStr == "MOVEMENT") return ActionType::MOVEMENT;
    if (typeStr == "SPECIAL") return ActionType::SPECIAL;
    if (typeStr == "COMPOUND") return ActionType::COMPOUND;
    
    // Default to ATTACK if unknown
    std::cerr << "Unknown action type: " << typeStr << ", defaulting to ATTACK" << std::endl;
    return ActionType::ATTACK;
}

} // namespace Game 
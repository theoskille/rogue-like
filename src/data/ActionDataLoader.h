#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "DataLoader.h"
#include "../game/combat/Action.h"

namespace Game {

// Loader for action data from JSON
class ActionDataLoader {
public:
    // Singleton pattern
    static ActionDataLoader& GetInstance();
    
    // Load all actions from a JSON file
    bool LoadActions(const std::string& filepath);
    
    // Get an action by ID
    std::shared_ptr<Action> GetAction(const std::string& id) const;
    
    // Check if an action exists
    bool HasAction(const std::string& id) const;
    
    // Get all loaded actions
    const std::unordered_map<std::string, std::shared_ptr<Action>>& GetAllActions() const;
    
    // Get action count
    size_t GetActionCount() const { return actions.size(); }
    
private:
    ActionDataLoader() = default;
    ~ActionDataLoader() = default;
    
    // Static instance
    static ActionDataLoader* instance;
    
    // Map of loaded actions
    std::unordered_map<std::string, std::shared_ptr<Action>> actions;
    
    // Helper to convert string to ActionType
    ActionType StringToActionType(const std::string& typeStr) const;
};

} // namespace Game 
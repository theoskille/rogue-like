#pragma once

#include <string>
#include <memory>

namespace Game {

// Forward declarations
class Entity;

/**
 * Base class for all types of room encounters
 */
enum class EncounterType {
    COMBAT,   // Battle against enemies
    TREASURE, // Treasure chest or reward
    EMPTY     // No special encounter
};

/**
 * Encounter result after completion
 */
enum class EncounterResult {
    NONE,       // Not completed yet
    VICTORY,    // Player won combat
    DEFEAT,     // Player lost combat
    COMPLETED,  // Non-combat encounter completed
    SKIPPED     // Player skipped encounter
};

/**
 * Base class for all room encounters
 */
class Encounter {
public:
    Encounter(EncounterType type, const std::string& name);
    virtual ~Encounter() = default;
    
    // Getters
    EncounterType GetType() const { return type; }
    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    bool IsCompleted() const { return completed; }
    EncounterResult GetResult() const { return result; }
    
    // Setters
    void SetDescription(const std::string& desc) { description = desc; }
    
    // Encounter lifecycle
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual bool IsActive() const = 0;
    
    // Complete the encounter
    virtual void Complete(EncounterResult result);
    
protected:
    EncounterType type;
    std::string name;
    std::string description;
    bool completed;
    EncounterResult result;
};

} // namespace Game 
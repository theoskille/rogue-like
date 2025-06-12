#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include "../entities/Entity.h"
#include "../entities/components/StatsComponent.h"

namespace Game {

// Forward declaration
class Battlefield;

// Types of actions that can be performed
enum class ActionType {
    ATTACK,     // Damage opponent
    BUFF,       // Apply positive effect to self
    DEBUFF,     // Apply negative effect to opponent
    HEAL,       // Restore health
    MOVEMENT,   // Reposition on the battlefield
    SPECIAL,    // Unique or combined effects
    COMPOUND    // Multiple effects combined
};

// Effect interface - represents a single effect that an action can have
class ActionEffect {
public:
    virtual ~ActionEffect() = default;
    
    // Execute the effect
    virtual bool Execute(Entity* user, Entity* target, Battlefield* battlefield) = 0;
    
    // Check if the effect can be applied
    virtual bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const = 0;
};

// Damage effect - deals damage to target
class DamageEffect : public ActionEffect {
public:
    DamageEffect(int baseDamage, bool isPhysical = true);
    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override;
    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override;
    
private:
    int damage;
    bool isPhysical;
};

// Healing effect - restores health to target
class HealingEffect : public ActionEffect {
public:
    explicit HealingEffect(int healAmount);
    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override;
    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override;
    
private:
    int amount;
};

// Movement effect - changes position on battlefield
class MovementEffect : public ActionEffect {
public:
    explicit MovementEffect(int positionChange);
    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override;
    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override;
    
private:
    int positionChange;
};

// Stat modifier effect - buffs or debuffs a stat
class StatModifierEffect : public ActionEffect {
public:
    StatModifierEffect(StatType statType, int value, int duration);
    bool Execute(Entity* user, Entity* target, Battlefield* battlefield) override;
    bool CanApply(const Entity* user, const Entity* target, const Battlefield* battlefield) const override;
    
private:
    StatType statType;
    int value;
    int duration;
};

// Class for combat actions (attacks, abilities, etc.)
class Action {
public:
    // Constructor with basic properties
    Action(const std::string& id, const std::string& name, ActionType type);
    
    // Destructor
    virtual ~Action();
    
    // Execute the action
    virtual bool Execute(Entity* user, Entity* target, Battlefield* battlefield);
    
    // Check if the action can be used
    virtual bool CanUse(const Entity* user, const Entity* target, const Battlefield* battlefield) const;
    
    // Get action details
    const std::string& GetID() const { return id; }
    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    ActionType GetType() const { return type; }
    
    // Set action details
    void SetDescription(const std::string& desc) { description = desc; }
    
    // Get action properties
    int GetAccuracy() const { return accuracy; }
    int GetRange() const { return range; }
    int GetCooldown() const { return cooldown; }
    int GetCurrentCooldown() const { return currentCooldown; }
    
    // Set action properties
    void SetAccuracy(int value) { accuracy = value; }
    void SetRange(int value) { range = value; }
    void SetCooldown(int value) { cooldown = value; }
    
    // For backward compatibility
    int GetDamage() const; 
    void SetDamage(int value);
    
    // Cooldown management
    void StartCooldown() { currentCooldown = cooldown; }
    void DecreaseCooldown() { if (currentCooldown > 0) currentCooldown--; }
    bool IsOnCooldown() const { return currentCooldown > 0; }
    
    // Custom effect callback
    void SetEffectCallback(std::function<void(Entity*, Entity*, Battlefield*)> callback) {
        effectCallback = callback;
    }
    
    // Add or set an additional property
    void SetProperty(const std::string& key, int value) {
        properties[key] = value;
    }
    
    // Get an additional property (returns 0 if not found)
    int GetProperty(const std::string& key) const {
        auto it = properties.find(key);
        return (it != properties.end()) ? it->second : 0;
    }
    
    // Add an effect to this action
    void AddEffect(std::unique_ptr<ActionEffect> effect) {
        effects.push_back(std::move(effect));
    }
    
    // Create standard effects based on properties
    void CreateEffectsFromProperties();
    
private:
    // Basic identification
    std::string id;              // Unique identifier
    std::string name;            // Display name
    std::string description;     // Action description
    ActionType type;             // Type of action
    
    // Core properties
    int accuracy = 100;          // Hit chance percentage (0-100)
    int range = 1;               // Required distance to use (tiles)
    int cooldown = 0;            // Turns before reuse
    int currentCooldown = 0;     // Current cooldown countdown
    
    // Custom effect callback
    std::function<void(Entity*, Entity*, Battlefield*)> effectCallback = nullptr;
    
    // Additional properties (knockback, healing amount, etc.)
    std::unordered_map<std::string, int> properties;
    
    // Collection of effects this action will apply
    std::vector<std::unique_ptr<ActionEffect>> effects;
};

} // namespace Game 
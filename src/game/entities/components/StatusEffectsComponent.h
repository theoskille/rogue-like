#pragma once

#include "Component.h"
#include "StatsComponent.h"
#include "../Entity.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace Game {

// Forward declarations
class Entity;

// Enum for status effect types
enum class StatusEffectType {
    POISON,         // Damage over time
    STUN,           // Skip turns
    BUFF,           // Stat increase
    DEBUFF,         // Stat decrease
    BURNING,        // Damage over time (fire)
    FREEZING,       // Reduced speed
    BLEEDING,       // Damage over time (physical)
    CONFUSION,      // Random action
    BLIND,          // Reduced accuracy
    SHIELD          // Damage reduction
};

// Base class for all status effects
class StatusEffect {
public:
    StatusEffect(StatusEffectType type, int duration, const std::string& name);
    virtual ~StatusEffect() = default;
    
    // Apply the effect at the start of a turn
    virtual void OnTurnStart(Entity* entity) = 0;
    
    // Apply the effect at the end of a turn
    virtual void OnTurnEnd(Entity* entity) = 0;
    
    // Called when a new turn is taken by the entity
    virtual bool OnNewTurn(Entity* entity);
    
    // Getters for basic properties
    StatusEffectType GetType() const { return type; }
    int GetDuration() const { return duration; }
    std::string GetName() const { return name; }
    std::string GetDescription() const { return description; }
    
    // Check if effect has expired
    bool HasExpired() const { return duration <= 0; }
    
    // Setters
    void SetDescription(const std::string& desc) { description = desc; }
    
protected:
    StatusEffectType type;
    int duration;
    std::string name;
    std::string description;
    
    // Reduce the duration by 1
    void DecreaseDuration() { if (duration > 0) duration--; }
};

// Component to manage status effects on an entity
class StatusEffectsComponent : public Component {
public:
    StatusEffectsComponent();
    
    // Add a status effect to the entity
    void AddEffect(std::unique_ptr<StatusEffect> effect);
    
    // Remove a status effect by name
    void RemoveEffect(const std::string& effectName);
    
    // Clear all status effects
    void ClearEffects();
    
    // Get all status effects
    const std::vector<std::unique_ptr<StatusEffect>>& GetEffects() const;
    
    // Check if entity has a specific effect
    bool HasEffect(StatusEffectType type) const;
    bool HasEffectByName(const std::string& effectName) const;
    
    // Process effects at turn start
    void ProcessTurnStart();
    
    // Process effects at turn end
    void ProcessTurnEnd();
    
    // Process new turn, returns true if entity can take a turn
    bool ProcessNewTurn();
    
    // Override Component methods
    void Update(float deltaTime) override;
    
    // Component lifecycle
    void OnAttach(Entity* entity) override;
    
private:
    // Reference to the owning entity
    Entity* owner;
    
    // List of active status effects
    std::vector<std::unique_ptr<StatusEffect>> activeEffects;
    
    // Helper to remove expired effects
    void RemoveExpiredEffects();
};

// Concrete status effect implementations

// Poison - deals damage over time
class PoisonEffect : public StatusEffect {
public:
    PoisonEffect(int duration, int damagePerTurn);
    
    void OnTurnStart(Entity* entity) override;
    void OnTurnEnd(Entity* entity) override;
    
private:
    int damagePerTurn;
};

// Stun - prevents the entity from taking actions
class StunEffect : public StatusEffect {
public:
    explicit StunEffect(int duration);
    
    void OnTurnStart(Entity* entity) override;
    void OnTurnEnd(Entity* entity) override;
    bool OnNewTurn(Entity* entity) override;
};

// StatBuffEffect - increases or decreases a stat
class StatBuffEffect : public StatusEffect {
public:
    StatBuffEffect(int duration, StatType statType, int modifierValue);
    
    void OnTurnStart(Entity* entity) override;
    void OnTurnEnd(Entity* entity) override;
    
private:
    StatType statType;
    int modifierValue;
    bool applied;
};

// Factory function to create status effects
std::unique_ptr<StatusEffect> CreateStatusEffect(
    StatusEffectType type, 
    int duration, 
    int magnitude = 0);

} // namespace Game 
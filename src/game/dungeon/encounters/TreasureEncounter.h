#pragma once

#include "Encounter.h"
#include <vector>
#include <string>

namespace Game {

/**
 * Item in a treasure encounter
 */
struct TreasureItem {
    std::string id;
    std::string name;
    std::string description;
    int value;
    
    TreasureItem(const std::string& id, const std::string& name, 
                const std::string& description, int value)
        : id(id), name(name), description(description), value(value) {}
};

/**
 * Treasure encounter with items/rewards
 */
class TreasureEncounter : public Encounter {
public:
    TreasureEncounter(const std::string& name, int quality);
    ~TreasureEncounter() override = default;
    
    // Quality level (affects treasure value)
    int GetQuality() const { return quality; }
    
    // Encounter interface implementation
    void Start() override;
    void Update(float deltaTime) override;
    bool IsActive() const override;
    void Complete(EncounterResult result) override;
    
    // Treasure management
    void AddTreasureItem(const TreasureItem& item);
    void GenerateTreasure();
    
    // Getters
    const std::vector<TreasureItem>& GetItems() const { return items; }
    
private:
    int quality;
    std::vector<TreasureItem> items;
    bool isActive;
    float timeElapsed;
    
    // Treasure generation helper
    TreasureItem CreateRandomTreasure(int level);
};

} // namespace Game 
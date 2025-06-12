#include "TreasureEncounter.h"
#include <iostream>
#include <random>
#include <sstream>

namespace Game {

TreasureEncounter::TreasureEncounter(const std::string& name, int quality)
    : Encounter(EncounterType::TREASURE, name),
      quality(std::max(1, quality)),
      isActive(false),
      timeElapsed(0.0f) {
    
    // Set more specific description based on quality
    std::stringstream ss;
    if (quality <= 1) {
        ss << "A small chest with some basic loot.";
    } else if (quality <= 3) {
        ss << "A medium-sized chest that might contain valuable items.";
    } else if (quality <= 5) {
        ss << "A large ornate chest that looks very promising!";
    } else {
        ss << "An ancient treasure hoard of legendary quality!";
    }
    description = ss.str();
}

void TreasureEncounter::Start() {
    if (!completed && !isActive) {
        std::cout << "Starting treasure encounter: " << name << std::endl;
        
        // Ensure we have some treasure
        if (items.empty()) {
            // Generate treasure based on quality
            GenerateTreasure();
        }
        
        isActive = true;
        timeElapsed = 0.0f;
    }
}

void TreasureEncounter::Update(float deltaTime) {
    if (!completed && isActive) {
        timeElapsed += deltaTime;
        
        // In a real game, you'd wait for player interaction here
        // For now, we'll just complete it after a delay for testing
        if (timeElapsed > 5.0f) {
            Complete(EncounterResult::COMPLETED);
        }
    }
}

bool TreasureEncounter::IsActive() const {
    return isActive && !completed;
}

void TreasureEncounter::Complete(EncounterResult result) {
    // Call base class implementation
    Encounter::Complete(result);
    
    // Clean up
    isActive = false;
    
    // Log the loot obtained
    std::cout << "Treasure encounter completed! Items obtained:" << std::endl;
    for (const auto& item : items) {
        std::cout << "- " << item.name << " (" << item.value << " gold)" << std::endl;
    }
}

void TreasureEncounter::AddTreasureItem(const TreasureItem& item) {
    items.push_back(item);
}

void TreasureEncounter::GenerateTreasure() {
    // Clear existing items
    items.clear();
    
    // Generate items based on quality
    int itemCount = 1 + quality / 2;
    
    for (int i = 0; i < itemCount; ++i) {
        // Create item with value based on quality
        auto item = CreateRandomTreasure(quality);
        items.push_back(item);
    }
    
    std::cout << "Generated " << items.size() << " treasure items for encounter: " << name << std::endl;
}

TreasureItem TreasureEncounter::CreateRandomTreasure(int level) {
    // Random engine
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    // Generate random treasure type (0-3)
    std::uniform_int_distribution<> typeDist(0, 3);
    int treasureType = typeDist(gen);
    
    // Create ID based on type and randomness
    std::uniform_int_distribution<> idDist(1, 1000);
    int itemId = idDist(gen);
    
    std::stringstream idSs;
    std::string name, description;
    int baseValue = 10 * level;
    
    switch (treasureType) {
        case 0: // Gold
            idSs << "gold_" << itemId;
            name = "Gold Coins";
            description = "A pile of shiny gold coins.";
            baseValue = 5 * level + (level * level);
            break;
            
        case 1: // Weapon
            idSs << "weapon_" << itemId;
            
            if (level <= 2) {
                name = "Common Sword";
                description = "A basic but functional sword.";
            } else if (level <= 4) {
                name = "Quality Blade";
                description = "A well-crafted blade of good steel.";
                baseValue *= 2;
            } else {
                name = "Legendary Weapon";
                description = "A weapon of extraordinary power.";
                baseValue *= 5;
            }
            break;
            
        case 2: // Armor
            idSs << "armor_" << itemId;
            
            if (level <= 2) {
                name = "Leather Armor";
                description = "Basic protective gear made of leather.";
            } else if (level <= 4) {
                name = "Chain Mail";
                description = "Metal rings linked together for protection.";
                baseValue *= 2;
            } else {
                name = "Enchanted Plate";
                description = "Magical armor that seems to move with you.";
                baseValue *= 5;
            }
            break;
            
        case 3: // Potion
            idSs << "potion_" << itemId;
            
            if (level <= 2) {
                name = "Minor Healing Potion";
                description = "Restores a small amount of health.";
            } else if (level <= 4) {
                name = "Healing Potion";
                description = "Restores a significant amount of health.";
                baseValue *= 1.5;
            } else {
                name = "Elixir of Life";
                description = "Completely restores health and grants temporary invulnerability.";
                baseValue *= 3;
            }
            break;
            
        default:
            idSs << "item_" << itemId;
            name = "Mysterious Item";
            description = "An unidentified object of unknown origin.";
            break;
    }
    
    // Add some randomness to the value
    std::uniform_int_distribution<> valueMod(-level, level * 2);
    int finalValue = std::max(1, baseValue + valueMod(gen));
    
    return TreasureItem(idSs.str(), name, description, finalValue);
}

} // namespace Game 
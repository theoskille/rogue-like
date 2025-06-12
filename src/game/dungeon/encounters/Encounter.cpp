#include "Encounter.h"
#include <iostream>

namespace Game {

Encounter::Encounter(EncounterType type, const std::string& name)
    : type(type),
      name(name),
      description(""),
      completed(false),
      result(EncounterResult::NONE) {
    
    // Set default description based on encounter type
    switch (type) {
        case EncounterType::COMBAT:
            description = "A hostile group of enemies blocks your path.";
            break;
        case EncounterType::TREASURE:
            description = "You discover a treasure chest containing valuable items.";
            break;
        case EncounterType::EMPTY:
            description = "An empty area with nothing of interest.";
            break;
        default:
            description = "An unknown encounter.";
            break;
    }
    
    std::cout << "Created encounter: " << name << " of type " 
              << static_cast<int>(type) << std::endl;
}

void Encounter::Complete(EncounterResult encounterResult) {
    if (!completed) {
        completed = true;
        result = encounterResult;
        
        std::cout << "Encounter " << name << " completed with result: " 
                  << static_cast<int>(result) << std::endl;
    }
}

} // namespace Game 
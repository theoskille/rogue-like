#include "EventSystem.h"
#include <iostream>

namespace Engine {

// Initialize static member
EventSystem* EventSystem::instance = nullptr;

EventSystem::EventSystem() {
    // Set singleton instance
    if (instance == nullptr) {
        instance = this;
    } else {
        std::cerr << "Warning: Multiple EventSystem instances created!" << std::endl;
    }
}

EventSystem::~EventSystem() {
    // Clean up the singleton instance
    if (instance == this) {
        instance = nullptr;
    }
}

void EventSystem::Subscribe(EventType type, EventCallback callback) {
    subscribers[type].push_back(callback);
}

void EventSystem::Unsubscribe(EventType type, EventCallback callback) {
    // Note: This is a placeholder. Properly comparing std::function objects
    // is non-trivial. In a production system, we would use a unique ID for each
    // subscription to allow unsubscribing.
    
    // For now, just print a message
    std::cout << "Unsubscribe not implemented yet." << std::endl;
}

void EventSystem::Publish(const Event& event) {
    // Find subscribers for this event type
    auto it = subscribers.find(event.type);
    if (it != subscribers.end()) {
        // Call each callback with the event
        for (const auto& callback : it->second) {
            callback(event);
        }
    }
}

EventSystem& EventSystem::GetInstance() {
    if (instance == nullptr) {
        std::cerr << "Warning: EventSystem instance not created yet!" << std::endl;
        // Create a default instance if none exists
        static EventSystem defaultInstance;
        return defaultInstance;
    }
    return *instance;
}

} // namespace Engine 
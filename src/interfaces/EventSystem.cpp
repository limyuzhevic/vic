// EventSystem.cpp - Implementation of EventSystem with factory support
// Integrates with ComponentFactory for plugin support

#pragma once

#include "EventSystem.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/ComponentFactory.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace nlm {

// Event factory for creating custom events
class EventFactory {
public:
    EventFactory() = default;
    
    // Register event creator for specific event type
    void registerEventCreator(const std::string& eventType, 
                              std::function<std::unique_ptr<Event>()> creator) {
        creators[eventType] = std::move(creator);
    }
    
    // Create event by type
    std::unique_ptr<Event> createEvent(const std::string& eventType) {
        auto it = creators.find(eventType);
        if (it != creators.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    // Check if event type is supported
    bool supportsEventType(const std::string& eventType) const {
        return creators.find(eventType) != creators.end();
    }
    
    // Get all supported event types
    std::vector<std::string> getSupportedEventTypes() const {
        std::vector<std::string> types;
        types.reserve(creators.size());
        for (const auto& pair : creators) {
            types.push_back(pair.first);
        }
        return types;
    }
    
private:
    std::unordered_map<std::string, std::function<std::unique_ptr<Event>()>> creators;
};

// Plugin for EventSystem integration with ComponentFactory
class EventSystemPlugin : public IComponentPlugin {
public:
    EventSystemPlugin() = default;
    
    std::string getComponentType() const override {
        return "EventSystem";
    }
    
    std::unique_ptr<void> createComponent(const std::shared_ptr<Config>& config,
                                         const std::shared_ptr<RandomGenerator>& rng,
                                         const std::map<std::string, std::unique_ptr<void>>& dependencies,
                                         const ComponentMetadata& metadata) override {
        auto eventSystem = std::make_unique<EventSystem>();
        
        // Configure based on metadata and config
        if (config) {
            auto maxEvents = config->getOr<uint64_t>("max_events_in_memory", 10000);
            eventSystem->setMaxEventsInMemory(maxEvents);
            
            auto timeout = config->getOr<float>("event_timeout", 1000.0f);
            eventSystem->setEventTimeout(timeout);
            
            auto queueSize = config->getOr<uint64_t>("max_event_queue_size", 4096);
            eventSystem->setMaxEventQueueSize(queueSize);
        }
        
        // Register event creators based on metadata
        registerEventCreators(*eventSystem, metadata);
        
        return eventSystem;
    }
    
    bool isCompatible(const std::shared_ptr<Config>& config) const override {
        if (!config) {
            return false;
        }
        
        // Check if config has event system specific parameters
        return config->has("max_events_in_memory") || 
               config->has("event_timeout") ||
               config->has("max_event_queue_size");
    }
    
    void cleanup() override {}
    
    std::string getPluginName() const override {
        return "EventSystemPlugin";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
private:
    void registerEventCreators(EventSystem& eventSystem, const ComponentMetadata& metadata) {
        // Register SpikeEvent creator
        eventSystem.registerEventCreator("SpikeEvent", []() {
            return std::make_unique<SpikeEvent>();
        });
        
        // Register DelayedSpikeEvent creator
        eventSystem.registerEventCreator("DelayedSpikeEvent", []() {
            return std::make_unique<DelayedSpikeEvent>();
        });
        
        // Additional event types can be registered here
    }
};

// Integration function to register EventSystem with ComponentFactory
void registerEventSystemWithFactory(ComponentFactory& factory) {
    auto plugin = std::make_unique<EventSystemPlugin>();
    factory.registerPlugin(std::move(plugin));
    
    // Register default event system metadata
    factory.registerComponentMetadata(
        "EventSystem",
        "EventSystem",
        "Advanced event-driven computation system with scheduling and filtering",
        {"SpikeSystem", "ComponentFactory"},
        true,
        true,
        5  // Initialization order 5 (after SpikeSystem at 0, after basic systems)
    );
}

// Integration function to initialize Brain's EventSystem
bool initializeBrainEventSystem(Brain& brain, std::shared_ptr<Config> config) {
    // Check if ComponentFactory exists
    auto& factory = ComponentFactory::getInstance();
    
    // Register EventSystem with factory if not already registered
    static bool registered = []() {
        registerEventSystemWithFactory(factory);
        return true;
    }();
    
    try {
        // Create EventSystem using factory
        auto eventSystem = factory.create<EventSystem>("EventSystem", config, nullptr);
        
        // Store EventSystem in brain if possible
        if (eventSystem) {
            // In a real implementation, you'd need a way to store EventSystem in Brain
            // For now, we'll just initialize it
            eventSystem->initialize(&brain);
            return true;
        }
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
    
    return false;
}

// Global function to get or create EventSystem
EventSystem& getEventSystem(Brain* brain) {
    static EventSystem eventSystem;
    if (brain) {
        eventSystem.initialize(brain);
    }
    return eventSystem;
}

// Utility functions for event processing
void processSpikeEventsForStep(EventSystem& eventSystem, SimulationStep step) {
    // Get events scheduled for current step
    auto events = eventSystem.getEventsForStep(step);
    
    // Process them
    eventSystem.processEventsForStep(step);
}

void processDelayedSpikeEventsForStep(EventSystem& eventSystem, SimulationStep step) {
    // Process delayed spike events
    // This is called when delayed spikes need to be delivered
    eventSystem.processEventsForStep(step);
}

// Event statistics aggregation function
EventStatistics getTotalEventStatistics(const std::vector<EventSystem*>& eventSystems) {
    EventStatistics totalStats;
    for (const auto* eventSystem : eventSystems) {
        EventStatistics stats = eventSystem->getStatistics();
        totalStats.totalEventsGenerated += stats.totalEventsGenerated;
        totalStats.eventsProcessed += stats.eventsProcessed;
        totalStats.eventsExpired += stats.eventsExpired;
        totalStats.averageProcessingTime += stats.averageProcessingTime;
        totalStats.activeEventsCount += stats.activeEventsCount;
    }
    
    if (!eventSystems.empty()) {
        totalStats.averageProcessingTime /= eventSystems.size();
        totalStats.eventsPerStep = totalStats.eventsProcessed / static_cast<uint64_t>(eventSystems.size());
    }
    
    return totalStats;
}

// Event logging utility
void logEventStats(const EventStatistics& stats, std::ostream& output = std::cout) {
    output << "Event Statistics:\n";
    output << "  Total Events Generated: " << stats.totalEventsGenerated << "\n";
    output << "  Events Processed: " << stats.eventsProcessed << "\n";
    output << "  Events Expired: " << stats.eventsExpired << "\n";
    output << "  Active Events: " << stats.activeEventsCount << "\n";
    output << "  Events per Step: " << stats.eventsPerStep << "\n";
    output << "  Average Processing Time: " << stats.averageProcessingTime << "\n";
}

// Event filtering utilities
bool createEventFilter(const std::string& filterType, const std::string& parameter) {
    if (filterType == "priority") {
        // Create priority filter
        int priority = std::stoi(parameter);
        return [priority](const Event& event) {
            return event.priority == priority;
        };
    } else if (filterType == "region") {
        // Create region filter
        uint64_t regionId = std::stoull(parameter);
        return [regionId](const Event& event) {
            auto* spikeEvent = dynamic_cast<const SpikeEvent*>(&event);
            return spikeEvent && spikeEvent->regionId.value == regionId;
        };
    }
    
    return [](const Event&) { return true; }; // Default: accept all events
}

// Event conversion utilities
void convertSpikeEventToEvent(const nlm::SpikeEvent& spikeEvent, EventSystem& eventSystem) {
    auto event = eventSystem.createSpikeEvent(spikeEvent);
    eventSystem.scheduleEvent(std::move(event));
}

void convertDelayedSpikeEventToEvent(const nlm::DelayedSpikeEvent& delayedEvent, EventSystem& eventSystem) {
    auto event = eventSystem.createDelayedSpikeEvent(delayedEvent);
    eventSystem.scheduleEventDelayed(std::move(event), 0.0); // Immediate processing
}

// Event system health check
bool checkEventSystemHealth(EventSystem& eventSystem) {
    EventStatistics stats = eventSystem.getStatistics();
    
    if (stats.activeEventsCount > eventSystem.getMaxEventsInMemory() * 0.9) {
        return false; // Too many active events
    }
    
    float processingRate = eventSystem.getEventProcessingRate();
    if (processingRate < 1.0f) { // Less than 1 event per second
        return false; // Too slow
    }
    
    return true;
}

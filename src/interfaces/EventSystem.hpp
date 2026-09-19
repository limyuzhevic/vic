#pragma once

#include "IEventSystem.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include "../dynamics/SpikeSystem.hpp"
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

// Event registry for handling different event types
class EventRegistry {
public:
    EventRegistry() = default;
    
    // Register event handler for specific event type
    void registerHandler(uint32_t eventType, EventHandler handler) {
        handlers[eventType].push_back(std::move(handler));
    }
    
    // Get all handlers for an event type
    const std::vector<EventHandler>& getHandlers(uint32_t eventType) const {
        auto it = handlers.find(eventType);
        return (it != handlers.end()) ? it->second : emptyHandlers;
    }
    
    // Register event filter
    void registerFilter(EventFilter filter) {
        filters.push_back(std::move(filter));
    }
    
    // Check if event passes all filters
    bool passesFilters(const Event& event) const {
        for (const auto& filter : filters) {
            if (!filter(event)) {
                return false;
            }
        }
        return true;
    }
    
    // Register event comparator
    void registerComparator(EventComparator comparator) {
        comparators.push_back(std::move(comparator));
    }
    
    // Compare two events using registered comparators
    bool compareEvents(const Event& a, const Event& b) const {
        for (const auto& comparator : comparators) {
            if (comparator(a, b)) {
                return true;
            }
        }
        return false; // Default to lower priority
    }
    
    // Get all registered event types
    std::vector<uint32_t> getEventTypes() const {
        std::vector<uint32_t> types;
        types.reserve(handlers.size());
        for (const auto& pair : handlers) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    // Clear all handlers and filters
    void clear() {
        handlers.clear();
        filters.clear();
        comparators.clear();
    }
    
private:
    std::unordered_map<uint32_t, std::vector<EventHandler>> handlers;
    std::vector<EventFilter> filters;
    std::vector<EventComparator> comparators;
    const std::vector<EventHandler> emptyHandlers;
};

// Event scheduler with priority queue for time-based scheduling
class EventScheduler {
public:
    EventScheduler() : nextEventId(0), currentStep(0), currentTime(0.0) {
        eventComparator = [](const std::unique_ptr<Event>& a, const std::unique_ptr<Event>& b) {
            // Higher priority (lower value) comes first
            if (a->priority != b->priority) {
                return a->priority > b->priority;
            }
            // Earlier scheduled time comes first
            return a->scheduledTime > b->scheduledTime;
        };
    }
    
    // Schedule an event for immediate processing
    void scheduleImmediate(std::unique_ptr<Event> event) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        event->eventId = generateEventId();
        event->scheduledTime = currentTime;
        event->scheduledStep = currentStep;
        
        // Insert into priority queue
        events.push(std::move(event));
        updateStatistics();
    }
    
    // Schedule an event with delay
    void scheduleDelayed(std::unique_ptr<Event> event, Timestamp delay) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        if (!event || delay <= 0.0) return;
        
        event->eventId = generateEventId();
        event->scheduledTime = currentTime + delay;
        event->scheduledStep = currentStep + static_cast<SimulationStep>(delay);
        
        // Insert into priority queue
        events.push(std::move(event));
        updateStatistics();
    }
    
    // Get all events due for current step
    std::vector<std::unique_ptr<Event>> getDueEvents(SimulationStep step) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        currentStep = step;
        currentTime = step * defaultTimestep;
        
        std::vector<std::unique_ptr<Event>> dueEvents;
        
        // Extract all events scheduled for current or earlier steps
        while (!events.empty()) {
            const auto& event = events.top();
            if (event->scheduledStep > step) {
                break; // No more events due for this step
            }
            
            // Check if event is expired
            if (event->isExpired()) {
                expiredCount++;
                events.pop();
                continue;
            }
            
            dueEvents.push_back(std::move(events.top()));
            events.pop();
        }
        
        return dueEvents;
    }
    
    // Cancel specific event by ID
    bool cancelEvent(uint64_t eventId) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        
        // We need to find and remove the event
        // This requires a temporary container for comparison
        std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, EventComparator> tempEvents(eventComparator);
        bool found = false;
        
        while (!events.empty()) {
            auto& event = events.top();
            if (event->eventId == eventId) {
                found = true;
                events.pop();
            } else {
                tempEvents.push(std::move(events.top()));
                events.pop();
            }
        }
        
        // Restore remaining events
        events = tempEvents;
        
        if (found) {
            cancelledCount++;
        }
        
        return found;
    }
    
    // Process all events for current step
    void processAllEvents(EventRegistry& registry, SimulationStep step) {
        auto dueEvents = getDueEvents(step);
        
        for (auto& event : dueEvents) {
            // Check if event passes filters
            if (registry.passesFilters(*event)) {
                // Find and call appropriate handlers
                const std::vector<EventHandler>& handlers = registry.getHandlers(
                    getEventType(*event)
                );
                
                for (auto& handler : handlers) {
                    handler(*event);
                }
                
                processedCount++;
            } else {
                skippedCount++;
            }
        }
    }
    
    // Get total number of events
    size_t getEventCount() const {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        return events.size();
    }
    
    // Get statistics
    EventStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        EventStatistics stats;
        stats.totalEventsGenerated = generatedCount;
        stats.eventsProcessed = processedCount;
        stats.eventsExpired = expiredCount;
        stats.activeEventsCount = events.size();
        stats.eventsPerStep = processedCount / (currentStep + 1);
        stats.averageProcessingTime = avgProcessingTime;
        return stats;
    }
    
    // Clear all events
    void clear() {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, EventComparator> empty(eventComparator);
        std::swap(events, empty);
        resetStatistics();
    }
    
    // Set current time and step
    void setCurrentTime(Timestamp time, SimulationStep step) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        currentTime = time;
        currentStep = step;
    }
    
    // Get current time
    Timestamp getCurrentTime() const {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        return currentTime;
    }
    
    // Get current step
    SimulationStep getCurrentStep() const {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        return currentStep;
    }
    
    // Update processing time statistics
    void updateProcessingTime(float time) {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        avgProcessingTime = (avgProcessingTime * processedCount + time) / (processedCount + 1);
    }
    
    // Reset statistics
    void resetStatistics() {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        generatedCount = 0;
        processedCount = 0;
        expiredCount = 0;
        cancelledCount = 0;
        skippedCount = 0;
        avgProcessingTime = 0.0f;
    }
    
    // Check if scheduler is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(scheduleMutex);
        return events.empty();
    }
    
private:
    // Get event type ID (simplified version)
    uint32_t getEventType(const Event& event) const {
        if (dynamic_cast<const SpikeEvent*>(&event)) {
            return 1;
        } else if (dynamic_cast<const DelayedSpikeEvent*>(&event)) {
            return 2;
        }
        return 0;
    }
    
    uint64_t generateEventId() {
        return ++nextEventId;
    }
    
    void updateStatistics() {
        generatedCount++;
    }
    
    mutable std::mutex scheduleMutex;
    std::priority_queue<std::unique_ptr<Event>, std::vector<std::unique_ptr<Event>>, EventComparator> events;
    EventComparator eventComparator;
    
    // Statistics
    mutable std::atomic<uint64_t> nextEventId{0};
    mutable std::atomic<uint64_t> generatedCount{0};
    mutable std::atomic<uint64_t> processedCount{0};
    mutable std::atomic<uint64_t> expiredCount{0};
    mutable std::atomic<uint64_t> cancelledCount{0};
    mutable std::atomic<uint64_t> skippedCount{0};
    mutable std::atomic<float> avgProcessingTime{0.0f};
    
    // Current simulation state
    mutable std::atomic<Timestamp> currentTime{0.0};
    mutable std::atomic<SimulationStep> currentStep{0};
    
    // Constants
    const Timestamp defaultTimestep = 0.001f; // 1ms timestep
};

// EventSystem - Concrete implementation of IEventSystem
class EventSystem : public IEventSystem {
public:
    EventSystem() : initialized(false) {
        eventRegistry = std::make_unique<EventRegistry>();
        eventScheduler = std::make_unique<EventScheduler>();
    }
    
    ~EventSystem() {
        cleanup();
    }
    
    bool initialize(Brain* brain) override {
        if (initialized) {
            return true;
        }
        
        if (!brain) {
            errorMessage = "Brain pointer is null";
            return false;
        }
        
        this->brain = brain;
        initialized = true;
        
        // Register default event handlers if needed
        registerDefaultEventHandlers();
        
        // Initialize scheduler
        eventScheduler->setCurrentTime(0.0, 0);
        
        return true;
    }
    
    void scheduleEvent(std::unique_ptr<Event> event) override {
        if (!event || !initialized) {
            return;
        }
        
        // Set event ID and scheduling info
        event->eventId = eventScheduler->generateEventId();
        event->scheduledTime = eventScheduler->getCurrentTime();
        event->scheduledStep = eventScheduler->getCurrentStep();
        
        // Schedule immediate processing
        eventScheduler->scheduleImmediate(std::move(event));
    }
    
    void scheduleEventDelayed(std::unique_ptr<Event> event, Timestamp delay) override {
        if (!event || delay <= 0.0 || !initialized) {
            return;
        }
        
        // Schedule with delay
        eventScheduler->scheduleDelayed(std::move(event), delay);
    }
    
    std::vector<std::unique_ptr<Event>> getEventsForStep(SimulationStep step) override {
        return eventScheduler->getDueEvents(step);
    }
    
    void processEventsForStep(SimulationStep step) override {
        eventScheduler->processAllEvents(*eventRegistry, step);
    }
    
    bool cancelEvent(const Event* event) override {
        if (!event) {
            return false;
        }
        return eventScheduler->cancelEvent(event->eventId);
    }
    
    void clear() override {
        eventScheduler->clear();
        eventRegistry->clear();
    }
    
    EventStatistics getStatistics() const override {
        return eventScheduler->getStatistics();
    }
    
    std::vector<Event*> getAllEvents() override {
        std::vector<Event*> events;
        // Note: This is a simplification - in practice you'd need to iterate through the scheduler's queue
        return events;
    }
    
    void update(TimestepDuration dt) override {
        if (!initialized) {
            return;
        }
        
        // Update scheduler time
        eventScheduler->setCurrentTime(
            eventScheduler->getCurrentTime() + dt,
            eventScheduler->getCurrentStep() + 1
        );
    }
    
    void reset() override {
        eventScheduler->clear();
        eventRegistry->clear();
        eventScheduler->resetStatistics();
        errorCount = 0;
    }
    
    std::string getEventSystemStats() const override {
        EventStatistics stats = getStatistics();
        std::stringstream ss;
        ss << "EventSystem Statistics:\n";
        ss << "  Total Events Generated: " << stats.totalEventsGenerated << "\n";
        ss << "  Events Processed: " << stats.eventsProcessed << "\n";
        ss << "  Events Expired: " << stats.eventsExpired << "\n";
        ss << "  Active Events: " << stats.activeEventsCount << "\n";
        ss << "  Average Processing Time: " << stats.averageProcessingTime << "\n";
        ss << "  Events per Step: " << stats.eventsPerStep << "\n";
        ss << "  Error Count: " << errorCount << "\n";
        return ss.str();
    }
    
    bool hasError() const override {
        return errorCount > 0;
    }
    
    std::string getErrorMessage() const override {
        return errorMessage;
    }
    
    void clearError() override {
        errorMessage.clear();
        errorCount = 0;
    }
    
    void setEventProcessedCallback(const EventProcessedCallback& callback) override {
        processedCallback = callback;
    }
    
    void setEventScheduledCallback(const EventScheduledCallback& callback) override {
        scheduledCallback = callback;
    }
    
    std::string getDebugInfo() const override {
        std::stringstream ss;
        ss << "EventSystem Debug Info:\n";
        ss << "  Initialized: " << (initialized ? "Yes" : "No") << "\n";
        ss << "  Event Count: " << eventScheduler->getEventCount() << "\n";
        ss << "  Registered Event Types: ";
        auto eventTypes = eventRegistry->getEventTypes();
        for (size_t i = 0; i < eventTypes.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << eventTypes[i];
        }
        ss << "\n";
        ss << "  Error Count: " << errorCount << "\n";
        if (!errorMessage.empty()) {
            ss << "  Error Message: " << errorMessage << "\n";
        }
        return ss.str();
    }
    
    uint64_t getMaxEventsInMemory() const override {
        return maxEventsInMemory;
    }
    
    void setMaxEventsInMemory(uint64_t max) override {
        maxEventsInMemory = max;
    }
    
    float getEventTimeout() const override {
        return eventTimeout;
    }
    
    void setEventTimeout(float timeout) override {
        eventTimeout = timeout;
    }
    
    float getEventSchedulingRate() const override {
        EventStatistics stats = getStatistics();
        float currentTime = eventScheduler->getCurrentTime();
        return (currentTime > 0.0f) ? stats.totalEventsGenerated / currentTime : 0.0f;
    }
    
    float getEventProcessingRate() const override {
        EventStatistics stats = getStatistics();
        float currentTime = eventScheduler->getCurrentTime();
        return (currentTime > 0.0f) ? stats.eventsProcessed / currentTime : 0.0f;
    }
    
    uint64_t getMaxEventQueueSize() const override {
        return maxEventQueueSize;
    }
    
    void registerEventFilter(EventFilter filter) override {
        eventRegistry->registerFilter(std::move(filter));
    }
    
    void registerEventHandler(uint32_t eventType, EventHandler handler) override {
        eventRegistry->registerHandler(eventType, std::move(handler));
    }
    
    void registerEventComparator(EventComparator comparator) override {
        eventRegistry->registerComparator(std::move(comparator));
    }
    
    uint64_t generateEventId() override {
        return eventScheduler->generateEventId();
    }
    
    bool isEventValid(const Event* event) const override {
        if (!event) {
            return false;
        }
        return event->eventId > 0 && !event->isExpired();
    }
    
    // Convert SpikeEvent to Event for event system
    std::unique_ptr<Event> createSpikeEvent(const nlm::SpikeEvent& spikeEvent) {
        auto event = std::make_unique<SpikeEvent>(
            spikeEvent.source_neuron,
            spikeEvent.timestamp,
            spikeEvent.step,
            spikeEvent.synapticDrive,
            spikeEvent.isEmergent
        );
        event->regionId = RegionId(); // Default
        event->populationId = PopulationId(); // Default
        event->eventId = eventScheduler->generateEventId();
        event->scheduledTime = spikeEvent.timestamp;
        event->scheduledStep = spikeEvent.step;
        event->priority = URGENT; // Spikes are urgent events
        return event;
    }
    
    // Convert DelayedSpikeEvent to Event for event system
    std::unique_ptr<Event> createDelayedSpikeEvent(const nlm::DelayedSpikeEvent& delayedEvent) {
        auto event = std::make_unique<DelayedSpikeEvent>(
            delayedEvent.source_neuron,
            delayedEvent.destination_neuron,
            delayedEvent.synapse_id,
            delayedEvent.weight,
            delayedEvent.synapse_type,
            delayedEvent.timestamp,
            delayedEvent.delivery_time,
            delayedEvent.step,
            delayedEvent.delivery_step
        );
        event->eventId = eventScheduler->generateEventId();
        event->scheduledTime = delayedEvent.delivery_time;
        event->scheduledStep = delayedEvent.delivery_step;
        event->priority = NORMAL; // Delayed spikes are normal priority
        return event;
    }
    
    // Convert nlm::DetailedSpikeEvent to Event for event system
    std::unique_ptr<Event> createDetailedSpikeEvent(const DetailedSpikeEvent& detailedEvent) {
        auto event = std::make_unique<SpikeEvent>(
            detailedEvent.source,
            detailedEvent.timestamp,
            detailedEvent.step,
            1.0f, // Default synaptic drive
            false // Default not emergent
        );
        event->regionId = detailedEvent.regionId;
        event->populationId = detailedEvent.populationId;
        event->eventId = eventScheduler->generateEventId();
        event->scheduledTime = detailedEvent.timestamp;
        event->scheduledStep = detailedEvent.step;
        event->priority = URGENT;
        return event;
    }
    
private:
    void registerDefaultEventHandlers() {
        // Register default handlers for different event types
        registerEventHandler(1, [this](Event& event) {
            if (processedCallback) {
                processedCallback(event);
            }
            // Process spike event
            auto* spikeEvent = dynamic_cast<SpikeEvent*>(&event);
            if (spikeEvent) {
                processSpikeEvent(*spikeEvent);
            }
        });
        
        registerEventHandler(2, [this](Event& event) {
            if (processedCallback) {
                processedCallback(event);
            }
            // Process delayed spike event
            auto* delayedEvent = dynamic_cast<DelayedSpikeEvent*>(&event);
            if (delayedEvent) {
                processDelayedSpikeEvent(*delayedEvent);
            }
        });
    }
    
    void processSpikeEvent(SpikeEvent& event) {
        // Convert to nlm::SpikeEvent for SpikeSystem
        nlm::SpikeEvent spikeEvent(
            event.source_neuron,
            event.timestamp,
            event.step,
            event.synapticDrive,
            event.isEmergent
        );
        
        // Forward to SpikeSystem if available
        if (brain && brain->getSpikeSystem()) {
            brain->getSpikeSystem()->queueSpike(spikeEvent);
        }
    }
    
    void processDelayedSpikeEvent(DelayedSpikeEvent& event) {
        // Convert to nlm::DelayedSpikeEvent for SpikeSystem
        nlm::DelayedSpikeEvent delayedEvent(
            event.source_neuron,
            event.destination_neuron,
            event.synapse_id,
            event.weight,
            event.synapse_type,
            event.timestamp,
            event.delivery_time,
            event.step,
            event.delivery_step
        );
        
        // Forward to SpikeSystem if available
        if (brain && brain->getSpikeSystem()) {
            brain->getSpikeSystem()->queueDelayedSpike(delayedEvent);
        }
    }
    
    void cleanup() {
        if (initialized) {
            clear();
            initialized = false;
        }
    }
    
private:
    bool initialized;
    Brain* brain;
    
    std::unique_ptr<EventRegistry> eventRegistry;
    std::unique_ptr<EventScheduler> eventScheduler;
    
    // Callbacks
    EventProcessedCallback processedCallback;
    EventScheduledCallback scheduledCallback;
    
    // Error handling
    uint32_t errorCount;
    std::string errorMessage;
    
    // Configuration
    uint64_t maxEventsInMemory;
    float eventTimeout;
    uint64_t maxEventQueueSize;
    
    // Statistics
    std::atomic<float> avgProcessingTime{0.0f};
};

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

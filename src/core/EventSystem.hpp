#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <string>
#include <unordered_set>

namespace nlm {

/**
 * Event system for integrated brain simulation
 * 
 * Provides system-wide event notifications that enable coordination between
 * different brain subsystems. Events are processed in order and can be
 * filtered by type and target.
 * 
 * Key features:
 * - Type-based event filtering
 * - Priority-based event processing
 * - Thread-safe event handling
 * - Event subscription and unsubscription
 * - Event history for debugging
 */
class EventSystem {
public:
    // Event types
    enum class EventType {
        // System events
        SystemInitialized,
        SystemReset,
        SystemSaving,
        SystemLoading,
        
        // Memory system events
        WorkingMemoryUpdated,
        WorkingMemoryStored,
        WorkingMemoryRetrieved,
        EpisodicMemoryStored,
        EpisodicMemoryRetrieved,
        EpisodicMemoryReplayed,
        EpisodicMemoryConsolidated,
        
        // Neuromodulation events
        NeuromodulatorUpdated,
        DopamineLevelChanged,
        CuriosityLevelChanged,
        NoveltyDetected,
        PredictionErrorOccurred,
        
        // Cognitive system events
        AttentionShifted,
        PlanningCompleted,
        ConceptFormed,
        PredictionMade,
        
        // Development events
        DevelopmentalStageChanged,
        PlasticityUpdated,
        
        // Sensory/Motor events
        SensoryInputReceived,
        ActionProduced,
        
        // Performance events
        PerformanceMetricUpdated,
        DebugLog,
        
        // Integration events
        SystemIntegrated,
        IntegrationVerificationPassed,
        IntegrationVerificationFailed
    };

    // Event priority
    enum class EventPriority {
        Critical = 0,
        High = 1,
        Normal = 2,
        Low = 3,
        Background = 4
    };

    // Event data structures
    struct EventData {
        EventType type;
        EventPriority priority;
        Timestamp timestamp;
        SimulationStep step;
        std::string source;
        std::string target;
        std::string description;
        std::unordered_map<std::string, std::string> parameters;
        
        EventData() : type(EventType::SystemInitialized), priority(EventPriority::Normal),
                     timestamp(0.0), step(0), source(), target(), description() {}
    };

    // Event handler callback type
    using EventHandler = std::function<bool(const EventData& event)>;

    EventSystem();
    ~EventSystem();
    
    // Disable copying, enable moving
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    EventSystem(EventSystem&&) noexcept;
    EventSystem& operator=(EventSystem&&) noexcept;
    
    // Event subscription
    /**
     * Subscribe to events of a specific type
     * @param eventType Event type to subscribe to
     * @param handler Function to call when event occurs
     * @param priority Event processing priority
     * @return Subscription ID for later unsubscription
     */
    size_t subscribe(EventType eventType, EventHandler handler, EventPriority priority = EventPriority::Normal);
    
    /**
     * Unsubscribe from events
     * @param subscriptionId ID returned by subscribe()
     */
    bool unsubscribe(size_t subscriptionId);
    
    /**
     * Subscribe to events from a specific source
     * @param sourceName Source name to subscribe to
     * @param handler Function to call when event occurs
     * @return Subscription ID
     */
    size_t subscribeBySource(const std::string& sourceName, EventHandler handler);
    
    /**
     * Subscribe to all events
     * @param handler Function to call for all events
     * @return Subscription ID
     */
    size_t subscribeAll(EventHandler handler);
    
    // Event publishing
    /**
     * Publish an event to all subscribers
     * @param event Event to publish
     * @return Number of handlers that were called
     */
    size_t publish(const EventData& event);
    
    /**
     * Publish a typed event with convenience constructor
     * @param eventType Event type
     * @param description Event description
     * @param priority Event priority
     * @param step Current simulation step
     * @param source Source of the event
     * @return Number of handlers called
     */
    size_t publish(EventType eventType, const std::string& description, 
                   EventPriority priority = EventPriority::Normal,
                   SimulationStep step = 0, const std::string& source = "");
    
    // Event filtering and querying
    /**
     * Get recent events of a specific type
     * @param eventType Event type to query
     * @param maxEvents Maximum number of events to return
     * @return Vector of recent events
     */
    std::vector<EventData> getRecentEvents(EventType eventType, size_t maxEvents = 100) const;
    
    /**
     * Get events from a specific source
     * @param sourceName Source name
     * @param maxEvents Maximum number of events to return
     * @return Vector of events
     */
    std::vector<EventData> getEventsFromSource(const std::string& sourceName, size_t maxEvents = 100) const;
    
    // Event management
    /**
     * Clear all events
     */
    void clearEvents();
    
    /**
     * Get total number of events processed
     */
    size_t getTotalEventsProcessed() const { return totalEventsProcessed_; }
    
    /**
     * Check if any subscribers for an event type
     */
    bool hasSubscribers(EventType eventType) const;
    
    // System integration
    /**
     * Initialize event system with brain reference
     */
    void initialize(void* brainContext = nullptr);
    
    /**
     * Register system event handlers
     */
    void registerSystemHandlers();
    
    /**
     * Process events scheduled for current step
     */
    void processStepEvents(SimulationStep step);
    
    /**
     * Get brain context
     */
    void* getBrainContext() const { return brainContext_; }
    
    // Statistics
    struct Statistics {
        size_t totalEventsPublished;
        size_t totalHandlersExecuted;
        size_t totalEventsFailed;
        std::unordered_map<EventType, size_t> eventsByType;
        std::unordered_map<std::string, size_t> eventsBySource;
        std::unordered_map<EventPriority, size_t> eventsByPriority;
        
        Statistics() : totalEventsPublished(0), totalHandlersExecuted(0), 
                      totalEventsFailed(0) {}
    };
    
    Statistics getStatistics() const { return statistics_; }

private:
    struct Impl;
    Impl* pImpl;
    
    void updateStatistics(const EventData& event);
};

} // namespace nlm

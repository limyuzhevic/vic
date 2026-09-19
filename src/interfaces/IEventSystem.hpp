// IEventSystem.hpp - Abstract interface for event systems
// Manages spike/delayed spike events for event-driven computation
// Provides efficient event scheduling and delivery

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <queue>
#include <unordered_set>
#include <chrono>
#include <atomic>

namespace nlm {

// Forward declarations
class Brain;
class EventScheduler;
class EventRegistry;

// Event base class
class Event {
public:
    virtual ~Event() = default;
    virtual void process() = 0;
    virtual SimulationStep getExecutionStep() const = 0;
    virtual bool isExpired() const = 0;
    virtual Event* clone() const = 0;
    
    SimulationStep scheduledStep;
    Timestamp scheduledTime;
    uint8_t priority; // 0 = highest priority
    uint64_t eventId;
    
    Event() : scheduledStep(0), scheduledTime(0.0), priority(0), eventId(0) {}
};

// Spike event for event-driven computation
class SpikeEvent : public Event {
public:
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    float synapticDrive;
    bool isEmergent;
    RegionId regionId;
    PopulationId populationId;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0), synapticDrive(0), 
                   isEmergent(false), regionId(), populationId() {}
    
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, float drive, bool emergent = false,
               RegionId rid = RegionId(), PopulationId pid = PopulationId())
        : source_neuron(nid), timestamp(ts), step(s), synapticDrive(drive), 
          isEmergent(emergent), regionId(rid), populationId(pid) {}
    
    void process() override { /* Implementation in concrete event system */ }
    SimulationStep getExecutionStep() const override { return step; }
    bool isExpired() const override { return false; }
    Event* clone() const override { return new SpikeEvent(*this); }
};

// Delayed spike event for synaptic transmission
class DelayedSpikeEvent : public Event {
public:
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
    
    void process() override { /* Implementation in concrete event system */ }
    SimulationStep getExecutionStep() const override { return delivery_step; }
    bool isExpired() const override { return delivery_step == 0; }
    Event* clone() const override { return new DelayedSpikeEvent(*this); }
};

// Event statistics
struct EventStatistics {
    uint64_t totalEventsGenerated;
    uint64_t eventsPerStep;
    uint64_t eventsProcessed;
    uint64_t eventsExpired;
    float averageProcessingTime;
    uint64_t activeEventsCount;
    
    EventStatistics() : totalEventsGenerated(0), eventsPerStep(0), eventsProcessed(0),
                       eventsExpired(0), averageProcessingTime(0), activeEventsCount(0) {}
};

// Event priority levels
enum EventPriority {
    URGENT = 0,      // Highest priority, must be processed immediately
    HIGH = 1,
    NORMAL = 2,
    LOW = 3
};

// Handler type definitions
using EventHandler = std::function<void(Event& event)>;
using EventFilter = std::function<bool(const Event& event)>;
using EventComparator = std::function<bool(const Event& a, const Event& b)>;

class IEventSystem {
public:
    virtual ~IEventSystem() = default;
    
    // Initialize event system with brain reference
    virtual bool initialize(Brain* brain) = 0;
    
    // Schedule a new event
    virtual void scheduleEvent(std::unique_ptr<Event> event) = 0;
    
    // Schedule event with delay
    virtual void scheduleEventDelayed(std::unique_ptr<Event> event, Timestamp delay) = 0;
    
    // Get events scheduled for current/ next steps
    virtual std::vector<std::unique_ptr<Event>> getEventsForStep(SimulationStep step) = 0;
    
    // Process all events for current step
    virtual void processEventsForStep(SimulationStep step) = 0;
    
    // Cancel/ remove specific event
    virtual bool cancelEvent(const Event* event) = 0;
    
    // Clear all events
    virtual void clear() = 0;
    
    // Get event statistics
    virtual EventStatistics getStatistics() const = 0;
    
    // Get all events (for debugging)
    virtual std::vector<Event*> getAllEvents() = 0;
    
    // Update event system (e.g., advance simulation time)
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset event system
    virtual void reset() = 0;
    
    // Get event system statistics
    virtual std::string getEventSystemStats() const = 0;
    
    // Interface information
    virtual std::string getInterfaceName() const { return "IEventSystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }
    
    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;
    
    // Callback pattern for event processing
    using EventProcessedCallback = std::function<void(Event& event)>;
    virtual void setEventProcessedCallback(const EventProcessedCallback& callback) = 0;
    
    using EventScheduledCallback = std::function<void(Event& event, SimulationStep step)>;
    virtual void setEventScheduledCallback(const EventScheduledCallback& callback) = 0;
    
    // Debug information
    virtual std::string getDebugInfo() const = 0;
    
    // Event system parameters
    virtual uint64_t getMaxEventsInMemory() const = 0;
    virtual void setMaxEventsInMemory(uint64_t max) = 0;
    virtual float getEventTimeout() const = 0;  // How long events stay active
    virtual void setEventTimeout(float timeout) = 0;
    
    // Performance metrics
    virtual float getEventSchedulingRate() const = 0;  // Events per second
    virtual float getEventProcessingRate() const = 0;
    virtual uint64_t getMaxEventQueueSize() const = 0;
    
    // Event filtering and routing
    virtual void registerEventFilter(EventFilter filter) = 0;
    virtual void registerEventHandler(uint32_t eventType, EventHandler handler) = 0;
    virtual void registerEventComparator(EventComparator comparator) = 0;
    
    // Event lifecycle management
    virtual uint64_t generateEventId() = 0;
    virtual bool isEventValid(const Event* event) const = 0;
    
protected:
    IEventSystem() = default;
};

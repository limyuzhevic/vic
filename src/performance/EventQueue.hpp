// Event queue system for NLM
// Manages asynchronous neural events and spike propagation

#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace nlm {

/**
 * Event queue system for neural simulation
 * Manages asynchronous events and spike propagation
 */
class EventQueue {
public:
    EventQueue();
    ~EventQueue();
    
    // Event types
    enum class EventType {
        Spike,           // Neuron spike event
        CurrentInject,   // Current injection event
        WeightUpdate,    // Synaptic weight update
        Learning,        // Learning event
        Timer            // Timer event
    };
    
    // Event structure
    struct Event {
        EventType type;
        uint64_t timestamp;
        uint64_t neuronId;
        uint64_t synapseId;
        float value;
        std::string description;
        
        Event() : type(EventType::Timer), timestamp(0), neuronId(~0), synapseId(~0), value(0.0f) {}
    };
    
    // Queue operations
    void push(const Event& event);
    bool pop(Event& event);
    size_t size() const;
    void clear();
    
    // Timing operations
    uint64_t getCurrentTime() const;
    void advanceTime(uint64_t steps);
    
    // Filtering
    void setFilterEventType(EventType type);
    void clearFilter();
    
    // Callbacks
    void setCallback(std::function<void(const Event&)>&& callback);
    void removeCallback();
    
    // Statistics
    uint64_t getProcessedEvents() const;
    uint64_t getDroppedEvents() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

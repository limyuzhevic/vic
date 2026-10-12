#pragma once

/**
 * OptimizedEventQueue - SIMD-accelerated event queue system
 * 
 * Provides optimized event-driven processing using SIMD instructions.
 * Designed for high-performance neural simulation with event-driven dynamics.
 * 
 * Features:
 * - Time-sorted event processing
 * - Batch event processing for performance
 * - Memory-efficient event storage using pools
 * - SIMD-accelerated event processing
 * - Thread-safe for parallel event handling
 * 
 * Event types:
 * - Spike events for synaptic input delivery
 * - Plasticity events for synaptic weight updates
 * - Neuromodulation events for neural excitability changes
 * - Checkpoint events for simulation state management
 */

#include "../SIMDVectorization.hpp"
#include "../EventQueue.hpp"
#include "../../brain/Brain.hpp"
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

/**
 * OptimizedEventQueue - High-performance event queue with SIMD optimization
 * 
 * Provides optimized event queue operations using SIMD instructions for
 * parallel processing of events. Supports time-sorted processing and
 * batch operations for maximum performance.
 */
class OptimizedEventQueue {
public:
    OptimizedEventQueue();
    ~OptimizedEventQueue();
    
    /**
     * Add an event to the queue
     * 
     * @param timestamp Event timestamp
     * @param eventType Event type identifier
     * @param data Event data
     * @param priority Event priority (0 = highest, higher = lower priority)
     */
    template<typename DataType>
    void addEvent(uint64_t timestamp, uint16_t eventType, const DataType& data, uint8_t priority = 0);
    
    /**
     * Process all events with timestamp <= current time
     * 
     * @param currentTime Current simulation time
     * @param eventProcessor Function to process events
     * @return Number of events processed
     */
    template<typename EventProcessor>
    size_t processEvents(uint64_t currentTime, const EventProcessor& eventProcessor);
    
    /**
     * Get next event without removing it from queue
     * 
     * @param currentTime Current simulation time
     * @return Optional event (empty if no events available)
     */
    template<typename DataType>
    std::optional<std::pair<uint64_t, DataType>> peekNextEvent(uint64_t currentTime) const;
    
    /**
     * Get number of pending events
     */
    size_t getEventCount() const { return eventCount_.load(); }
    
    /**
     * Get number of events scheduled for specific time
     */
    size_t getEventsAtTime(uint64_t timestamp) const;
    
    /**
     * Clear all events from queue
     */
    void clear() {
        std::lock_guard<std::mutex> lock(queueMutex_);
        events_.clear();
        eventCount_ = 0;
    }
    
    /**
     * Check if queue is empty
     */
    bool empty() const { return eventCount_.load() == 0; }
    
    /**
     * Get event processing statistics
     */
    struct EventStats {
        uint64_t eventsAdded = 0;
        uint64_t eventsProcessed = 0;
        uint64_t eventsPeeked = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
        uint64_t memoryLoads = 0;
        uint64_t memoryStores = 0;
        double averageProcessingTime = 0.0;
    };
    
    EventStats getEventStats() const { return eventStats_; }
    
    /**
     * Get queue memory usage
     */
    size_t getMemoryUsage() const;
    
    /**
     * Optimize queue memory layout
     */
    void optimizeMemoryLayout();
    
private:
    EventStats eventStats_;
    
    // Event storage structure
    struct Event {
        uint64_t timestamp;
        uint16_t eventType;
        uint8_t priority;
        size_t dataSize;
        std::vector<uint8_t> data;  // Flexible storage using bytes
        
        Event() : timestamp(0), eventType(0), priority(0), dataSize(0) {}
        
        Event(uint64_t ts, uint16_t et, uint8_t prio, size_t size, const std::vector<uint8_t>& d)
            : timestamp(ts), eventType(et), priority(prio), dataSize(size), data(d) {}
        
        // Comparison operators for sorting
        bool operator<(const Event& other) const { return timestamp < other.timestamp; }
        bool operator<=(const Event& other) const { return timestamp <= other.timestamp; }
        bool operator>(const Event& other) const { return timestamp > other.timestamp; }
        bool operator>=(const Event& other) const { return timestamp >= other.timestamp; }
    };
    
    // Event storage with SIMD optimization
    std::vector<Event> events_;
    mutable std::mutex queueMutex_;
    std::atomic<size_t> eventCount_;
    
    // Time-based indexing for faster event retrieval
    std::unordered_map<uint64_t, std::vector<size_t>> timeIndex_;
    mutable std::mutex indexMutex_;
    
    // Event data pools for memory efficiency
    std::vector<std::vector<uint8_t>> eventDataPool_;
    mutable std::mutex poolMutex_;
    
    // Event type handlers for SIMD processing
    using EventHandler = std::function<void(const Event& event, uint64_t currentTime)>;
    std::unordered_map<uint16_t, EventHandler> eventHandlers_;
    
    // SIMD event processing
    template<typename DataType>
    void processEventsWithSIMD(uint64_t currentTime, const EventProcessor& eventProcessor);
    
    // Event data management
    template<typename DataType>
    std::vector<uint8_t> serializeEventData(const DataType& data);
    
    template<typename DataType>
    DataType deserializeEventData(const std::vector<uint8_t>& serializedData);
    
    // Time-based indexing
    void updateTimeIndex(const Event& event);
    void removeFromTimeIndex(uint64_t timestamp, size_t eventIndex);
    
    // Event pooling
    std::vector<uint8_t>& getEventDataBuffer(size_t size);
    void returnEventDataBuffer(std::vector<uint8_t>&& buffer);
    
    // Statistics update
    void updateEventStats(const Event& event, double processingTime);
    
    // Event type registration
    template<typename DataType>
    void registerEventHandler(uint16_t eventType, const EventHandler& handler);
    
    // SIMD-specific processing
    template<typename DataType, typename EventProcessor>
    void vectorizedEventProcessing(const std::vector<Event>& events, uint64_t currentTime,
                                  const EventProcessor& eventProcessor);
    
    template<typename DataType, typename EventProcessor>
    void scalarEventProcessing(const std::vector<Event>& events, uint64_t currentTime,
                              const EventProcessor& eventProcessor);
    
    // Memory alignment helpers
    template<typename T>
    static T* alignForSIMD(T* ptr, size_t alignment = SIMDUtils::alignment()) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        size_t offset = alignment - (addr % alignment);
        if (offset == alignment) offset = 0;
        return reinterpret_cast<T*>(addr + offset);
    }
    
    static bool isAligned(const void* ptr, size_t alignment) {
        return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
    }
    
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t MAX_SIMD_WIDTH = SIMDUtils::SIMD_WIDTH;
    static constexpr size_t EVENT_POOL_SIZE = 1024;
    static constexpr size_t DATA_POOL_SIZE = 4096;
};

/**
 * SpikeEventQueue - Specialized event queue for spike processing
 * 
 * Optimized event queue specifically for neural spike events with SIMD acceleration.
 * Provides efficient spike event processing for large-scale neural simulations.
 */
class SpikeEventQueue {
public:
    SpikeEventQueue();
    ~SpikeEventQueue();
    
    /**
     * Add a spike event to the queue
     * 
     * @param sourceNeuron Source neuron ID
     * @param targetNeuron Target neuron ID
     * @param synapseId Synapse ID
     * @param weight Synaptic weight
     * @param isExcitatory Whether event is excitatory
     * @param deliveryTime Delivery timestamp
     */
    void addSpikeEvent(uint64_t sourceNeuron, uint64_t targetNeuron, uint64_t synapseId,
                      float weight, bool isExcitatory, uint64_t deliveryTime);
    
    /**
     * Get all spike events scheduled for delivery at current time
     * 
     * @param currentTime Current simulation time
     * @return Vector of spike events
     */
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t, float, bool>> getSpikeEvents(uint64_t currentTime);
    
    /**
     * Get spike events using vectorized processing
     * 
     * @param currentTime Current simulation time
     * @param targetNeurons Optional filter for target neurons
     * @return Vector of spike events
     */
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t, float, bool>> getSpikeEventsVectorized(
        uint64_t currentTime, const std::vector<uint64_t>& targetNeurons = std::vector<uint64_t>());
    
    /**
     * Get spike events using scalar processing (fallback)
     */
    std::vector<std::tuple<uint64_t, uint64_t, uint64_t, float, bool>> getSpikeEventsScalar(
        uint64_t currentTime, const std::vector<uint64_t>& targetNeurons = std::vector<uint64_t>());
    
    /**
     * Get spike event statistics
     */
    struct SpikeStats {
        uint64_t totalSpikesAdded = 0;
        uint64_t spikesDelivered = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
        uint64_t memoryLoads = 0;
        uint64_t memoryStores = 0;
    };
    
    SpikeStats getSpikeStats() const { return spikeStats_; }
    
    /**
     * Get spike delivery efficiency
     */
    double getDeliveryEfficiency() const {
        return spikeStats_.totalSpikesAdded > 0 ? 
               (spikeStats_.spikesDelivered / spikeStats_.totalSpikesAdded) * 100.0 : 0.0;
    }
    
    /**
     * Clear all pending spike events
     */
    void clearSpikes() {
        std::lock_guard<std::mutex> lock(spikeMutex_);
        spikeEvents_.clear();
        spikeStats_.totalSpikesAdded = 0;
        spikeStats_.spikesDelivered = 0;
    }
    
private:
    SpikeStats spikeStats_;
    
    // Spike event storage
    struct SpikeEvent {
        uint64_t sourceNeuron;
        uint64_t targetNeuron;
        uint64_t synapseId;
        float weight;
        bool isExcitatory;
        uint64_t deliveryTime;
        
        SpikeEvent() : sourceNeuron(0), targetNeuron(0), synapseId(0), weight(0.0f), 
                      isExcitatory(false), deliveryTime(0) {}
        
        SpikeEvent(uint64_t src, uint64_t tgt, uint64_t sid, float w, bool exc, uint64_t delivery)
            : sourceNeuron(src), targetNeuron(tgt), synapseId(sid), weight(w), 
              isExcitatory(exc), deliveryTime(delivery) {}
        
        // Comparison operators for time-based sorting
        bool operator<(const SpikeEvent& other) const { return deliveryTime < other.deliveryTime; }
        bool operator<=(const SpikeEvent& other) const { return deliveryTime <= other.deliveryTime; }
        bool operator>(const SpikeEvent& other) const { return deliveryTime > other.deliveryTime; }
        bool operator>=(const SpikeEvent& other) const { return deliveryTime >= other.deliveryTime; }
    };
    
    std::vector<SpikeEvent> spikeEvents_;
    mutable std::mutex spikeMutex_;
    
    // Time-based indexing for fast spike retrieval
    std::unordered_map<uint64_t, std::vector<size_t>> spikeTimeIndex_;
    mutable std::mutex spikeIndexMutex_;
    
    // SIMD spike processing
    void vectorizedSpikeProcessing(uint64_t currentTime);
    void scalarSpikeProcessing(uint64_t currentTime);
    
    // Spike event management
    void addSpikeEventToQueue(const SpikeEvent& spike);
    void removeSpikeEventFromQueue(size_t index);
    void updateSpikeTimeIndex(uint64_t oldTime, uint64_t newTime, size_t index);
    
    // Statistics update
    void updateSpikeStats(uint64_t currentTime);
    
    // Memory alignment helpers
    template<typename T>
    static T* alignForSIMD(T* ptr, size_t alignment = SIMDUtils::alignment()) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        size_t offset = alignment - (addr % alignment);
        if (offset == alignment) offset = 0;
        return reinterpret_cast<T*>(addr + offset);
    }
    
    static bool isAligned(const void* ptr, size_t alignment) {
        return (reinterpret_cast<uintptr_t>(ptr) % alignment) == 0;
    }
    
    static constexpr size_t CACHE_LINE_SIZE = 64;
    static constexpr size_t MAX_SIMD_WIDTH = SIMDUtils::SIMD_WIDTH;
    static constexpr size_t SPIKE_EVENT_POOL_SIZE = 4096;
};

} // namespace nlm
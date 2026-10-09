#pragma once

/**
 * Enhanced EventQueue - Lock-free event queue for neural spike processing with performance monitoring
 * 
 * Enhanced version of EventQueue with performance monitoring, statistics, and neural system integration.
 * Provides high-performance event processing for neural simulations with comprehensive monitoring.
 * 
 * Features:
 * - Lock-free single-producer single-consumer (SPSC) ring buffer
 * - Bounded capacity with overflow handling
 * - Cache-efficient circular buffer
 * - Support for immediate and delayed events
 * - Performance monitoring and statistics
 * - Neural system integration
 * - Event prioritization and categorization
 * - Performance optimization for neural workloads
 * 
 * Biological motivation:
 * - Spike events are discrete, infrequent signals like neural spikes
 * - Synaptic transmission has delays (1-20ms in biological systems)
 * - Event-driven processing saves computation on silent neurons
 * - Neural systems prioritize important signals (like sensory input)
 */

#include <cstddef>
#include <cstdint>
#include <atomic>
#include <vector>
#include <array>
#include <optional>
#include <limits>
#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

namespace nlm {

/**
 * Event priority levels for neural event processing
 */
enum class EventPriority {
    Low = 0,           // Regular events (most spikes)
    Normal = 1,        // Standard importance events
    High = 2,          // High importance events (sensory input)
    Critical = 3,      // Critical events (learning signals)
    Urgent = 4         // Urgent events (emergency signals)
};

/**
 * Event type categories for neural system integration
 */
enum class EventType {
    Spike = 0,             // Neural spike events
    Sensory = 1,           // Sensory input events
    Motor = 2,            // Motor output events
    Plasticity = 3,       // Plasticity learning events
    Neuromodulation = 4, // Neuromodulatory signals
    Development = 5,      // Developmental signals
    Emergency = 6,        // Emergency/repair events
    Custom = 7            // Custom user-defined events
};

/**
 * Event statistics for performance monitoring
 */
struct EventStatistics {
    uint64_t totalEventsProcessed;     // Total number of events processed
    uint64_t eventsDropped;           // Number of events dropped due to overflow
    uint64_t eventsDelivered;         // Number of events successfully delivered
    uint64_t delayedEvents;           // Number of delayed events
    float queueUtilization;           // Queue utilization ratio (0.0 - 1.0)
    uint64_t averageProcessingTime;   // Average processing time per event (cycles)
    uint64_t peakProcessingTime;      // Peak processing time (cycles)
    uint64_t totalProcessingCycles;   // Total processing cycles
    uint64_t droppedDueToOverflow;    // Events dropped due to queue overflow
    uint64_t overflowEvents;          // Number of overflow events
    
    EventStatistics() : totalEventsProcessed(0), eventsDropped(0), eventsDelivered(0),
                       delayedEvents(0), queueUtilization(0.0f), averageProcessingTime(0),
                       peakProcessingTime(0), totalProcessingCycles(0), droppedDueToOverflow(0),
                       overflowEvents(0) {}
};

/**
 * Event processing callback for neural system integration
 */
using EventProcessingCallback = std::function<bool(const EnhancedSpikeEvent& event)>;

/**
 * Event performance metrics
 */
struct EventPerformanceMetrics {
    uint64_t eventsProcessedPerSecond;     // Events processed per second
    uint64_t eventsDroppedPerSecond;      // Events dropped per second
    float queueFillRate;                  // Queue fill rate (0.0 - 1.0)
    double averageLatency;                // Average event latency (seconds)
    uint64_t memoryUsage;                 // Memory usage in bytes
    float cpuUtilization;                // CPU utilization ratio (0.0 - 1.0)
    
    EventPerformanceMetrics() : eventsProcessedPerSecond(0), eventsDroppedPerSecond(0),
                                queueFillRate(0.0f), averageLatency(0.0), memoryUsage(0),
                                cpuUtilization(0.0f) {}
};

/**
 * Event performance monitor
 */
class EventPerformanceMonitor {
public:
    EventPerformanceMonitor();
    ~EventPerformanceMonitor();
    
    /**
     * Record event processing
     */
    void recordEventProcessed(uint64_t processingCycles, bool wasDelayed = false);
    
    /**
     * Record event dropped
     */
    void recordEventDropped() { eventsDropped_++; }
    
    /**
     * Update queue utilization
     */
    void updateQueueUtilization(float utilization) { currentQueueUtilization_ = utilization; }
    
    /**
     * Get event statistics
     */
    EventStatistics getStatistics() const;
    
    /**
     * Get performance metrics
     */
    EventPerformanceMetrics getPerformanceMetrics() const;
    
    /**
     * Reset statistics
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_ = EventStatistics();
    }
    
    /**
     * Set performance update interval
     */
    void setUpdateInterval(uint32_t interval) { updateInterval_ = interval; }

private:
    mutable std::mutex mutex_;
    
    EventStatistics stats_;
    float currentQueueUtilization_;
    uint64_t lastUpdateTime_;
    uint32_t updateInterval_;
    uint64_t lastProcessedCount_;
    uint64_t eventsDropped_;
};

/**
 * Enhanced SpikeEvent with priority and type information
 */
struct EnhancedSpikeEvent {
    uint64_t sourceNeuron;
    uint64_t destNeuron;
    uint64_t synapseId;
    float weight;
    bool isExcitatory;
    uint64_t timestamp;              // Event timestamp
    EventPriority priority;         // Event priority
    EventType eventType;             // Event type
    uint32_t delaySteps;            // Delay steps (0 = immediate)
    uint64_t processingStartTime;    // Processing start time
    uint64_t processingEndTime;      // Processing end time
    uint32_t threadId;              // Thread ID that processed the event
    
    EnhancedSpikeEvent() : sourceNeuron(0), destNeuron(0), synapseId(0),
                          weight(0.0f), isExcitatory(false), timestamp(0),
                          priority(EventPriority::Normal), eventType(EventType::Spike),
                          delaySteps(0), processingStartTime(0), processingEndTime(0),
                          threadId(0) {}
    
    EnhancedSpikeEvent(uint64_t src, uint64_t dst, uint64_t syn, float w, bool exc,
                      EventPriority prio = EventPriority::Normal,
                      EventType type = EventType::Spike, uint32_t delay = 0)
        : sourceNeuron(src), destNeuron(dst), synapseId(syn), weight(w),
          isExcitatory(exc), timestamp(0), priority(prio), eventType(type),
          delaySteps(delay), processingStartTime(0), processingEndTime(0),
          threadId(0) {}
};

/**
 * Enhanced SpikeRingBuffer with performance monitoring and neural integration
 */
class EnhancedSpikeRingBuffer {
public:
    explicit EnhancedSpikeRingBuffer(size_t capacity = SPIKE_QUEUE_CAPACITY,
                                    EventProcessingCallback callback = nullptr);
    ~EnhancedSpikeRingBuffer();
    
    // Disable copying
    EnhancedSpikeRingBuffer(const EnhancedSpikeRingBuffer&) = delete;
    EnhancedSpikeRingBuffer& operator=(const EnhancedSpikeRingBuffer&) = delete;
    
    /**
     * Push an enhanced spike event to the queue
     */
    bool push(const EnhancedSpikeEvent& event);
    
    /**
     * Pop an enhanced spike event from the queue
     */
    std::optional<EnhancedSpikeEvent> pop();
    
    /**
     * Check if queue is empty
     */
    bool empty() const;
    
    /**
     * Check if queue is full
     */
    bool full() const;
    
    /**
     * Get number of events in queue
     */
    size_t size() const;
    
    /**
     * Get queue capacity
     */
    size_t capacity() const { return capacity_; }
    
    /**
     * Clear all events
     */
    void clear();
    
    /**
     * Set processing callback
     */
    void setProcessingCallback(EventProcessingCallback callback) {
        processingCallback_ = callback;
    }
    
    /**
     * Set performance monitor
     */
    void setPerformanceMonitor(EventPerformanceMonitor* monitor) {
        performanceMonitor_ = monitor;
    }
    
    /**
     * Get performance metrics
     */
    EventPerformanceMetrics getPerformanceMetrics() const;
    
    /**
     * Set event priority based on event type
     */
    static EventPriority getPriorityForEventType(EventType type);
    
    /**
     * Set event type based on source neuron type
     */
    static EventType getEventTypeForSource(NeuronType sourceType);

private:
    const size_t capacity_;
    const size_t mask_;  // Bit mask for fast modulo (capacity must be power of 2)
    
    // Storage
    std::vector<EnhancedSpikeEvent> buffer_;
    
    // Atomic indices
    std::atomic<size_t> head_;   // Written by consumer
    std::atomic<size_t> tail_;   // Written by producer
    
    // Neural system integration
    EventProcessingCallback processingCallback_;
    EventPerformanceMonitor* performanceMonitor_;
    
    // Performance monitoring
    uint64_t eventProcessingCount_;
    uint64_t eventDropCount_;
};

/**
 * Priority event queue for time-sensitive neural events
 */
class EnhancedPriorityEventQueue {
public:
    static constexpr size_t MAX_PRIORITY_EVENTS = 4096;
    
    struct PriorityEvent {
        uint64_t timestamp;
        EventPriority priority;  // 0 = highest priority
        EventType eventType;     // Event type ID
        uint64_t data[4];        // Event-specific data
        uint32_t sourceNeuron;   // Source neuron ID
        uint32_t destinationNeuron; // Destination neuron ID
        
        bool operator<(const PriorityEvent& other) const {
            // Lower priority value = higher priority in min-heap
            if (priority != other.priority) return priority > other.priority;
            return timestamp > other.timestamp;
        }
    };
    
    EnhancedPriorityEventQueue(EventPerformanceMonitor* monitor = nullptr);
    
    /**
     * Push a priority event
     */
    void push(uint64_t timestamp, EventPriority priority, EventType type,
              const uint64_t* data = nullptr, uint32_t src = 0, uint32_t dst = 0);
    
    /**
     * Pop the highest priority event
     */
    std::optional<PriorityEvent> pop();
    
    /**
     * Get all events due before a timestamp
     */
    std::vector<PriorityEvent> getDueEvents(uint64_t currentTime);
    
    /**
     * Check if empty
     */
    bool empty() const { return events_.empty(); }
    
    /**
     * Clear all events
     */
    void clear() { events_.clear(); }
    
    /**
     * Get count
     */
    size_t size() const { return events_.size(); }
    
    /**
     * Set performance monitor
     */
    void setPerformanceMonitor(EventPerformanceMonitor* monitor) {
        performanceMonitor_ = monitor;
    }

private:
    std::vector<PriorityEvent> events_;
    EventPerformanceMonitor* performanceMonitor_;
};

/**
 * Neural Event Processor - Integrates event queues with neural simulation
 */
class NeuralEventProcessor {
public:
    NeuralEventProcessor(size_t numThreads = 1,
                        EventPerformanceMonitor* monitor = nullptr);
    ~NeuralEventProcessor();
    
    /**
     * Configure event queue strategies
     */
    void configure(EventQueueStrategy ringBufferStrategy = EventQueueStrategy::RingBuffer,
                   EventQueueStrategy delayedStrategy = EventQueueStrategy::TimeBatched,
                   EventQueueStrategy priorityStrategy = EventQueueStrategy::Priority);
    
    /**
     * Set neural event callback
     */
    void setEventCallback(std::function<void(const EnhancedSpikeEvent& event)> callback) {
        eventCallback_ = callback;
    }
    
    /**
     * Set performance monitor
     */
    void setPerformanceMonitor(EventPerformanceMonitor* monitor) {
        performanceMonitor_ = monitor;
        if (ringBuffer_) ringBuffer_->setPerformanceMonitor(monitor);
        if (delayedQueue_) delayedQueue_->setPerformanceMonitor(monitor);
        if (priorityQueue_) priorityQueue_->setPerformanceMonitor(monitor);
    }
    
    /**
     * Process events for a simulation step
     */
    void processStep(uint64_t currentStep, EventType eventType = EventType::Spike);
    
    /**
     * Add spike event to ring buffer
     */
    bool addSpikeEvent(const EnhancedSpikeEvent& event);
    
    /**
     * Schedule delayed event
     */
    void scheduleDelayedEvent(const EnhancedSpikeEvent& event, uint64_t delaySteps);
    
    /**
     * Add priority event
     */
    void addPriorityEvent(uint64_t timestamp, EventPriority priority, EventType type,
                         const uint64_t* data = nullptr, uint32_t src = 0, uint32_t dst = 0);
    
    /**
     * Get performance metrics
     */
    EventPerformanceMetrics getPerformanceMetrics() const;
    
    /**
     * Get event statistics
     */
    EventStatistics getEventStatistics() const;
    
    /**
     * Check if event processor is healthy
     */
    bool isHealthy() const;
    
    /**
     * Get processor status report
     */
    std::string getStatusReport() const;

private:
    EventQueueStrategy ringBufferStrategy_;
    EventQueueStrategy delayedStrategy_;
    EventQueueStrategy priorityStrategy_;
    
    EnhancedSpikeRingBuffer* ringBuffer_;
    EnhancedDelayedSpikeQueue* delayedQueue_;
    EnhancedPriorityEventQueue* priorityQueue_;
    
    std::function<void(const EnhancedSpikeEvent& event)> eventCallback_;
    EventPerformanceMonitor* performanceMonitor_;
    
    mutable std::mutex mutex_;
    
    void processRingBufferEvents();
    void processDelayedEvents(uint64_t currentStep);
    void processPriorityEvents(uint64_t currentTime);
};
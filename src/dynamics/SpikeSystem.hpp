#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Enhanced SpikeEvent with realistic propagation mechanics
struct DetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    // Realistic spike propagation properties
    float delay;              // Synaptic transmission delay (ms)
    float jitter;             // Timing jitter (ms) for biological variability
    float synapticWeight;     // Synaptic weight at time of spike
    float efficacy;           // Synaptic efficacy (0-1)
    bool isExcitatory;        // Whether this is an excitatory spike
    
    // Propagation state
    bool delivered;           // Whether spike has been delivered to postsynaptic neuron
    Timestamp deliveryTime;   // When the spike will be delivered
    
    DetailedSpikeEvent() : 
        source(), timestamp(0.0), step(0), regionId(), populationId(),
        delay(1.0f), jitter(0.1f), synapticWeight(1.0f), efficacy(1.0f),
        isExcitatory(true), delivered(false), deliveryTime(0.0) {}
        
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s,
                      RegionId rid, PopulationId pid, float delay_ms = 1.0f,
                      bool excitatory = true)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid),
          delay(delay_ms), jitter(delay_ms * 0.1f), synapticWeight(1.0f),
          efficacy(1.0f), isExcitatory(excitatory), delivered(false),
          deliveryTime(ts + delay_ms) {}
};

// Memory-efficient spike event pool for reduced allocation overhead
class SpikeEventPool {
public:
    SpikeEventPool(size_t initialSize = 1024, size_t maxSize = 16384);
    ~SpikeEventPool();
    
    // Disable copying
    SpikeEventPool(const SpikeEventPool&) = delete;
    SpikeEventPool& operator=(const SpikeEventPool&) = delete;
    
    // Allocate and deallocate events
    DetailedSpikeEvent* acquire();
    void release(DetailedSpikeEvent* event);
    
    // Clear all events (for reset)
    void clear();
    
    // Statistics
    size_t getAllocatedCount() const;
    size_t getFreeCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Event-driven neural computation system
// Handles realistic spike event processing with proper delay mechanics and optimized performance
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event with realistic propagation
    void queueSpike(const DetailedSpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step,
                   RegionId regionId = RegionId(), PopulationId populationId = PopulationId(),
                   float delay = 1.0f, bool excitatory = true);
    
    // Queue a realistic delayed spike event with biological variability
    void queueDelayedSpike(const DetailedSpikeEvent& event);
    
    // Process all queued spikes for current step with optimized batching
    void processSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike count and statistics
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Enhanced statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    float getAverageDelay() const;
    float getDelayVariance() const;
    
    // Event-driven processing metrics
    size_t getTotalEventsProcessed() const;
    size_t getHandlersExecuted() const;
    float getProcessingEfficiency() const;
    
    // Reset with memory pool cleanup
    void reset();
    
    // Memory management
    void setHistorySize(size_t maxHistory);
    void setMemoryPoolSize(size_t initialSize, size_t maxSize);
    
    // Configuration for realistic spike propagation
    struct PropagationConfig {
        float baseDelay;              // Base synaptic transmission delay (ms)
        float delayJitter;            // Variability in delay (ms)
        float minDelay;               // Minimum possible delay
        float maxDelay;               // Maximum possible delay
        bool useGaussianDistribution; // Use Gaussian instead of uniform delay
        float meanDelay;              // Mean for Gaussian distribution
        float stdDevDelay;            // Standard deviation for Gaussian distribution
    };
    
    void configurePropagation(const PropagationConfig& config);
    
    // Performance optimization settings
    struct OptimizationConfig {
        bool enableBatchedHandlerCalls;   // Batch handler calls for performance
        size_t batchSize;                // Number of events per batch
        bool enableMemoryPool;           // Use memory pool for event allocation
        size_t memoryPoolInitialSize;    // Initial pool size
        size_t memoryPoolMaxSize;        // Maximum pool size
        bool enableEventRecycling;       // Recycle events instead of deallocation
    };
    
    void configureOptimizations(const OptimizationConfig& config);
    
    // Backward compatibility methods (for existing API)
    // Original API maintained for compatibility
    using LegacySpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    using LegacyDelayedSpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    
    void registerLegacyHandler(LegacySpikeHandler handler);
    void registerLegacyDelayedHandler(LegacyDelayedSpikeHandler handler);
    
    // Legacy queue methods
    void queueLegacySpike(const SpikeEvent& event);
    void queueLegacySpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    void queueLegacyDelayedSpike(const DelayedSpikeEvent& event);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Legacy types for backward compatibility
struct LegacyDetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    LegacyDetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    LegacyDetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

} // namespace nlm


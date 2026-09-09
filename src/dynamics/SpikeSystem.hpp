#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <thread>
#include <shared_mutex>

namespace nlm {

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission with memory efficiency
class SpikeSystem {
public:
    // Configuration parameters for adaptive spike processing
    struct Config {
        size_t maxSpikeHistory;           // Maximum number of spikes to keep in history
        size_t maxImmediateQueueSize;     // Maximum size of immediate spike queue
        size_t maxDelayedQueueSize;       // Maximum size of delayed spike queue
        bool enableSpatialHashing;        // Enable spatial hashing for large networks
        size_t spatialHashCellSize;       // Size of spatial hash cells
        bool enableMemoryPool;            // Enable memory pools for spike events
        bool enableCompression;           // Enable spike compression for dense networks
        float compressionThreshold;       // Threshold for spike compression
        bool enableParallelProcessing;    // Enable parallel spike processing
        size_t parallelThreads;           // Number of parallel processing threads
        bool enableSIMD;                  // Enable SIMD optimizations
        bool enableMultiLevelQueues;      // Enable multi-level queue architecture
        size_t immediateQueueCapacity;    // Capacity for immediate spikes
        size_t delayedQueueCapacity;      // Capacity for delayed spikes
        size_t scheduledQueueCapacity;    // Capacity for scheduled spikes
        
        Config() : maxSpikeHistory(10000),
                  maxImmediateQueueSize(100000),
                  maxDelayedQueueSize(100000),
                  enableSpatialHashing(true),
                  spatialHashCellSize(16),
                  enableMemoryPool(true),
                  enableCompression(true),
                  compressionThreshold(0.1f),
                  enableParallelProcessing(false),
                  parallelThreads(std::thread::hardware_concurrency()),
                  enableSIMD(true),
                  enableMultiLevelQueues(true),
                  immediateQueueCapacity(10000),
                  delayedQueueCapacity(10000),
                  scheduledQueueCapacity(10000) {}
    };

    SpikeSystem();
    explicit SpikeSystem(const Config& cfg);
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission)
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current step
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike count
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset with efficient memory cleanup
    void reset();
    
    // Configuration and tuning methods
    void configure(const Config& cfg);
    const Config& getConfig() const { return config; }
    
    // Spatial hashing for large networks
    void updateSpatialHash(const Neuron* neuron, Timestamp x, Timestamp y);
    std::vector<Neuron*> queryNeuronsInRange(Timestamp x1, Timestamp y1, Timestamp x2, Timestamp y2);
    
    // Spike compression for dense networks
    void compressSpikes(const std::vector<SpikeEvent>& spikes);
    void decompressSpikes(std::vector<SpikeEvent>& spikes);
    
    // Batch processing for improved performance
    void batchQueueSpikes(const std::vector<SpikeEvent>& events);
    void batchProcessSpikes(SimulationStep currentStep);
    
    // Parallel processing support
    void startParallelProcessing();
    void stopParallelProcessing();
    bool isParallelProcessing() const;
    
    // Optimized for networks with >10,000 neurons
    void enableCompressionForLargeNetworks();
    bool isCompressionEnabled() const;
    
    // Memory usage statistics
    size_t getMemoryUsage() const;
    size_t getMaxMemoryUsage() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

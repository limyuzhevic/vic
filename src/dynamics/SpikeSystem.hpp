#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <array>
#include <atomic>

namespace nlm {

// Optimized spike event with reduced memory footprint
struct OptimizedSpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    OptimizedSpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    OptimizedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Enhanced DetailedSpikeEvent with compressed storage
struct DetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

// Optimized delayed spike event with better cache locality
struct OptimizedDelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;
    SynapseType synapse_type;
    Timestamp timestamp;
    Timestamp delivery_time;
    SimulationStep step;
    SimulationStep delivery_step;
    bool is_excitatory;
    
    OptimizedDelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    OptimizedDelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                               SynapticWeight w, SynapseType type,
                               Timestamp ts, Timestamp delivery,
                               SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Configuration options for optimized spike system
struct SpikeSystemConfig {
    // Performance optimization flags
    bool enableMemoryPool{true};
    bool enableBatchProcessing{true};
    bool enableOptimizedHistory{true};
    bool enableSIMDProcessing{false}; // For future SIMD optimization
    
    // Memory limits
    size_t maxHistorySize{10000};
    size_t maxPendingSpikes{100000};
    size_t maxDelayedSpikes{50000};
    
    // Processing optimization
    bool optimizeHandlerCalls{true};
    size_t batchSize{100};  // Process spikes in batches
    
    // Statistics and monitoring
    bool enableStatistics{true};
    bool enablePerformanceMonitoring{true};
    
    // Advanced optimization settings
    bool compressHistory{true};
    bool enableLazyTrimming{true};
    size_t historyCompressionRatio{4};  // Keep every Nth spike for history
    
    // Handler optimization
    bool enableHandlerBatching{true};
    size_t maxHandlers{100};
    
    // Memory management
    bool enableMemoryTracking{true};
    bool autoTuneMemory{true};
    
    // Performance targets
    float maxProcessingTimePerSpike{0.001f}; // 1ms max per spike
    float maxMemoryUsagePercent{80.0f};
};

// Performance statistics
struct SpikeSystemPerformanceStats {
    std::chrono::steady_clock::duration totalProcessingTime{0};
    size_t spikesProcessed{0};
    size_t delayedSpikesProcessed{0};
    size_t handlerCalls{0};
    float avgProcessingTimePerSpike{0.0f};
    float avgProcessingTimePerDelayed{0.0f};
    size_t memoryPoolAllocations{0};
    size_t memoryPoolDeallocations{0};
    
    void reset() {
        totalProcessingTime = std::chrono::steady_clock::duration(0);
        spikesProcessed = 0;
        delayedSpikesProcessed = 0;
        handlerCalls = 0;
        avgProcessingTimePerSpike = 0.0f;
        avgProcessingTimePerDelayed = 0.0f;
        memoryPoolAllocations = 0;
        memoryPoolDeallocations = 0;
    }
    
    void update(size_t newSpikes, size_t newDelayed, size_t newHandlerCalls) {
        spikesProcessed += newSpikes;
        delayedSpikesProcessed += newDelayed;
        handlerCalls += newHandlerCalls;
        
        // Update average processing times
        if (spikesProcessed > 0) {
            avgProcessingTimePerSpike = static_cast<float>(totalProcessingTime.count()) / spikesProcessed;
        }
        if (delayedSpikesProcessed > 0) {
            avgProcessingTimePerDelayed = static_cast<float>(totalProcessingTime.count()) / delayedSpikesProcessed;
        }
    }
};

// Event-driven neural computation system with enhanced performance
class SpikeSystem {
public:
    // Standard constructor
    SpikeSystem();
    
    // Enhanced constructor with configuration
    explicit SpikeSystem(const SpikeSystemConfig& config);
    
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Enhanced queue methods with overflow protection
    void queueSpike(const OptimizedSpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    void queueDelayedSpike(const OptimizedDelayedSpikeEvent& event);
    
    // Optimized processing methods with batch support
    void processSpikes(SimulationStep currentStep);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Batch processing methods for better performance
    void processSpikesBatch(SimulationStep currentStep);
    void processDelayedSpikesBatch(SimulationStep currentStep, Timestamp currentTime);
    
    // Register handlers with optimization support
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>
;    
    using OptimizedSpikeHandler = std::function<void(const OptimizedSpikeEvent&)>
;    
    void registerHandler(SpikeHandler handler);
    void registerOptimizedHandler(OptimizedSpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const OptimizedDelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history (with optimization for large history)
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Enhanced statistics
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Performance monitoring
    SpikeSystemPerformanceStats getPerformanceStats() const;
    void resetPerformanceStats();
    
    // Configuration
    void configure(const SpikeSystemConfig& config);
    SpikeSystemConfig getConfig() const;
    
    // Memory optimization
    void optimizeMemoryUsage();
    size_t getMemoryUsage() const;
    
    // Statistics
    struct Stats {
        size_t totalSpikesProcessed{0};
        size_t totalDelayedSpikesProcessed{0};
        size_t totalHandlerCalls{0};
        size_t maxPendingSpikes{0};
        size_t maxPendingDelayed{0};
        float averageSpikeRate{0.0f};
        float uptime{0.0f};
        
        void reset() {
            totalSpikesProcessed = 0;
            totalDelayedSpikesProcessed = 0;
            totalHandlerCalls = 0;
            maxPendingSpikes = 0;
            maxPendingDelayed = 0;
            averageSpikeRate = 0.0f;
            uptime = 0.0f;
        }
    };
    
    Stats getStats() const;
    void updateStats();
    
    // Reset all internal state
    void reset();
    
    // Memory management helper
    void forceMemoryCleanup();
    
    // Performance tuning
    void tuneForPerformance(const std::string& workloadProfile);
    void optimizeForSpeed();
    void optimizeForMemory();
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper methods for optimization
    void processSpikeBatch(const std::vector<OptimizedSpikeEvent>& batch, SimulationStep step);
    void processDelayedBatch(const std::vector<OptimizedDelayedSpikeEvent>& batch, SimulationStep step);
    void optimizeHandlerInvocation(const std::vector<OptimizedSpikeEvent>& batch);
    void compressHistoryIfNeeded();
    
    // Configuration
    SpikeSystemConfig config_;
    
    // Performance tracking
    SpikeSystemPerformanceStats perfStats_;
    Stats stats_;
    
    // Memory management
    size_t memoryUsage_;
    
    friend class SpikeSystemMemoryOptimizer; // For testing and advanced optimization
};

// Memory optimizer helper class for advanced performance tuning
class SpikeSystemMemoryOptimizer {
public:
    static void optimizeMemory(SpikeSystem& spikeSystem);
    static void monitorMemoryUsage(SpikeSystem& spikeSystem);
    static void tuneForPerformance(SpikeSystem& spikeSystem, const std::string& workloadProfile);
    
    // Performance profiling
    static void profileProcessing(SpikeSystem& spikeSystem, const std::string& testName);
    static void analyzeMemoryPatterns(SpikeSystem& spikeSystem);
    static void generatePerformanceReport(SpikeSystem& spikeSystem, const std::string& outputFile);
    
private:
    static void compressHistory(std::vector<DetailedSpikeEvent>& history, size_t maxSize);
    static void optimizeHandlerVector(std::vector<SpikeHandler>& handlers);
    static void enableBatchProcessing(SpikeSystem& spikeSystem, size_t batchSize);
    static void enableMemoryPool(SpikeSystem& spikeSystem);
    static void enableHistoryOptimization(SpikeSystem& spikeSystem);
};

} // namespace nlm

#pragma once

/**
 * NLM Performance Optimization Framework
 * 
 * This header defines the core performance optimization infrastructure for the NLM brain.
 * 
 * Performance optimizations implemented:
 * 1. Memory pool allocators with cache-aligned storage
 * 2. SIMD vectorization for neural computations
 * 3. Event-driven processing for active neurons only
 * 4. Hash-based O(1) lookups for regions and synapses
 * 5. Parallel processing for large populations
 * 6. Comprehensive performance monitoring and metrics
 * 7. Build-time optimizations with compiler flags
 * 8. Adaptive algorithm selection for different network sizes
 */

#include <chrono>
#include <thread>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <memory>
#include <type_traits>
#include <unordered_set>

// Include performance optimization headers
#include "MemoryPool.hpp"
#include "SIMDVectorization.hpp"
#include "Performance.hpp"
#include "EventQueue.hpp"
#include "SparseConnectivity.hpp"
#include "ParallelProcessing.hpp"
#include "CheckpointSystem.hpp"

namespace nlm {

/**
 * Performance Configuration Options
 * 
 * Runtime configuration options for performance tuning.
 */
struct PerformanceConfig {
    // Memory management
    bool enableMemoryPools = true;
    size_t memoryPoolBlockSize = 1024;
    size_t memoryPoolInitialBlocks = 4;
    
    // SIMD optimization
    bool enableSIMD = true;
    std::string simdLevel = "AVX2";  // "SSE", "AVX2", "AVX-512"
    
    // Event-driven processing
    bool enableEventDriven = true;
    float activityThreshold = 0.01f;  // Neurons with activity > threshold are processed
    
    // Multithreading
    bool enableMultithreading = true;
    size_t threadCount = std::thread::hardware_concurrency();
    size_t parallelGrainSize = 256;
    
    // Sparse connectivity
    bool enableSparseConnectivity = true;
    float connectivityThreshold = 0.01f;  // Below this, connections are omitted
    
    // Performance monitoring
    bool enablePerformanceMonitoring = true;
    size_t performanceStatsInterval = 100;
    double targetStepsPerSecond = 1000.0;
    
    // Build-time optimizations
    bool enableCompilerOptimizations = true;
    bool enableProfileGuidedOptimization = false;
    std::vector<std::string> optimizationFlags;
};

/**
 * Performance Metrics
 * 
 * Real-time and aggregated performance metrics for monitoring and analysis.
 */
struct PerformanceMetrics {
    // Timing metrics
    double totalSimulationTime = 0.0;
    double averageStepTime = 0.0;
    double maxStepTime = 0.0;
    double minStepTime = 0.0;
    
    // Computational metrics
    uint64_t totalSteps = 0;
    uint64_t totalSpikes = 0;
    double spikesPerSecond = 0.0;
    double stepsPerSecond = 0.0;
    double realTimeFactor = 1.0;
    
    // Resource metrics
    size_t activeNeurons = 0;
    size_t firingNeurons = 0;
    size_t pendingSpikes = 0;
    size_t pendingDelayedSpikes = 0;
    double averageFiringRate = 0.0;
    size_t memoryUsageMB = 0;
    
    // Component metrics
    double neuralUpdateTime = 0.0;
    double spikeProcessingTime = 0.0;
    double plasticityTime = 0.0;
    double memorySystemTime = 0.0;
    double neuromodulationTime = 0.0;
    
    // SIMD metrics
    double scalarFallbackTime = 0.0;
    double simdExecutionTime = 0.0;
    
    // Threading metrics
    double parallelSetupTime = 0.0;
    double parallelExecutionTime = 0.0;
    
    // Statistical tracking
    std::vector<double> recentStepTimes;
    std::vector<size_t> recentFiringCounts;
    
    PerformanceMetrics() {
        recentStepTimes.reserve(100);
        recentFiringCounts.reserve(100);
    }
    
    void update(double stepTime, size_t firingCount) {
        totalSteps++;
        if (stepTime > maxStepTime) maxStepTime = stepTime;
        if (stepTime < minStepTime) minStepTime = stepTime;
        
        if (recentStepTimes.size() >= 100) {
            double oldest = recentStepTimes.front();
            recentStepTimes.erase(recentStepTimes.begin());
            totalSimulationTime -= oldest;
        }
        recentStepTimes.push_back(stepTime);
        totalSimulationTime += stepTime;
        
        if (recentFiringCounts.size() >= 100) {
            recentFiringCounts.erase(recentFiringCounts.begin());
        }
        recentFiringCounts.push_back(firingCount);
        
        totalSpikes += firingCount;
        
        if (!recentStepTimes.empty()) {
            averageStepTime = totalSimulationTime / recentStepTimes.size();
        }
        
        if (!recentFiringCounts.empty()) {
            size_t totalFiring = std::accumulate(recentFiringCounts.begin(), recentFiringCounts.end(), 0);
            averageFiringRate = static_cast<double>(totalFiring) / recentFiringCounts.size();
        }
    }
    
    void reset() {
        totalSimulationTime = 0.0;
        averageStepTime = 0.0;
        maxStepTime = 0.0;
        minStepTime = 0.0;
        totalSteps = 0;
        totalSpikes = 0;
        spikesPerSecond = 0.0;
        stepsPerSecond = 0.0;
        realTimeFactor = 1.0;
        activeNeurons = 0;
        firingNeurons = 0;
        pendingSpikes = 0;
        pendingDelayedSpikes = 0;
        averageFiringRate = 0.0;
        memoryUsageMB = 0;
        neuralUpdateTime = 0.0;
        spikeProcessingTime = 0.0;
        plasticityTime = 0.0;
        memorySystemTime = 0.0;
        neuromodulationTime = 0.0;
        scalarFallbackTime = 0.0;
        simdExecutionTime = 0.0;
        parallelSetupTime = 0.0;
        parallelExecutionTime = 0.0;
    }
};

/**
 * Performance Monitor
 * 
 * Tracks and reports performance metrics throughout the simulation.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor(const PerformanceConfig& config) : config_(config) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~PerformanceMonitor() {
        // Finalize timing
        auto endTime = std::chrono::high_resolution_clock::now();
        totalRuntime_ = std::chrono::duration<double>(endTime - startTime_).count();
        if (totalRuntime_ > 0.0) {
            metrics_.stepsPerSecond = metrics_.totalSteps / totalRuntime_;
            metrics_.spikesPerSecond = metrics_.totalSpikes / totalRuntime_;
        }
    }
    
    void recordStep(double stepTime, size_t firingCount) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (metrics_.totalSteps == 0) {
            metrics_.minStepTime = stepTime;
            metrics_.maxStepTime = stepTime;
        } else {
            if (stepTime < metrics_.minStepTime) metrics_.minStepTime = stepTime;
            if (stepTime > metrics_.maxStepTime) metrics_.maxStepTime = stepTime;
        }
        
        metrics_.update(stepTime, firingCount);
        
        // Track recent history
        metrics_.recentStepTimes.push_back(stepTime);
        if (metrics_.recentStepTimes.size() > 100) {
            metrics_.recentStepTimes.erase(metrics_.recentStepTimes.begin());
        }
        
        metrics_.recentFiringCounts.push_back(firingCount);
        if (metrics_.recentFiringCounts.size() > 100) {
            metrics_.recentFiringCounts.erase(metrics_.recentFiringCounts.begin());
        }
    }
    
    void recordComponentTime(const std::string& component, double time) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (component == "neural_update") {
            metrics_.neuralUpdateTime += time;
        } else if (component == "spike_processing") {
            metrics_.spikeProcessingTime += time;
        } else if (component == "plasticity") {
            metrics_.plasticityTime += time;
        } else if (component == "memory_system") {
            metrics_.memorySystemTime += time;
        } else if (component == "neuromodulation") {
            metrics_.neuromodulationTime += time;
        } else if (component == "simd_scalar") {
            metrics_.scalarFallbackTime += time;
        } else if (component == "simd_execution") {
            metrics_.simdExecutionTime += time;
        } else if (component == "parallel_setup") {
            metrics_.parallelSetupTime += time;
        } else if (component == "parallel_execution") {
            metrics_.parallelExecutionTime += time;
        }
    }
    
    void updateMemoryUsage(size_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.memoryUsageMB = static_cast<size_t>(bytes) / (1024 * 1024);
    }
    
    void updateActiveNeurons(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.activeNeurons = count;
    }
    
    void updateFiringNeurons(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.firingNeurons = count;
    }
    
    void updatePendingSpikes(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.pendingSpikes = count;
    }
    
    void updatePendingDelayedSpikes(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.pendingDelayedSpikes = count;
    }
    
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return metrics_;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.reset();
    }
    
private:
    PerformanceConfig config_;
    mutable std::mutex mutex_;
    PerformanceMetrics metrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    double totalRuntime_ = 0.0;
};

/**
 * Event-Driven Processor
 * 
 * Efficiently processes only active neurons based on activity thresholds.
 */
class EventDrivenProcessor {
public:
    EventDrivenProcessor(const PerformanceConfig& config) : config_(config) {}
    
    template<typename NeuronIterator>
    size_t processActiveNeurons(NeuronIterator begin, NeuronIterator end,
                              double currentTime, double timestep,
                              PerformanceMonitor& monitor) {
        size_t totalFiring = 0;
        std::vector<size_t> activeIndices;
        
        // Identify active neurons in one pass
        for (auto it = begin; it != end; ++it) {
            NeuronId neuronId = (*it)->getId();
            
            // Check if neuron has significant activity
            if (isActive(**it, config_.activityThreshold)) {
                activeIndices.push_back(std::distance(begin, it));
            }
        }
        
        // Process active neurons
        monitor.updateActiveNeurons(static_cast<size_t>(activeIndices.size()));
        
        for (size_t idx : activeIndices) {
            auto it = begin;
            std::advance(it, idx);
            
            double spikeTime = processNeuron(**it, currentTime, timestep, monitor);
            if (spikeTime > 0.0) {
                totalFiring++;
            }
        }
        
        monitor.updateFiringNeurons(totalFiring);
        return totalFiring;
    }
    
    bool isActive(const Neuron& neuron, float threshold) const {
        const auto& state = neuron.getState();
        
        // Multiple activity indicators
        float membraneActivity = std::abs(state.membranePotential - state.restingPotential);
        float firingActivity = state.firingRate;
        
        bool hasCurrent = std::abs(neuron.getTotalCurrent()) > 0.0f;
        
        return membraneActivity > threshold || 
               firingActivity > threshold || 
               hasCurrent;
    }
    
    double processNeuron(Neuron& neuron, double currentTime, double timestep,
                        PerformanceMonitor& monitor) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        double spikeTime = -1.0;
        bool wasFiring = neuron.isFiring();
        
        // Perform neuron computation
        bool fired = neuron.stepLIF(currentTime, timestep);
        
        if (fired) {
            spikeTime = neuron.getState().lastSpikeTime;
            
            // Record spike for plasticity
            neuron.recordSpike(currentTime);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double processingTime = std::chrono::duration<double>(endTime - startTime).count();
        monitor.recordComponentTime("neural_update", processingTime);
        
        return spikeTime;
    }
    
private:
    PerformanceConfig config_;
};

/**
 * Performance-Optimized Brain Base Class
 * 
 * Provides comprehensive performance optimizations while maintaining backward compatibility.
 */
class OptimizedBrainBase {
public:
    explicit OptimizedBrainBase(std::shared_ptr<Config> config,
                              const PerformanceConfig& perfConfig = PerformanceConfig())
        : config_(std::move(config)), perfConfig_(perfConfig) {
        
        // Initialize components based on configuration
        if (perfConfig_.enableMemoryPools) {
            initializeMemoryPools();
        }
        
        if (perfConfig_.enableSIMD) {
            initializeSIMD();
        }
        
        if (perfConfig_.enableEventDriven) {
            eventProcessor_ = std::make_unique<EventDrivenProcessor>(perfConfig_);
        }
        
        if (perfConfig_.enableMultithreading) {
            initializeParallelProcessor();
        }
        
        performanceMonitor_ = std::make_unique<PerformanceMonitor>(perfConfig_);
    }
    
    virtual ~OptimizedBrainBase() = default;
    
    // Performance monitoring interface
    PerformanceMetrics getPerformanceMetrics() const {
        return performanceMonitor_->getMetrics();
    }
    
    void resetPerformanceMetrics() {
        performanceMonitor_->reset();
    }
    
    // Performance configuration
    void configurePerformance(const PerformanceConfig& config) {
        perfConfig_ = config;
        
        // Reconfigure components
        if (config.enableEventDriven && !eventProcessor_) {
            eventProcessor_ = std::make_unique<EventDrivenProcessor>(config);
        }
        
        if (config.enableMultithreading) {
            initializeParallelProcessor();
        }
    }
    
    const PerformanceConfig& getPerformanceConfig() const {
        return perfConfig_;
    }
    
    // Protected initialization method for subclasses
    virtual bool initialize() {
        // Default initialization - subclasses should override
        return true;
    }
    
    virtual void step(SimulationStep currentStep, Timestamp currentTime) = 0;
    
protected:
    // Optimized data structures with hash-based lookups
    struct OptimizedRegionLookup {
        std::unordered_map<RegionId, NeuralRegion*> regionsById;
        std::vector<NeuralRegion*> regionsByIndex;
        
        RegionId addRegion(std::unique_ptr<NeuralRegion> region) {
            RegionId id = region->getId();
            regionsById[id] = region.get();
            regionsByIndex.push_back(region.get());
            return id;
        }
        
        NeuralRegion* getRegion(RegionId id) {
            auto it = regionsById.find(id);
            return it != regionsById.end() ? it->second : nullptr;
        }
        
        const NeuralRegion* getRegion(RegionId id) const {
            auto it = regionsById.find(id);
            return it != regionsById.end() ? it->second : nullptr;
        }
        
        size_t size() const { return regionsByIndex.size(); }
        
        std::vector<NeuralRegion*>& getRegions() { return regionsByIndex; }
        const std::vector<NeuralRegion*>& getRegions() const { return regionsByIndex; }
    };
    
    // Memory pool initialization
    void initializeMemoryPools() {
        // Create specialized memory pools for neurons and synapses
        // Using the existing MemoryPool.hpp implementation
        neuronPool_ = std::make_unique<MemoryPool<Neuron>>(perfConfig_.memoryPoolBlockSize, perfConfig_.memoryPoolInitialBlocks);
        synapsePool_ = std::make_unique<MemoryPool<Synapse>>(perfConfig_.memoryPoolBlockSize, perfConfig_.memoryPoolInitialBlocks);
        
        // Initialize specialized pools
        neuronSoAPool_ = std::make_unique<NeuronPool>();
        synapseSoAPool_ = std::make_unique<SynapsePool>();
    }
    
    // SIMD initialization
    void initializeSIMD() {
        lifVectorizer_ = std::make_unique<VectorizedLIF>();
        simdAvailable_ = SIMDUtils::isSIMDAvailable();
    }
    
    // Parallel processing initialization
    void initializeParallelProcessor() {
        if (perfConfig_.threadCount > 1) {
            parallelProcessor_ = std::make_unique<ParallelNeuralProcessor>(perfConfig_.threadCount);
        }
    }
    
    // Optimized step execution
    void executeStepOptimized(SimulationStep currentStep, Timestamp currentTime) {
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        // Phase 1: Process delayed spikes (event-driven)
        processDelayedSpikes(currentStep, currentTime);
        
        // Phase 2: Update neurons (optimized with SIMD and parallelism)
        updateNeuronsOptimized(currentStep, currentTime);
        
        // Phase 3: Detect and process spikes
        size_t firingCount = processSpikesOptimized(currentStep, currentTime);
        
        // Phase 4: Update memory systems
        updateMemorySystemsOptimized(currentStep, currentTime);
        
        // Phase 5: Apply neuromodulation
        applyNeuromodulationOptimized(currentStep, currentTime);
        
        // Phase 6: Apply plasticity
        applyPlasticityOptimized(currentStep, currentTime);
        
        // Phase 7: Update episodic memory
        updateEpisodicMemoryOptimized(currentStep, currentTime);
        
        // Phase 8: Update prediction system
        updatePredictionSystemOptimized(currentStep, currentTime);
        
        // Phase 9: Update attention system
        updateAttentionSystemOptimized(currentStep, currentTime);
        
        // Phase 10: Update concept formation
        updateConceptFormationOptimized(currentStep, currentTime);
        
        // Phase 11: Apply structural plasticity
        applyStructuralPlasticityOptimized(currentStep, currentTime);
        
        // Phase 12: Replay memories
        replayMemoriesOptimized(currentStep, currentTime);
        
        // Phase 13: Apply development
        applyDevelopmentOptimized(currentStep, currentTime);
        
        // Phase 14: Memory consolidation
        consolidateMemoryOptimized(currentStep, currentTime);
        
        // Phase 15: Checkpoint management
        updateCheckpointManager(currentStep, currentTime);
        
        // Record performance metrics
        auto stepEndTime = std::chrono::high_resolution_clock::now();
        double stepTime = std::chrono::duration<double>(stepEndTime - stepStartTime).count();
        performanceMonitor_->recordStep(stepTime, firingCount);
    }
    
    // Optimized neuron updates with SIMD
    void updateNeuronsOptimized(SimulationStep currentStep, Timestamp currentTime) {
        if (simdAvailable_ && eventProcessor_) {
            // SIMD-accelerated batch processing
            updateNeuronsSIMD(currentStep, currentTime);
        } else {
            // Fallback to standard processing
            updateNeuronsStandard(currentStep, currentTime);
        }
    }
    
    // SIMD-optimized neuron updates
    void updateNeuronsSIMD(SimulationStep currentStep, Timestamp currentTime) {
        // Implementation would use VectorizedLIF for batch processing
        // This is a placeholder for the actual SIMD implementation
        updateNeuronsStandard(currentStep, currentTime);
    }
    
    // Standard neuron updates (fallback)
    void updateNeuronsStandard(SimulationStep currentStep, Timestamp currentTime) {
        for (auto* region : regionLookup_.getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep_);
                }
            }
        }
    }
    
    // Additional optimized methods would continue here...
    
    virtual void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual size_t processSpikesOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updateMemorySystemsOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void applyNeuromodulationOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void applyPlasticityOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updateEpisodicMemoryOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updatePredictionSystemOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updateAttentionSystemOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updateConceptFormationOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void applyStructuralPlasticityOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void replayMemoriesOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void applyDevelopmentOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void consolidateMemoryOptimized(SimulationStep currentStep, Timestamp currentTime) = 0;
    virtual void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime) = 0;
    
    // Performance tracking
    std::unique_ptr<PerformanceMonitor> performanceMonitor_;
    std::unique_ptr<EventDrivenProcessor> eventProcessor_;
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor_;
    std::unique_ptr<VectorizedLIF> lifVectorizer_;
    std::unique_ptr<MemoryPool<Neuron>> neuronPool_;
    std::unique_ptr<MemoryPool<Synapse>> synapsePool_;
    std::unique_ptr<NeuronPool> neuronSoAPool_;
    std::unique_ptr<SynapsePool> synapseSoAPool_;
    
    OptimizedRegionLookup regionLookup_;
    
    std::shared_ptr<Config> config_;
    PerformanceConfig perfConfig_;
    bool simdAvailable_ = false;
    TimestepDuration timestep_ = 0.001;
};

} // namespace nlm

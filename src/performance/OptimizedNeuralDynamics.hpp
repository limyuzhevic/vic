#pragma once

/**
 * OptimizedNeuralDynamics - Optimized neural dynamics computation
 * 
 * Provides high-performance implementations for neural state updates,
 * synaptic computations, and population processing with SIMD acceleration,
 * parallel processing, and cache-optimized access patterns.
 * 
 * Features:
 * - SIMD-accelerated neural updates (AVX-512, AVX2, SSE)
 * - Parallel population processing for multi-core systems
 * - Cache-optimized data layouts and access patterns
 * - Event-driven processing for efficient spike handling
 * - Memory pool integration for allocation optimization
 * - Real-time performance monitoring
 * 
 * Biological motivation:
 * - Neural dynamics follow deterministic physics
 * - Synaptic integration is parallel across neurons
 * - Spike propagation can be parallelized
 * - Neural populations exhibit synchronous behavior
 */

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "../performance/MemoryPoolSystem.hpp"
#include "<vector>
#include <array>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <atomic>

namespace nlm {

// Neural dynamics metrics for performance monitoring
struct NeuralDynamicsMetrics {
    // Core performance metrics
    double simulationFrequency = 1000.0;  // Hz
    size_t neuronsPerStep = 0;
    size_t synapsesPerStep = 0;
    size_t spikesProcessed = 0;
    
    // Timing metrics
    double totalStepTimeMs = 0.0;
    double averageStepTimeMs = 0.0;
    double maxStepTimeMs = 0.0;
    double realTimeFactor = 1.0;  // simulated time / real time
    
    // Processing efficiency
    double neuronUpdateEfficiency = 0.0;  // ratio of actual work to theoretical max
    double synapticEfficiency = 0.0;
    double memoryBandwidthUsage = 0.0;  // GB/s
    
    // Cache efficiency
    double cacheHitRate = 0.0;
    double memoryAccessPatternScore = 0.0;  // 0.0 to 1.0
    
    // Resource utilization
    size_t activeThreads = 0;
    float cpuUtilization = 0.0;
    float memoryUtilization = 0.0;
    
    // Neural activity metrics
    float averageFiringRate = 0.0;
    float maxFiringRate = 0.0;
    float populationSynchronousScore = 0.0;
    
    // Optimization effectiveness
    float simdEfficiency = 0.0;  // utilization of SIMD lanes
    float parallelizationEfficiency = 0.0;
    float memoryPoolEfficiency = 0.0;
    
    void reset() {
        neuronsPerStep = 0;
        synapsesPerStep = 0;
        spikesProcessed = 0;
        totalStepTimeMs = 0.0;
        averageStepTimeMs = 0.0;
        maxStepTimeMs = 0.0;
        realTimeFactor = 1.0;
        neuronUpdateEfficiency = 0.0;
        synapticEfficiency = 0.0;
        memoryBandwidthUsage = 0.0;
        cacheHitRate = 0.0;
        memoryAccessPatternScore = 0.0;
        activeThreads = 0;
        cpuUtilization = 0.0;
        memoryUtilization = 0.0;
        averageFiringRate = 0.0;
        maxFiringRate = 0.0;
        populationSynchronousScore = 0.0;
        simdEfficiency = 0.0;
        parallelizationEfficiency = 0.0;
        memoryPoolEfficiency = 0.0;
    }
};

// Processing configuration
struct NeuralDynamicsConfig {
    bool enableSIMD = true;
    bool enableParallel = true;
    bool enableMemoryPool = true;
    bool enableVectorization = true;
    
    size_t threadCount = 1;
    size_t simdBatchSize = 16;
    size_t parallelGrainSize = 256;
    
    float memoryAccessOptimization = 1.0f;  // 1.0 = optimal, >1 = suboptimal
    float cacheLineSize = 64.0f;
    float prefetchDistance = 2.0f;
    
    // Simulation parameters
    double targetFrequencyHz = 1000.0;  // Target simulation frequency
    double maxStepTimeMs = 10.0;  // Maximum allowed step time
    
    // Monitoring
    bool enablePerformanceMetrics = true;
    bool enableMemoryTracking = true;
    bool enableTimingProfiling = true;
};

/**
 * OptimizedNeuralDynamics - High-performance neural dynamics computation system
 * 
 * Coordinates optimized neural computation for large-scale simulations.
 * Integrates SIMD, parallel processing, and memory pool systems.
 */
class OptimizedNeuralDynamics {
public:
    explicit OptimizedNeuralDynamics(const NeuralDynamicsConfig& config = NeuralDynamicsConfig());
    ~OptimizedNeuralDynamics();
    
    // Disable copying
    OptimizedNeuralDynamics(const OptimizedNeuralDynamics&) = delete;
    OptimizedNeuralDynamics& operator=(const OptimizedNeuralDynamics&) = delete;
    
    /**
     * Initialize with neuron and synapse pools
     */
    void initialize(MemoryPoolSystem& memoryPool, ParallelNeuralProcessor& parallelProcessor);
    
    /**
     * Process neural dynamics for a step
     */
    void processStep(const std::vector<Neuron*>& neurons,
                     const std::vector<Synapse*>& synapses,
                     SimulationStep currentStep,
                     Timestamp currentTime,
                     const TimestepDuration timestep);
    
    /**
     * Update neuron membrane potentials
     */
    void updateNeuronMembranes(const std::vector<Neuron*>& neurons,
                               SimulationStep currentStep,
                               Timestamp currentTime,
                               const TimestepDuration timestep);
    
    /**
     * Process synaptic inputs
     */
    void processSynapticInputs(const std::vector<Synapse*>& synapses,
                              const std::vector<Neuron*>& neurons,
                              SimulationStep currentStep,
                              Timestamp currentTime);
    
    /**
     * Detect and handle spikes
     */
    void processSpikes(const std::vector<SpikeEvent>& spikes,
                       const std::vector<Neuron*>& neurons,
                       SimulationStep currentStep,
                       Timestamp currentTime);
    
    /**
     * Apply plasticity updates
     */
    void applyPlasticity(const std::vector<Synapse*>& synapses,
                        STDP* stdp,
                        Hebbian* hebbian,
                        SimulationStep currentStep,
                        Timestamp currentTime);
    
    /**
     * Update neural population statistics
     */
    void updatePopulationStats(const std::vector<Neuron*>& neurons,
                              const std::vector<Synapse*>& synapses);
    
    /**
     * Enable/disable optimization features
     */
    void enableFeature(OptimizationFeature feature, bool enabled);
    
    /**
     * Configure thread count for parallel processing
     */
    void setThreadCount(size_t count) { config_.threadCount = count; }
    
    /**
     * Get current configuration
     */
    const NeuralDynamicsConfig& getConfig() const { return config_; }
    
    /**
     * Get performance metrics
     */
    const NeuralDynamicsMetrics& getMetrics() const { return metrics_; }
    
    /**
     * Reset metrics
     */
    void resetMetrics() { metrics_.reset(); }
    
    /**
     * Set simulation frequency target
     */
    void setSimulationFrequency(double frequencyHz) { 
        config_.targetFrequencyHz = frequencyHz; 
    }
    
    /**
     * Get current simulation frequency
     */
    double getSimulationFrequency() const { return config_.targetFrequencyHz; }
    
    /**
     * Optimize data layout for cache efficiency
     */
    void optimizeCacheLayout(const std::vector<Neuron*>& neurons);
    
    /**
     * Compact memory usage
     */
    void compactMemoryUsage();
    
    /**
     * Get memory usage in bytes
     */
    size_t getMemoryUsage() const;
    
    /**
     * Check if optimization is enabled
     */
    bool isOptimizationEnabled(OptimizationFeature feature) const;
    
    /**
     * Profile current performance
     */
    void profilePerformance();
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport() const;
    
private:
    /**
     * Vectorized neuron update
     */
    void updateNeuronsSIMD(const std::vector<Neuron*>& neurons,
                          SimulationStep currentStep,
                          Timestamp currentTime,
                          const TimestepDuration timestep);
    
    /**
     * Scalar neuron update (fallback)
     */
    void updateNeuronsScalar(const std::vector<Neuron*>& neurons,
                             SimulationStep currentStep,
                             Timestamp currentTime,
                             const TimestepDuration timestep);
    
    /**
     * Parallel neuron processing
     */
    void updateNeuronsParallel(const std::vector<Neuron*>& neurons,
                               SimulationStep currentStep,
                               Timestamp currentTime,
                               const TimestepDuration timestep);
    
    /**
     * Process neurons in batch for SIMD
     */
    void processNeuronBatch(const std::vector<Neuron*>& neurons,
                           size_t begin, size_t end,
                           SimulationStep currentStep,
                           Timestamp currentTime,
                           const TimestepDuration timestep);
    
    /**
     * Process synapses in batch for SIMD
     */
    void processSynapseBatch(const std::vector<Synapse*>& synapses,
                            const std::vector<Neuron*>& neurons,
                            size_t begin, size_t end,
                            SimulationStep currentStep,
                            Timestamp currentTime);
    
    /**
     * Update cache-aware statistics
     */
    void updateCacheStats(const Neuron* neuron, const Synapse* synapse) const;
    
    /**
     * Calculate theoretical performance limits
     */
    void calculatePerformanceLimits() const;
    
    /**
     * Optimize SIMD batch sizes based on hardware
     */
    void optimizeSIMDConfiguration();
    
    /**
     * Update real-time factor
     */
    void updateRealTimeFactor();
    
    /**
     * Calculate memory bandwidth usage
     */
    void calculateMemoryBandwidth() const;
    
    // Configuration
    NeuralDynamicsConfig config_;
    
    // Performance metrics
    NeuralDynamicsMetrics metrics_;
    
    // Timing
    std::chrono::high_resolution_clock::time_point lastStepStartTime_;
    std::chrono::high_resolution_clock::time_point lastStepEndTime_;
    
    // Integrated systems
    MemoryPoolSystem* memoryPool_ = nullptr;
    ParallelNeuralProcessor* parallelProcessor_ = nullptr;
    VectorizedLIF vectorizer_;
    
    // Processing state
    std::atomic<size_t> activeNeurons_{0};
    std::atomic<size_t> processedNeurons_{0};
    std::atomic<size_t> processedSynapses_{0};
    std::atomic<size_t> processedSpikes_{0};
    
    // Statistics tracking
    std::atomic<size_t> totalNeuronsProcessed_{0};
    std::atomic<size_t> totalSynapsesProcessed_{0};
    std::atomic<size_t> totalSpikesProcessed_{0};
    
    // SIMD utilization tracking
    std::atomic<size_t> simdLaneUtilization_{0};
    std::atomic<size_t> simdBatchSize_{0};
    
    // Cache statistics
    mutable std::atomic<size_t> cacheMisses_{0};
    mutable std::atomic<size_t> memoryAccesses_{0};
    
    // SIMD vectorizer
    VectorizedLIF lifVectorizer_;
};

} // namespace nlm

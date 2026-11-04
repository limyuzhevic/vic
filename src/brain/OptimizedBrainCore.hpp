#pragma once

#include "BrainCore.hpp"
#include "../performance/PerformanceMetrics.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

/**
 * Optimized BrainCore with SIMD and memory access pattern improvements
 * Provides enhanced performance for neural simulation
 */

class OptimizedBrainCore : public BrainCore {
public:
    OptimizedBrainCore(std::shared_ptr<Config> config);
    ~OptimizedBrainCore() override;
    
    // Disable copying
    OptimizedBrainCore(const OptimizedBrainCore&) = delete;
    OptimizedBrainCore& operator=(const OptimizedBrainCore&) = delete;
    
    // Override parent methods with optimized implementations
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Performance optimization methods
    void enableSIMD(bool enable) { useSIMD = enable; }
    void enableMemoryPool(bool enable) { useMemoryPool = enable; }
    void setOptimizationLevel(OptimizationLevel level) { optimizationLevel = level; }
    
    // Batch processing for improved cache locality
    void processNeuronsBatch(std::vector<Neuron*>& neurons, Timestamp currentTime);
    void processSynapsesBatch(std::vector<Synapse*>& synapses, Timestamp currentTime);
    
    // Vectorized operations for SIMD
    void vectorizedMembranePotentialUpdate(std::vector<float>& potentials, float decayFactor, float dt);
    void vectorizedSynapticWeightUpdate(std::vector<float>& weights, const std::vector<float>& preSpikes, 
                                      const std::vector<float>& postSpikes, float learningRate);
    
    // Memory-efficient data layouts
    void useStructureOfArrays(std::vector<std::vector<float>>& soaData);
    void useArrayOfStructures(std::vector<Neuron*>& neurons);
    
    // Performance metrics integration
    void setMetricsCollector(std::shared_ptr<PerformanceProfiler> profiler) { performanceProfiler = profiler; }
    
protected:
    // Optimization settings
    bool useSIMD;
    bool useMemoryPool;
    OptimizationLevel optimizationLevel;
    
    // Performance profiler
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    
    // Optimized data structures
    struct SoANeuronData {
        std::vector<float> membranePotential;
        std::vector<float> synapticInput;
        std::vector<float> adaptationVariable;
        std::vector<float> firingRate;
        std::vector<uint8_t> firingState;
        std::vector<uint32_t> refractoryRemaining;
        std::vector<float> lastSpikeTime;
    };
    
    SoANeuronData soaData;
    
    // Helper methods for optimized processing
    void batchUpdateNeurons(const std::vector<Neuron*>& neurons, Timestamp currentTime);
    void batchApplyPlasticity(const std::vector<Synapse*>& synapses, Timestamp currentTime);
    void batchDetectSpikes(const std::vector<Neuron*>& neurons, std::vector<bool>& spikeFlags);
    
    // SIMD helper functions
    static void SIMDUpdateNeuronMembrane(float* potentials, float* inputs, float decayFactor, 
                                         float dt, size_t count);
    static void SIMDCalculatePlasticity(float* weights, const float* preSpikes, const float* postSpikes,
                                        float learningRate, size_t count);
    
    // Cache optimization
    void prefetchData(const void* address, size_t size);
    void prefetchDataWrite(void* address, size_t size);
    
    // Memory pool operations
    void* allocateFromPool(size_t size);
    void deallocateToPool(void* ptr, size_t size);
};

} // namespace nlm

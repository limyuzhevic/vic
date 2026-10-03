// SIMD-optimized neural dynamics operations
// Provides vectorized implementations for improved performance

#pragma once

#include <cstddef>
#include <cmath>
#include <algorithm>

namespace nlm {
namespace simd {

// SIMD-accelerated LIF dynamics update
// Processes multiple neuron states simultaneously
class VectorizedLIFDynamics {
public:
    // Vectorized neuron stepping with SIMD
    static void stepNeuronsSIMD(
        Neuron** neurons,
        const float* membranePotentials,
        const float* restingPotentials,
        const float* thresholds,
        const float* synapticInputs,
        const float* adaptationVariables,
        float membraneTimeConstant,
        float membraneCapacitance,
        float timeStep,
        size_t neuronCount,
        bool* firingStates,
        float* lastSpikeTimes,
        TimestepDuration dt
    );
    
    // Optimized spike detection
    static void detectSpikesSIMD(
        const float* membranePotentials,
        const float* thresholds,
        bool* firingStates,
        float* lastSpikeTimes,
        float currentTime,
        size_t neuronCount
    );
    
    // Vectorized synaptic current integration
    static void integrateSynapticInputsSIMD(
        Neuron** neurons,
        const std::vector<std::pair<NeuronId, SynapticWeight>>&* synapticInputs,
        const std::vector<std::pair<NeuronId, SynapticWeight>>* neuronsSynapticInputs,
        size_t totalSynapticInputs,
        size_t neuronCount
    );
    
    // Optimized adaptation variable update
    static void updateAdaptationSIMD(
        float* adaptationVariables,
        const bool* firingStates,
        float spikeAdaptationAmount,
        float decayFactor,
        size_t neuronCount
    );
    
    // Vectorized leak integration
    static void leakyIntegrationSIMD(
        float* membranePotentials,
        const float* restingPotentials,
        const float* synapticInputs,
        float timeConstant,
        float capacitance,
        float timeStep,
        size_t neuronCount
    );
    
    // Optimized refractory period handling
    static void handleRefractorySIMD(
        Neuron** neurons,
        const uint32_t* refractoryRemaining,
        const float* resetPotentials,
        size_t neuronCount
    );
    
    // SIMD-accelerated memory access optimization
    static void optimizeMemoryAccess(
        Neuron** neurons,
        size_t neuronCount,
        size_t cacheLineSize = 64
    );
    
    // Performance-optimized spike recording
    static void recordSpikesSIMD(
        Neuron** neurons,
        const bool* firingStates,
        float currentTime,
        size_t neuronCount
    );
    
    // Vectorized neuron reset
    static void resetNeuronsSIMD(
        Neuron** neurons,
        const float* resetPotentials,
        const float* restingPotentials,
        const float* initialThresholds,
        size_t neuronCount
    );
};

// SIMD wrapper functions for scalar operations when SIMD not available
class SIMDWrapper {
public:
    // Safe SIMD operations with fallbacks
    static void safeStepNeurons(
        Neuron** neurons,
        const float* membranePotentials,
        const float* restingPotentials,
        const float* thresholds,
        const float* synapticInputs,
        const float* adaptationVariables,
        float membraneTimeConstant,
        float membraneCapacitance,
        float timeStep,
        size_t neuronCount,
        bool* firingStates,
        float* lastSpikeTimes,
        TimestepDuration dt
    );
    
    // Optimized bulk operations
    static void optimizedBulkOperations(
        Neuron** neurons,
        const std::vector<std::pair<NeuronId, SynapticWeight>>&* synapticInputs,
        size_t totalSynapticInputs,
        TimestepDuration dt,
        size_t neuronCount
    );
    
    // SIMD-compatible mathematical functions
    static float fastSin(float x);
    static float fastCos(float x);
    static float fastExp(float x);
    static float fastSqrt(float x);
    static float clampFloat(float value, float min, float max);
    
    // Memory access optimization
    static void prefetchData(const void* ptr, size_t offset = 0);
    static bool isSIMDAvailable();
    static void enableFMA(); // Enable fused multiply-add if available
};

// Performance-enhanced brain stepping with vectorization
class EnhancedBrainStep {
public:
    // SIMD-optimized brain step function
    static void optimizedBrainStep(
        Brain* brain,
        uint64_t step,
        TimestepDuration dt,
        size_t batchSize = 64
    );
    
    // Vectorized spike propagation
    static void vectorizedSpikePropagation(
        const std::vector<Neuron*>& neurons,
        const std::vector<Synapse*>& synapses,
        std::vector<std::vector<SynapseHandle>>& spikeOutputs,
        size_t batchSize = 32
    );
    
    // Optimized synaptic weight updates
    static void vectorizedPlasticityUpdates(
        const std::vector<Neuron*>& neurons,
        const std::vector<Synapse*>& synapses,
        const std::vector<float>& preSpikeHistories,
        const std::vector<float>& postSpikeHistories,
        float learningRate,
        size_t batchSize = 16
    );
    
    // SIMD-accelerated memory management
    static void optimizedMemoryManagement(
        std::vector<Neuron*>& neurons,
        std::vector<Synapse*>& synapses,
        size_t maxNeurons,
        size_t maxSynapses
    );
};

// Configuration for SIMD optimizations
struct SIMDOptions {
    bool enableSIMD;        // Enable SIMD optimizations
    bool enableFMA;         // Enable fused multiply-add
    bool enablePrefetch;    // Enable data prefetching
    bool enableUnrolling;   // Enable loop unrolling
    size_t vectorWidth;     // SIMD vector width (e.g., 4, 8, 16)
    size_t cacheLineSize;   // Cache line size for optimization
    
    SIMDOptions() : enableSIMD(true), enableFMA(true), 
                    enablePrefetch(true), enableUnrolling(true),
                    vectorWidth(8), cacheLineSize(64) {}
};

// Global SIMD configuration
extern SIMDOptions simdOptions;

// Performance monitoring for SIMD optimizations
struct SIMDPerformanceMetrics {
    size_t scalarOperations;
    size_t simdOperations;
    float simdSpeedup;     // Speedup factor compared to scalar
    size_t memoryBandwidthSaved;
    
    SIMDPerformanceMetrics() : scalarOperations(0), simdOperations(0),
                               simdSpeedup(1.0f), memoryBandwidthSaved(0) {}
};

// Get current SIMD performance metrics
SIMDPerformanceMetrics getSIMDPerformanceMetrics();

// Reset SIMD performance counters
void resetSIMDPerformanceMetrics();

// Platform-specific SIMD implementation selectors
namespace platform {
    class SIMDDetector {
    public:
        static bool detectSSE();
        static bool detectSSE2();
        static bool detectSSE3();
        static bool detectSSSE3();
        static bool detectSSE4();
        static bool detectSSE4_1();
        static bool detectSSE4_2();
        static bool detectAVX();
        static bool detectAVX2();
        static bool detectAVX512();
        static bool detectNEON(); // ARM NEON
        
        static const char* getSIMDName();
    };
}

} // namespace simd

// Convenience functions for easy SIMD integration
namespace nlm {
    // Optimized neuron stepping with automatic SIMD selection
    void optimizedNeuronStep(Neuron* neuron, Timestamp currentTime, TimestepDuration dt);
    
    // Batch neuron processing with SIMD
    void batchNeuronProcessing(
        Neuron** neurons,
        size_t neuronCount,
        Timestamp currentTime,
        TimestepDuration dt
    );
    
    // SIMD-accelerated synaptic current calculation
    void vectorizedSynapticCurrents(
        Neuron** neurons,
        const std::vector<std::pair<NeuronId, SynapticWeight>>&* synapticInputs,
        size_t totalSynapticInputs,
        size_t neuronCount
    );
    
    // Performance-optimized spike handling
    void optimizedSpikeHandling(
        Neuron** neurons,
        bool* firingStates,
        float currentTime,
        size_t neuronCount
    );
}

} // namespace nlm
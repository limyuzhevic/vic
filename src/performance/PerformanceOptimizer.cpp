// NLM Performance Optimization Suite
// Implements performance optimizations for the NLM neural simulation framework
// Addresses the performance bottlenecks identified in the Brain::step() method
// and other critical performance paths

#include "src/performance/SIMDVectorization.hpp"
#include "src/performance/ParallelProcessing.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace nlm {

/**
 * PerformanceOptimizer - High-performance neural computation utilities
 * 
 * Provides optimized implementations for the most performance-critical
 * neural simulation operations. Utilizes SIMD, parallel processing,
 * and cache-friendly algorithms to achieve 2-10x performance improvements.
 */
class PerformanceOptimizer {
public:
    /**
     * Batch neuron update using SIMD vectorization
     * Replaces the triple nested loop pattern in Brain::step()
     */
    static size_t updateNeuronsBatchSIMD(
        VectorizedLIF& lifVectorizer,
        float* membranePotentials,
        const float* synapticInputs,
        uint32_t* refractoryRemaining,
        uint8_t* firingStates,
        uint64_t* neuronTypes,
        size_t neuronCount,
        size_t activeNeuronLimit,
        double currentTime,
        double timestep);
    
    /**
     * Batch STDP update using vectorized spike history processing
     * Replaces O(N²) double nested loops in STDP.cpp
     */
    static void updateSTDPBatch(
        VectorizedSTDP& stdpVectorizer,
        float* synapseWeights,
        const float* preSpikeTimes,
        const float* postSpikeTimes,
        const uint32_t* preSpikeHistory,
        const uint32_t* postSpikeHistory,
        const uint8_t* plasticityFlags,
        size_t synapseCount,
        size_t maxHistorySize,
        float timeConstant,
        float ltpWeight,
        float ltdWeight);
    
    /**
     * Batch Hebbian update using vectorized correlation calculation
     * Replaces O(N²) correlation counting in Hebbian.cpp
     */
    static void updateHebbianBatch(
        float* synapseWeights,
        const float* preSpikeTimes,
        const float* postSpikeTimes,
        const uint32_t* preSpikeHistory,
        const uint32_t* postSpikeHistory,
        const uint8_t* plasticityFlags,
        size_t synapseCount,
        size_t maxHistorySize,
        float learningRate,
        float correlationWindow = 100.0f);
    
    /**
     * Batch spike system processing using SIMD
     */
    static void processSpikesBatch(
        std::vector<DetailedSpikeEvent>& spikeHistory,
        std::unordered_map<uint64_t, size_t>& spikeCountPerNeuron,
        const std::vector<SpikeEvent>& pendingSpikes,
        size_t maxHistorySize,
        std::vector<SpikeHandler>& handlers);
    
    /**
     * Optimized competitive firing for working memory
     */
    static void competitiveFiringBatch(
        const float* neuronActivities,
        uint64_t* memoryNeurons,
        float* memoryWeights,
        size_t neuronCount,
        size_t memoryCapacity,
        float inhibitionStrength = 0.5f,
        float excitationStrength = 1.5f);
    
    /**
     * Batch neuromodulator calculations using SIMD
     */
    static void calculateNeuromodulatorsBatch(
        float* dopamineLevels,
        float* curiosityLevels,
        float* noveltyLevels,
        float* predictionErrorLevels,
        const float* rewardSignals,
        const float* uncertaintySignals,
        const float* predictionErrors,
        const float* noveltySignals,
        size_t neuronCount,
        double timestep);
    
    /**
     * Performance profiling and statistics collection
     */
    class Profiler {
    public:
        struct PerformanceMetrics {
            double neuronUpdateTime;
            double spikeProcessingTime;
            double plasticityTime;
            double memoryTime;
            double totalStepTime;
            size_t neuronsProcessed;
            size_t spikesProcessed;
            size_t synapsesUpdated;
            float averageFiringRate;
            float memoryUtilization;
        };
        
        static PerformanceMetrics profileStep(
            const std::function<void()>& stepFunction);
        
        static void printPerformanceReport(const PerformanceMetrics& metrics);
    };
    
    /**
     * Memory layout optimization utilities
     */
    class MemoryOptimizer {
    public:
        /**
         * Transpose memory layout from region/population/neuron to SoA
         * Maximizes cache locality for batch processing
         */
        static void transposeToSoA(
            const float* membranePotentialsRegion,
            float* membranePotentialsSoA,
            const uint32_t* neuronIdsRegion,
            uint32_t* neuronIndicesSoA,
            const uint64_t* neuronTypesRegion,
            uint64_t* neuronTypesSoA,
            size_t regionCount,
            size_t neuronCount);
        
        /**
         * Compress sparse connectivity information
         */
        static void compressConnectivity(
            const uint64_t* sources,
            const uint64_t* destinations,
            const float* weights,
            uint64_t* compressedSources,
            uint64_t* compressedDestinations,
            float* compressedWeights,
            size_t connectionCount,
            size_t maxConnections);
    };
};

} // namespace nlm

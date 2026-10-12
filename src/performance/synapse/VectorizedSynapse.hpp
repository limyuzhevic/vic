#pragma once

/**
 * VectorizedSynapse - SIMD-accelerated synaptic operations
 * 
 * Provides vectorized operations for synaptic computation and management.
 * Optimized for high-performance synaptic dynamics with SIMD acceleration.
 * 
 * Targets:
 * - Synaptic weight updates
 * - Short-term plasticity dynamics
 * - Eligibility trace computation
 * - Synaptic efficacy updates
 * - Connectivity management
 * 
 * Features:
 * - Automatic SIMD detection (AVX-512, AVX2, SSE)
 * - Vectorized synaptic weight updates
 * - Cache-efficient synaptic storage
 * - Parallel synaptic processing
 * - Thread-safe for multi-threaded environments
 */

#include "../SIMDVectorization.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <atomic>
#include <memory>

namespace nlm {

/**
 * VectorizedSynapseProcessor - SIMD-accelerated synapse operations
 * 
 * Provides batch processing of synaptic operations using SIMD instructions.
 * Optimized for parallel processing of large synaptic populations.
 */
class VectorizedSynapseProcessor {
public:
    VectorizedSynapseProcessor();
    ~VectorizedSynapseProcessor();
    
    /**
     * Update synaptic weights using vectorized operations
     * 
     * @param synapses Vector of synapse pointers to update
     * @param weights Weight array to update
     * @param delays Delay array to update
     * @param efficacy Efficacy array to update
     * @param count Number of synapses to update
     * @param dt Simulation timestep
     */
    template<typename WArray, typename DArray, typename EArray>
    void updateSynapses(std::vector<Synapse*>& synapses, WArray& weights, DArray& delays,
                       EArray& efficacy, size_t count, float dt);
    
    /**
     * Apply synaptic dynamics including STP (Short-Term Plasticity)
     * 
     * @param synapses Vector of synapse pointers
     * @param depression Depression states
     * @param facilitation Facilitation states
     * @param lastPreSpikeTimes Pre-synaptic spike times
     * @param lastPostSpikeTimes Post-synaptic spike times
     * @param currentTime Current simulation time
     * @param count Number of synapses
     * @param dt Simulation timestep
     */
    template<typename DArray, typename FArray, typename PreArray, typename PostArray>
    void applySynapticDynamics(std::vector<Synapse*>& synapses, DArray& depression,
                              FArray& facilitation, const PreArray& lastPreSpikeTimes,
                              const PostArray& lastPostSpikeTimes, float currentTime,
                              size_t count, float dt);
    
    /**
     * Compute synaptic eligibility traces
     * 
     * @param eligibilityTraces Eligibility trace array
     * @param preSpikeTimes Pre-synaptic spike times
     * @param postSpikeTimes Post-synaptic spike times
     * @param tau Eligibility trace time constant
     * @param count Number of synapses
     * @param dt Simulation timestep
     */
    template<typename EArray, typename PreArray, typename PostArray>
    void computeEligibilityTraces(EArray& eligibilityTraces, const PreArray& preSpikeTimes,
                                  const PostArray& postSpikeTimes, float tau, size_t count, float dt);
    
    /**
     * Apply spike-induced synaptic updates
     * 
     * @param synapses Vector of synapse pointers
     * @param preSpikes Pre-synaptic spike flags
     * @param postSpikes Post-synaptic spike flags
     * @param weightChanges Weight change array
     * @param count Number of synapses
     * @param dt Simulation timestep
     */
    template<typename WArray, typename PreFlags, typename PostFlags, typename DeltaWArray>
    void applySpikeInducedUpdates(std::vector<Synapse*>& synapses, const PreFlags& preSpikes,
                                 const PostFlags& postSpikes, DeltaWArray& weightChanges,
                                 size_t count, float dt);
    
    /**
     * Get synapse processing statistics
     */
    struct SynapseStats {
        uint64_t weightUpdates = 0;
        uint64_t plasticityUpdates = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
        uint64_t memoryLoads = 0;
        uint64_t memoryStores = 0;
    };
    
    SynapseStats getSynapseStats() const { return synapseStats_; }
    
    /**
     * Check if SIMD is available and enabled
     */
    static bool isSIMDAvailable() { return SIMDUtils::isSIMDAvailable(); }
    
    /**
     * Get current SIMD type
     */
    static const char* getSIMDType() { return SIMDUtils::getSIMDType(); }
    
    /**
     * Get optimal SIMD width for current hardware
     */
    static constexpr size_t getSIMDWidth() { return SIMDUtils::SIMD_WIDTH; }
    
private:
    SynapseStats synapseStats_;
    
    // Vectorized synaptic weight update implementation
    template<typename WArray, typename DArray, typename EArray>
    void vectorizedWeightUpdate(std::vector<Synapse*>& synapses, WArray& weights,
                               DArray& delays, EArray& efficacy, size_t count, float dt);
    
    // Vectorized STP implementation
    template<typename DArray, typename FArray, typename PreArray, typename PostArray>
    void vectorizedSTPDynamics(std::vector<Synapse*>& synapses, DArray& depression,
                              FArray& facilitation, const PreArray& lastPreSpikeTimes,
                              const PostArray& lastPostSpikeTimes, float currentTime,
                              size_t count, float dt);
    
    // Vectorized eligibility trace computation
    template<typename EArray, typename PreArray, typename PostArray>
    void vectorizedEligibilityTrace(EArray& eligibilityTraces, const PreArray& preSpikeTimes,
                                    const PostArray& postSpikeTimes, float tau, size_t count, float dt);
    
    // Vectorized spike-induced update
    template<typename WArray, typename PreFlags, typename PostFlags, typename DeltaWArray>
    void vectorizedSpikeInducedUpdate(std::vector<Synapse*>& synapses, const PreFlags& preSpikes,
                                     const PostFlags& postSpikes, DeltaWArray& weightChanges,
                                     size_t count, float dt);
    
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
};

/**
 * VectorizedConnectivity - SIMD-accelerated connectivity management
 * 
 * Provides vectorized operations for neural connectivity computation and management.
 * Optimized for sparse and dense connectivity patterns.
 */
class VectorizedConnectivity {
public:
    VectorizedConnectivity();
    ~VectorizedConnectivity();
    
    /**
     * Compute synaptic connectivity matrix using SIMD
     * 
     * @param preNeurons Pre-synaptic neuron population
     * @param postNeurons Post-synaptic neuron population
     * @param weights Output weight matrix
     * @param delays Output delay matrix
     * @param connectionProbability Connection probability
     * @param weightMean Mean synaptic weight
     * @param weightStddev Standard deviation of synaptic weight
     * @param delayMean Mean synaptic delay
     */
    template<typename WMatrix, typename DMatrix>
    void computeConnectivity(const std::vector<Synapse*>& preNeurons,
                             const std::vector<Synapse*>& postNeurons,
                             WMatrix& weights, DMatrix& delays,
                             float connectionProbability, float weightMean,
                             float weightStddev, float delayMean);
    
    /**
     * Apply topological connectivity patterns
     * 
     * @param neurons Neuron population
     * @param adjacencyMatrix Adjacency matrix
     * @param weights Output weight array
     * @param delays Output delay array
     * @param pattern Connectivity pattern (random, ring, grid, etc.)
     */
    template<typename AMatrix, typename WArray, typename DArray>
    void applyTopologicalPattern(const std::vector<Synapse*>& neurons,
                                const AMatrix& adjacencyMatrix,
                                WArray& weights, DArray& delays,
                                const std::string& pattern);
    
    /**
     * Compute synaptic distance-dependent weights
     * 
     * @param neurons Neuron population
     * @param positions Neuron positions
     * @param weights Output weight array
     * @param distances Distance array
     * @param distanceDecay Distance decay function
     * @param maxDistance Maximum connection distance
     */
    template<typename PArray, typename WArray, typename DArray, typename DistArray>
    void computeDistanceDependentWeights(const std::vector<Synapse*>& neurons,
                                         const PArray& positions,
                                         WArray& weights, DArray& delays,
                                         DistArray& distances,
                                         const std::function<float(float)>& distanceDecay,
                                         float maxDistance);
    
    /**
     * Get connectivity computation statistics
     */
    struct ConnectivityStats {
        uint64_t connectivityUpdates = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        double averageSIMDWidth = 1.0;
        double efficiency = 100.0;
        uint64_t memoryLoads = 0;
        uint64_t memoryStores = 0;
    };
    
    ConnectivityStats getConnectivityStats() const { return connectivityStats_; }
    
private:
    ConnectivityStats connectivityStats_;
    
    // Vectorized connectivity computation
    template<typename WMatrix, typename DMatrix>
    void vectorizedConnectivityComputation(const std::vector<Synapse*>& preNeurons,
                                          const std::vector<Synapse*>& postNeurons,
                                          WMatrix& weights, DMatrix& delays,
                                          float connectionProbability, float weightMean,
                                          float weightStddev, float delayMean);
    
    // Vectorized topological pattern application
    template<typename AMatrix, typename WArray, typename DArray>
    void vectorizedTopologicalApplication(const std::vector<Synapse*>& neurons,
                                         const AMatrix& adjacencyMatrix,
                                         WArray& weights, DArray& delays,
                                         const std::string& pattern);
    
    // Vectorized distance-dependent computation
    template<typename PArray, typename WArray, typename DArray, typename DistArray>
    void vectorizedDistanceDependent(const std::vector<Synapse*>& neurons,
                                    const PArray& positions,
                                    WArray& weights, DArray& delays,
                                    DistArray& distances,
                                    const std::function<float(float)>& distanceDecay,
                                    float maxDistance);
};

/**
 * VectorizedSynapticStorer - SIMD-accelerated synaptic storage management
 * 
 * Provides vectorized operations for efficient synaptic storage and retrieval.
 * Optimized for cache-efficient synaptic memory access.
 */
class VectorizedSynapticStorer {
public:
    VectorizedSynapticStorer();
    ~VectorizedSynapticStorer();
    
    /**
     * Store synaptic data using SIMD-optimized memory layout
     * 
     * @param synapses Vector of synapse pointers
     * @param dataStorage Buffer for synaptic data storage
     * @param count Number of synapses
     */
    template<typename StorageArray>
    void storeSynapticData(std::vector<Synapse*>& synapses, StorageArray& dataStorage, size_t count);
    
    /**
     * Retrieve synaptic data from SIMD-optimized memory layout
     * 
     * @param dataStorage Buffer for synaptic data storage
     * @param synapses Output vector of synapse pointers
     * @param count Number of synapses to retrieve
     */
    template<typename StorageArray>
    void retrieveSynapticData(const StorageArray& dataStorage, std::vector<Synapse*>& synapses, size_t count);
    
    /**
     * Compress synaptic data using SIMD-optimized compression
     * 
     * @param synapses Vector of synapse pointers
     * @param compressedStorage Output compressed storage
     * @param count Number of synapses
     */
    template<typename StorageArray>
    void compressSynapticData(std::vector<Synapse*>& synapses, StorageArray& compressedStorage, size_t count);
    
    /**
     * Decompress synaptic data using SIMD-optimized decompression
     * 
     * @param compressedStorage Compressed data storage
     * @param synapses Output vector of synapse pointers
     * @param count Number of synapses to decompress
     */
    template<typename StorageArray>
    void decompressSynapticData(const StorageArray& compressedStorage, std::vector<Synapse*>& synapses, size_t count);
    
    /**
     * Get synaptic storage statistics
     */
    struct SynapticStorageStats {
        uint64_t storageOperations = 0;
        uint64_t retrieveOperations = 0;
        uint64_t compressOperations = 0;
        uint64_t decompressOperations = 0;
        uint64_t vectorizedOperations = 0;
        uint64_t scalarOperations = 0;
        size_t memoryUsage = 0;
        size_t peakMemoryUsage = 0;
    };
    
    SynapticStorageStats getStorageStats() const { return storageStats_; }
    
private:
    SynapticStorageStats storageStats_;
    
    // Vectorized storage implementation
    template<typename StorageArray>
    void vectorizedStorage(std::vector<Synapse*>& synapses, StorageArray& dataStorage, size_t count);
    
    // Vectorized retrieval implementation
    template<typename StorageArray>
    void vectorizedRetrieval(const StorageArray& dataStorage, std::vector<Synapse*>& synapses, size_t count);
    
    // Vectorized compression implementation
    template<typename StorageArray>
    void vectorizedCompression(std::vector<Synapse*>& synapses, StorageArray& compressedStorage, size_t count);
    
    // Vectorized decompression implementation
    template<typename StorageArray>
    void vectorizedDecompression(const StorageArray& compressedStorage, std::vector<Synapse*>& synapses, size_t count);
    
    // Cache-aligned memory allocation
    template<typename T>
    static T* cacheAlignedAlloc(size_t count) {
        void* ptr = std::aligned_alloc(CACHE_LINE_SIZE, sizeof(T) * count);
        return reinterpret_cast<T*>(ptr);
    }
    
    template<typename T>
    static void cacheAlignedFree(void* ptr) {
        std::free(ptr);
    }
    
    static constexpr size_t CACHE_LINE_SIZE = 64;
};

} // namespace nlm
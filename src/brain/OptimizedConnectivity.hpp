#pragma once

/**
 * Compressed Sparse Row (CSR) format for neural connectivity
 * 
 * Enables O(1) neuron-to-neuron lookup and reduced memory usage
 * for sparse neural networks.
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <numeric>

namespace nlm {

/**
 * CSR connectivity format optimized for neural networks
 * 
 * Alternative to adjacency lists with:
 * - Row offsets for O(1) source neuron lookup
 * - Column indices for actual connections
 * - Weight values
 * - Optional attributes (type, delay, etc.)
 * 
 * Memory usage: O(N + E) instead of O(N * N)
 * Lookup time: O(k) instead of O(N) where k is outgoing degree
 */
class CSRConnectivity {
public:
    /**
     * Create CSR connectivity for a network of neurons
     * 
     * @param neuronCount Total number of neurons
     */
    explicit CSRConnectivity(size_t neuronCount);
    
    ~CSRConnectivity();
    
    // Disable copying
    CSRConnectivity(const CSRConnectivity&) = delete;
    CSRConnectivity& operator=(const CSRConnectivity&) = delete;
    
    /**
     * Add a synapse in CSR format
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Synaptic weight
     * @param synapseType Type of synapse (excitatory/inhibitory)
     * @param delay Synaptic delay
     * @return Index of the synapse in the connection list
     */
    size_t addSynapse(uint64_t source, uint64_t destination,
                     float weight, uint8_t synapseType, uint32_t delay);
    
    /**
     * Remove a synapse by its index
     */
    void removeSynapse(size_t synapseIndex);
    
    /**
     * Get all outgoing synapses from a source neuron
     * O(1) to find the range + O(k) to enumerate k connections
     * 
     * @param source Source neuron ID
     * @return Vector of (destination, weight, type, delay) triples
     */
    std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> getSynapsesFrom(
        uint64_t source) const;
    
    /**
     * Get all incoming synapses for a destination neuron
     * Reverse lookup by iterating all synapses
     * 
     * @param destination Destination neuron ID
     * @return Vector of (source, weight, type, delay) triples
     */
    std::vector<std::tuple<uint64_t, float, uint8_t, uint32_t>> getSynapsesTo(
        uint64_t destination) const;
    
    /**
     * Get total synapse count
     */
    size_t getSynapseCount() const { return totalSynapses_; }
    
    /**
     * Get neuron count
     */
    size_t getNeuronCount() const { return neuronCount_; }
    
    /**
     * Get connectivity statistics
     */
    struct Stats {
        size_t numNeurons;
        size_t numSynapses;
        float connectivityProbability;
        float avgOutDegree;
        float avgInDegree;
        size_t maxOutDegree;
        size_t maxInDegree;
        float excitatoryRatio;
        size_t memoryUsageBytes;
    };
    
    Stats getStats() const;
    
    /**
     * Clear all synapses
     */
    void clear();
    
    /**
     * Memory usage in bytes
     */
    size_t memoryUsage() const;
    
    /**
     * Check if neuron is active (has outgoing synapses)
     */
    bool isNeuronActive(uint64_t neuronId) const {
        return rowOffsets_[neuronId] < rowOffsets_[neuronId + 1];
    }
    
private:
    // CSR arrays
    std::vector<size_t> rowOffsets_;      // Offset into columnIndices_ for each neuron
    std::vector<uint64_t> columnIndices_;  // Destination neuron for each synapse
    std::vector<float> weights_;          // Synaptic weight for each connection
    std::vector<uint8_t> synapseTypes_;   // Synapse type (excitatory/inhibitory)
    std::vector<uint32_t> delays_;       // Synaptic delay
    
    // Reverse lookup for efficient destination-to-source queries
    std::vector<std::vector<size_t>> incomingLists_;  // For each dest, list of synapse indices
    
    // Statistics
    size_t neuronCount_;
    size_t totalSynapses_;
    
    // Active neuron lists for fast lookup
    std::vector<bool> activeNeurons_;
    std::vector<size_t> activeNeuronIndices_;
};

/**
 * Memory-efficient connectivity cache for frequently accessed neurons
 * 
 * Stores recent connectivity queries to avoid recomputation
 */
class ConnectivityCache {
public:
    static constexpr size_t CACHE_SIZE = 1024;
    
    struct CacheEntry {
        uint64_t neuronId;
        Timestamp timestamp;
        std::vector<uint64_t> outgoingNeighbors;
        std::vector<float> weights;
        
        CacheEntry(uint64_t id) : neuronId(id), timestamp(0), weights(outgoingNeighbors.size()) {}
    };
    
    ConnectivityCache() : head_(0), tail_(0), size_(0) {}
    
    /**
     * Get cached connectivity for a neuron
     * 
     * @param neuronId Neuron to get connections for
     * @param currentTime Current simulation time
     * @param fetchFunc Function to fetch connections if not cached
     * @return Vector of (neighbor, weight) pairs
     */
    template<typename FetchFunc>
    std::vector<std::pair<uint64_t, float>> getCachedConnections(
        uint64_t neuronId, Timestamp currentTime, FetchFunc&& fetchFunc);
    
    /**
     * Invalidate cache for a neuron (when connectivity changes)
     */
    void invalidate(uint64_t neuronId);
    
    /**
     * Clear entire cache
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        head_ = tail_ = size_ = 0;
    }
    
    /**
     * Check if cache entry is still valid
     */
    bool isValid(const CacheEntry& entry, Timestamp currentTime) const {
        return (currentTime - entry.timestamp) < 1000;  // Valid for 1000 time units
    }
    
private:
    std::array<CacheEntry, CACHE_SIZE> cache_;
    std::array<size_t, CACHE_SIZE> nextIndex_;
    size_t head_;
    size_t tail_;
    size_t size_;
    mutable std::mutex mutex_;
};

/**
 * Connectivity optimizer for biological patterns
 * 
 * Applies biological connectivity principles:
 * - Distance-dependent connectivity
 * - Layer-based patterns
 * - Activity-dependent plasticity
 * - Functional clustering
 */
class BiologicalConnectivityOptimizer {
public:
    /**
     * Apply distance-dependent connectivity to CSR structure
     * 
     * @param connectivity CSR connectivity to optimize
     * @param neuronPositions Array of neuron positions (x, y, z)
     * @param maxDistance Maximum distance for connections
     * @param distanceDecay Decay factor for distance-based probability
     */
    static void applyDistanceDependency(
        CSRConnectivity& connectivity,
        const std::vector<std::array<float, 3>>& neuronPositions,
        float maxDistance = 100.0f,
        float distanceDecay = 0.5f);
    
    /**
     * Group neurons by function and create regional hierarchies
     * 
     * @param connectivity CSR connectivity to modify
     * @param neuronTypes Array of neuron types
     * @param regionMapping Neuron to region assignment
     */
    static void createRegionalHierarchies(
        CSRConnectivity& connectivity,
        const std::vector<NeuronType>& neuronTypes,
        const std::vector<uint64_t>& regionMapping);
    
    /**
     * Compress connectivity by grouping similar neurons
     * 
     * @param connectivity CSR connectivity to compress
     * @param compressionThreshold Minimum connection probability to keep
     */
    static void compressConnectivity(
        CSRConnectivity& connectivity,
        float compressionThreshold = 0.01f);
    
    /**
     * Apply synaptic weight quantization for memory savings
     * 
     * @param connectivity CSR connectivity to quantize
     * @param bits Number of bits per weight (8, 16, or 32)
     */
    static void applyWeightQuantization(
        CSRConnectivity& connectivity,
        uint8_t bits = 8);
};

/**
 * Fast neuron indexer for O(1) lookup by type, region, and function
 */
class NeuronIndexer {
public:
    struct NeuronIndex {
        uint64_t neuronId;
        uint32_t regionIndex;
        uint32_t populationIndex;
        uint32_t localIndex;
    };
    
    NeuronIndexer(size_t neuronCount);
    
    /**
     * Add a neuron to all indexes
     */
    void addNeuron(uint64_t neuronId, uint32_t regionIndex,
                   uint32_t populationIndex, uint32_t localIndex);
    
    /**
     * Remove a neuron from all indexes
     */
    void removeNeuron(uint64_t neuronId);
    
    /**
     * Get all neurons of a specific type (requires neuron type lookup)
     */
    std::vector<uint64_t> getNeuronsByType(NeuronType type) const;
    
    /**
     * Get all neurons in a region
     */
    std::vector<uint64_t> getNeuronsInRegion(uint32_t regionIndex) const;
    
    /**
     * Get all neurons in a population
     */
    std::vector<uint64_t> getNeuronsInPopulation(uint32_t populationIndex) const;
    
    /**
     * Get neuron index by ID
     */
    const NeuronIndex* getNeuronIndex(uint64_t neuronId) const;
    
    /**
     * Get all active neurons (those with outgoing connections)
     */
    std::vector<uint64_t> getActiveNeurons() const;
    
    /**
     * Update connectivity index
     */
    void updateConnectivity(uint64_t source, uint64_t destination, bool connected);
    
private:
    // Direct lookup by neuron ID
    std::vector<NeuronIndex> neuronIndices_;
    std::unordered_map<uint64_t, uint32_t> neuronToIndexMap_;
    
    // Indexes for fast querying
    std::unordered_map<NeuronType, std::vector<uint64_t>> neuronsByType_;
    std::unordered_map<uint32_t, std::vector<uint64_t>> neuronsByRegion_;
    std::unordered_map<uint32_t, std::vector<uint64_t>> neuronsByPopulation_;
    std::vector<uint64_t> activeNeurons_;
    std::unordered_map<uint64_t, bool> neuronActive_;
};

/**
 * Memory pool for connectivity data structures
 * 
 * Provides pre-allocated memory for fast connectivity operations
 */
class ConnectivityMemoryPool {
public:
    ConnectivityMemoryPool();
    ~ConnectivityMemoryPool();
    
    /**
     * Allocate neuron indices
     */
    NeuronIndexer::NeuronIndex* allocateNeuronIndex();
    
    /**
     * Deallocate neuron indices
     */
    void deallocateNeuronIndex(NeuronIndexer::NeuronIndex* index);
    
    /**
     * Allocate CSR connectivity
     */
    CSRConnectivity* allocateCSRConnectivity(size_t neuronCount);
    
    /**
     * Deallocate CSR connectivity
     */
    void deallocateCSRConnectivity(CSRConnectivity* connectivity);
    
    /**
     * Get memory usage
     */
    size_t memoryUsage() const;
    
private:
    // Neuron index pool
    struct NeuronIndexBlock {
        NeuronIndexer::NeuronIndex indices[1024];  // 1024 indices per block
        NeuronIndexBlock* next;
    };
    
    NeuronIndexBlock* indexBlockHead_;
    size_t freeIndices_[1024];  // Stack of free indices
    size_t freeCount_;
    
    // CSR connectivity pool
    struct CSRBlock {
        CSRConnectivity connectivity;
        CSRBlock* next;
    };
    
    CSRBlock* csrBlockHead_;
    size_t csrBlockCapacity_;
};

/**
 * Connectivity statistics calculator
 * 
 * Computes pre-computed statistics for connectivity queries
 */
class ConnectivityStatsCalculator {
public:
    struct Stats {
        float averageDegree;
        float degreeVariance;
        float clusteringCoefficient;
        float pathLength;
        float efficiency;
        float smallWorldness;
        float modularity;
        std::vector<float> degreeDistribution;
    };
    
    static Stats calculate(const CSRConnectivity& connectivity);
    static void updateIncremental(CSRConnectivity& connectivity, 
                                   uint64_t neuronId, bool connected);
    
private:
    static void computeDegreeStats(const CSRConnectivity& connectivity,
                                   Stats& stats);
    static void computeClusteringStats(const CSRConnectivity& connectivity,
                                       Stats& stats);
};

/**
 * Connectivity compressor for sparse networks
 * 
 * Reduces memory usage of connectivity data structures
 */
class ConnectivityCompressor {
public:
    /**
     * Compress CSR connectivity using run-length encoding for dense regions
     */
    static void compressDenseRegions(CSRConnectivity& connectivity);
    
    /**
     * Apply bitmap representation for small neighborhoods
     */
    static void applyBitmapCompression(CSRConnectivity& connectivity,
                                       size_t neighborhoodSize = 64);
    
    /**
     * Convert to hybrid format (CSR + bitmap for dense regions)
     */
    static void convertToHybridFormat(CSRConnectivity& connectivity);
};

} // namespace nlm

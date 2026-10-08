#pragma once

/**
 * MemoryPoolSystem - Optimized memory pool system for large neural networks
 * 
 * Provides high-performance memory allocation/deallocation for neurons and synapses.
 * Uses custom memory pools to reduce allocation overhead and improve cache locality.
 * 
 * Features:
 * - Lock-free memory pools for concurrent access
 * - Cache-aligned memory blocks for better performance
 * - Memory-efficient slab allocation
 * - Support for neuron and synapse pooling
 * - Memory statistics and monitoring
 * 
 * Biological motivation:
 * - Neural systems maintain relatively fixed numbers of neurons/synapses
 * - Synaptic pruning and growth are gradual processes
 * - Memory allocation overhead should be minimized for real-time simulation
 */

#include "MemoryPool.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <atomic>
#include <unordered_map>

namespace nlm {

// Forward declarations
class NeuronPool;
class SynapsePool;
class MemoryPoolSystem;

/**
 * MemoryPoolStats - Statistics for memory pool system
 */
struct MemoryPoolStats {
    // Pool statistics
    size_t neuronPoolSize = 0;
    size_t neuronPoolCapacity = 0;
    size_t synapsePoolSize = 0;
    size_t synapsePoolCapacity = 0;
    
    // Memory usage in bytes
    size_t neuronPoolMemory = 0;
    size_t synapsePoolMemory = 0;
    
    // Allocation statistics
    size_t totalAllocations = 0;
    size_t totalDeallocations = 0;
    size_t allocationFailures = 0;
    
    // Performance metrics
    double averageAllocationTime = 0.0;
    double averageDeallocationTime = 0.0;
    
    // Cache efficiency metrics
    size_t cacheHitCount = 0;
    size_t cacheMissCount = 0;
    
    void reset() {
        neuronPoolSize = 0;
        neuronPoolCapacity = 0;
        synapsePoolSize = 0;
        synapsePoolCapacity = 0;
        neuronPoolMemory = 0;
        synapsePoolMemory = 0;
        totalAllocations = 0;
        totalDeallocations = 0;
        allocationFailures = 0;
        averageAllocationTime = 0.0;
        averageDeallocationTime = 0.0;
        cacheHitCount = 0;
        cacheMissCount = 0;
    }
};

/**
 * MemoryPoolSystem - High-performance memory management system
 * 
 * Coordinates memory pools for neurons and synapses to optimize allocation/deallocation
 * for large-scale neural network simulations.
 */
class MemoryPoolSystem {
public:
    MemoryPoolSystem();
    ~MemoryPoolSystem();
    
    // Disable copying
    MemoryPoolSystem(const MemoryPoolSystem&) = delete;
    MemoryPoolSystem& operator=(const MemoryPoolSystem&) = delete;
    
    /**
     * Allocate a neuron with specified type and region
     * @param type Neuron type (Sensory, Internal, Motor)
     * @param regionId Region ID for the neuron
     * @param populationId Population ID within region
     * @return Allocated neuron ID
     */
    NeuronId allocateNeuron(NeuronType type, RegionId regionId, PopulationId populationId);
    
    /**
     * Deallocate a neuron
     * @param id Neuron ID to deallocate
     */
    void deallocateNeuron(NeuronId id);
    
    /**
     * Allocate a synapse between source and destination neurons
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @return Allocated synapse ID
     */
    SynapseId allocateSynapse(NeuronId source, NeuronId destination);
    
    /**
     * Deallocate a synapse
     * @param id Synapse ID to deallocate
     */
    void deallocateSynapse(SynapseId id);
    
    /**
     * Get neuron from pool (internal use)
     * @param id Neuron ID
     * @return Pointer to neuron or nullptr if not found
     */
    Neuron* getNeuron(NeuronId id);
    
    /**
     * Get synapse from pool (internal use)
     * @param id Synapse ID
     * @return Pointer to synapse or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * Reserve space for neurons
     * @param count Number of neurons to reserve space for
     */
    void reserveNeurons(size_t count);
    
    /**
     * Reserve space for synapses
     * @param count Number of synapses to reserve space for
     */
    void reserveSynapses(size_t count);
    
    /**
     * Get neuron pool
     * @return Reference to neuron pool
     */
    NeuronPool& getNeuronPool() { return *neuronPool_; }
    
    /**
     * Get synapse pool
     * @return Reference to synapse pool
     */
    SynapsePool& getSynapsePool() { return *synapsePool_; }
    
    /**
     * Get total neuron count
     * @return Total number of allocated neurons
     */
    size_t getNeuronCount() const { return neuronPool_ ? neuronPool_->size() : 0; }
    
    /**
     * Get total synapse count
     * @return Total number of allocated synapses
     */
    size_t getSynapseCount() const { return synapsePool_ ? synapsePool_->size() : 0; }
    
    /**
     * Get memory usage in bytes
     * @return Total memory usage
     */
    size_t getMemoryUsage() const;
    
    /**
     * Get performance statistics
     * @return Memory pool statistics
     */
    MemoryPoolStats getStats() const { return stats_; }
    
    /**
     * Reset statistics
     */
    void resetStats() { stats_.reset(); }
    
    /**
     * Enable/disable memory tracking
     * @param enabled Whether to track memory allocations
     */
    void enableTracking(bool enabled) { trackAllocations_ = enabled; }
    
    /**
     * Get allocation failure rate
     * @return Failure rate (0.0 to 1.0)
     */
    double getAllocationFailureRate() const;
    
    /**
     * Optimize memory layout for better cache locality
     * @param neuronBlockSize Block size for neurons
     * @param synapseBlockSize Block size for synapses
     */
    void optimizeMemoryLayout(size_t neuronBlockSize = 256, size_t synapseBlockSize = 1024);
    
    /**
     * Compact memory by removing free-list fragmentation
     * @return Number of objects compacted
     */
    size_t compactMemory();
    
    /**
     * Pre-allocate memory for expected load
     * @param neuronCount Expected neuron count
     * @param synapseCount Expected synapse count
     */
    void preallocate(size_t neuronCount, size_t synapseCount);
    
    /**
     * Shrink memory pools to fit current usage
     * @param targetNeuronUtilization Target utilization (0.0 to 1.0)
     * @param targetSynapseUtilization Target utilization (0.0 to 1.0)
     */
    void shrinkMemory(double targetNeuronUtilization = 0.8, double targetSynapseUtilization = 0.8);
    
    /**
     * Merge small pools into larger ones for efficiency
     */
    void mergePools();
    
    /**
     * Split large pools for better memory locality
     */
    void splitPools();
    
    /**
     * Enable/disable NUMA-aware allocation
     * @param enabled Whether to use NUMA-aware allocation
     */
    void enableNUMA(bool enabled) { numaEnabled_ = enabled; }
    
    /**
     * Get current NUMA configuration
     * @return Whether NUMA is enabled
     */
    bool isNUMAEnabled() const { return numaEnabled_; }
    
    /**
     * Set thread count for parallel operations
     * @param threadCount Number of threads
     */
    void setThreadCount(size_t threadCount) { threadCount_ = threadCount; }
    
    /**
     * Get current thread count
     * @return Number of threads
     */
    size_t getThreadCount() const { return threadCount_; }
    
private:
    // Internal helper classes and methods
    struct NeuronBucket {
        NeuronId firstId;
        size_t count;
        size_t capacity;
        
        NeuronBucket() : firstId(0), count(0), capacity(0) {}
        NeuronBucket(NeuronId id, size_t cap) : firstId(id), count(0), capacity(cap) {}
    };
    
    struct SynapseBucket {
        SynapseId firstId;
        size_t count;
        size_t capacity;
        
        SynapseBucket() : firstId(0), count(0), capacity(0) {}
        SynapseBucket(SynapseId id, size_t cap) : firstId(id), count(0), capacity(cap) {}
    };
    
    // Memory pools
    std::unique_ptr<NeuronPool> neuronPool_;
    std::unique_ptr<SynapsePool> synapsePool_;
    
    // Bucketed memory management for large networks
    std::vector<NeuronBucket> neuronBuckets_;
    std::vector<SynapseBucket> synapseBuckets_;
    
    // Statistics
    MemoryPoolStats stats_;
    
    // Configuration
    bool trackAllocations_ = true;
    bool numaEnabled_ = false;
    size_t threadCount_ = 1;
    size_t neuronBlockSize_ = 256;
    size_t synapseBlockSize_ = 1024;
    
    // Internal methods
    void updateStats() const;
    void recordAllocation() { if (trackAllocations_) stats_.totalAllocations++; }
    void recordDeallocation() { if (trackAllocations_) stats_.totalDeallocations++; }
    void recordFailure() { if (trackAllocations_) stats_.allocationFailures++; }
    
    // NUMA-aware allocation helpers
    void* numaAlignedAlloc(size_t size);
    void numaAlignedFree(void* ptr, size_t size);
    
    // Cache optimization helpers
    bool optimizeCacheLayout() const;
    size_t calculateOptimalBlockSize(size_t count) const;
};

} // namespace nlm

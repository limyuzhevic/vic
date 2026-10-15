#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace nlm {

/**
 * Memory Pool Management for Performance Optimizations
 * 
 * Pre-allocated object pools for neurons and synapses:
 * - Lock-free memory allocation
 * - Pre-allocated memory blocks
 * - Memory-efficient slab allocation
 * - Thread-safe operations
 * - Biological motivation (fixed neuron/synapse numbers)
 * 
 * Reduces allocation overhead during simulation
 */
class MemoryPool {
public:
    MemoryPool();
    ~MemoryPool();

    // ===== MEMORY POOL MANAGEMENT =====

    // Neuron memory pool
    class NeuronPool {
    public:
        NeuronPool();
        ~NeuronPool();

        // Allocate neuron with state
        NeuronId allocateNeuron();

        // Deallocate neuron
        void deallocateNeuron(NeuronId id);

        // Get neuron state directly (for SoA access)
        template<typename Member>
        Member* getNeuronState(NeuronIndex index);

        // Get neuron state by ID
        template<typename Member>
        Member* getNeuronStateById(NeuronId id);

        // Get total count
        size_t size() const { return size_; }

        // Check if empty
        bool empty() const { return freeList_.empty(); }

        // Get memory usage
        size_t memoryUsage() const;

        // Reserve additional neurons
        void reserve(size_t count);

        // Get neuron indices for parallel processing
        std::vector<NeuronIndex> getNeuronIndices() const;

        // Get neuron indices for specific region
        std::vector<NeuronIndex> getRegionNeuronIndices(RegionId regionId) const;

    private:
        // Cache line alignment
        static constexpr size_t CACHE_LINE = 64;

        // Neuron block with cache-aligned storage
        struct alignas(CACHE_LINE) NeuronBlock {
            // SoA storage for neuron states
            std::vector<float> membranePotential;
            std::vector<float> restingPotential;
            std::vector<float> threshold;
            std::vector<float> resetPotential;
            std::vector<float> leakConductance;
            std::vector<float> synapseConductance;
            std::vector<float> firingRate;
            std::vector<uint8_t> firingState;  // FiringState enum
            std::vector<uint32_t> refractoryRemaining;
            std::vector<uint32_t> refractoryPeriod;
            std::vector<float> adaptationVariable;
            std::vector<float> lastSpikeTime;
            std::vector<uint64_t> neuronType;  // NeuronType enum
            std::vector<uint64_t> regionId;
            std::vector<uint64_t> populationId;

            // Per-neuron current injection (hot path)
            std::vector<float> synapticInput;

            NeuronBlock() {}
        };

        size_t size_;
        size_t capacity_;
        std::vector<std::unique_ptr<NeuronBlock>> blocks_;

        // Free list for deallocated neurons
        std::vector<NeuronIndex> freeList_;
        std::atomic<size_t> freeListTop_;

        // Region mappings
        std::map<RegionId, std::vector<NeuronIndex>> regionNeuronIndices_;

        // Statistics
        std::atomic<size_t> totalAllocations_;
        std::atomic<size_t> totalDeallocations_;

        // Helper methods
        NeuronIndex allocateFromFreeList();
        void returnToFreeList(NeuronIndex index);
        NeuronIndex getNeuronIndex(NeuronId id) const;
        void updateRegionMappings();
        void cleanupBlock(NeuronBlock* block);
    };

    // Synapse memory pool
    class SynapsePool {
    public:
        SynapsePool();
        ~SynapsePool();

        // Allocate synapse
        SynapseId allocateSynapse(NeuronId source, NeuronId destination);

        // Deallocate synapse
        void deallocateSynapse(SynapseId id);

        // Get synapse data (for SoA access)
        template<typename Member>
        Member* getSynapseState(SynapseIndex index);

        // Get synapse data by ID
        template<typename Member>
        Member* getSynapseStateById(SynapseId id);

        // Get source/destination neurons
        NeuronId getSourceNeuron(SynapseIndex index) const;
        NeuronId getDestinationNeuron(SynapseIndex index) const;

        // Total count
        size_t size() const { return size_; }

        // Check if empty
        bool empty() const { return freeList_.empty(); }

        // Memory usage
        size_t memoryUsage() const;

        // Reserve additional synapses
        void reserve(size_t count);

        // Get synapse indices for parallel processing
        std::vector<SynapseIndex> getSynapseIndices() const;

        // Get synapse indices for specific source neuron
        std::vector<SynapseIndex> getSourceSynapseIndices(NeuronId sourceId) const;

        // Get synapse indices for specific target neuron
        std::vector<SynapseIndex> getTargetSynapseIndices(NeuronId targetId) const;

    private:
        // Cache line alignment
        static constexpr size_t CACHE_LINE = 64;

        // Synapse block with cache-aligned storage
        struct alignas(CACHE_LINE) SynapseBlock {
            // SoA storage for synapse states
            std::vector<uint64_t> sourceNeuron;  // Source neuron ID
            std::vector<uint64_t> destinationNeuron;  // Dest neuron ID
            std::vector<float> weight;
            std::vector<uint32_t> delay;
            std::vector<uint8_t> synapseType;  // SynapseType enum
            std::vector<uint8_t> plasticityFlags;  // Packed flags

            // Short-term plasticity
            std::vector<float> shortTermDepression;
            std::vector<float> shortTermFacilitation;
            std::vector<float> lastPreSpikeTime;
            std::vector<float> lastPostSpikeTime;

            // Eligibility trace
            std::vector<float> eligibilityTrace;

            // Synaptic efficacy
            std::vector<float> efficacy;

            SynapseBlock() {}
        };

        size_t size_;
        size_t capacity_;
        std::vector<std::unique_ptr<SynapseBlock>> blocks_;

        // Free list for deallocated synapses
        std::vector<SynapseIndex> freeList_;
        std::atomic<size_t> freeListTop_;

        // Connectivity mappings
        std::map<NeuronId, std::vector<SynapseIndex>> sourceToSynapseIndices_;
        std::map<NeuronId, std::vector<SynapseIndex>> targetToSynapseIndices_;

        // Statistics
        std::atomic<size_t> totalAllocations_;
        std::atomic<size_t> totalDeallocations_;

        // Helper methods
        SynapseIndex allocateFromFreeList();
        void returnToFreeList(SynapseIndex index);
        SynapseIndex getSynapseIndex(SynapseId id) const;
        void updateConnectivityMappings();
        void cleanupBlock(SynapseBlock* block);
    };

    // ===== THREAD-SAFE OPERATIONS =====

    // Thread-safe memory allocation
    class ThreadSafePool {
    public:
        ThreadSafePool(std::unique_ptr<NeuronPool> neuronPool,
                      std::unique_ptr<SynapsePool> synapsePool);
        ~ThreadSafePool();

        // Allocate neuron (thread-safe)
        NeuronId allocateNeuron();

        // Allocate synapse (thread-safe)
        SynapseId allocateSynapse(NeuronId source, NeuronId destination);

        // Deallocate neuron (thread-safe)
        void deallocateNeuron(NeuronId id);

        // Deallocate synapse (thread-safe)
        void deallocateSynapse(SynapseId id);

        // Get neuron state (thread-safe)
        template<typename Member>
        Member* getNeuronState(NeuronIndex index);

        // Get synapse state (thread-safe)
        template<typename Member>
        Member* getSynapseState(SynapseIndex index);

        // Get pool statistics
        struct PoolStats {
            size_t neuronAllocations;
            size_t neuronDeallocations;
            size_t synapseAllocations;
            size_t synapseDeallocations;
            size_t neuronMemoryUsage;
            size_t synapseMemoryUsage;
        };

        PoolStats getStats() const;

        // Wait until pool has sufficient capacity
        bool waitForCapacity(size_t neuronCount, size_t synapseCount, 
                            std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

    private:
        std::unique_ptr<NeuronPool> neuronPool_;
        std::unique_ptr<SynapsePool> synapsePool_;
        mutable std::mutex mutex_;
        std::condition_variable capacityCV_;
    };

    // ===== BIOLOGICAL MOTIVATION =====

    // Biological neuron pool characteristics
    struct BiologicalCharacteristics {
        float neuronBirthRate;           // Rate of new neuron creation (neurons/year)
        float neuronDeathRate;          // Rate of neuron loss (neurons/year)
        float synapseFormationRate;      // Rate of new synapse formation
        float synapsePruningRate;        // Rate of synapse pruning
        float homeostaticTarget;        // Target neuron count
        float homeostaticRange;         // Acceptable neuron count range
        float criticalPeriodLength;     // Length of critical period (years)
    };

    void setBiologicalCharacteristics(const BiologicalCharacteristics& bio) { 
        biologicalCharacteristics_ = bio; 
    }

    // ===== PERFORMANCE OPTIMIZATION =====

    // Pool performance optimization
    struct PerformanceOptimization {
        bool enableNUMA;                // Enable NUMA-aware allocation
        bool enableCacheAffinity;       // Enable cache affinity optimization
        bool enableLockFree;            // Enable lock-free operations
        bool enableMemoryPooling;       // Enable memory pooling
        float memoryAlignment;          // Memory alignment (bytes)
        size_t minBlockSize;            // Minimum block size
        size_t maxBlockSize;            // Maximum block size
        size_t blockGrowthFactor;       // Block growth factor
    };

    void setPerformanceOptimization(const PerformanceOptimization& opt) { 
        performanceOpt_ = opt; 
    }

    // ===== LIFTS =====

    // Get access to pools
    NeuronPool& getNeuronPool() { return *neuronPool_; }
    SynapsePool& getSynapsePool() { return *synapsePool_; }
    ThreadSafePool& getThreadSafePool() { return *threadSafePool_; }

    // Initialize pools
    bool initialize(size_t initialNeurons, size_t initialSynapses);

    // Update biological dynamics
    void updateBiologicalDynamics(float dt);

    // Cleanup
    void cleanup();

    // Get overall memory usage
    size_t totalMemoryUsage() const;

    // Get pool statistics
    struct OverallStats {
        size_t neuronAllocations;
        size_t neuronDeallocations;
        size_t synapseAllocations;
        size_t synapseDeallocations;
        size_t neuronMemoryUsage;
        size_t synapseMemoryUsage;
        float utilizationRate;          // Pool utilization rate (0-1)
        float allocationEfficiency;    // Allocation efficiency
    };

    OverallStats getOverallStats() const;

private:
    std::unique_ptr<NeuronPool> neuronPool_;
    std::unique_ptr<SynapsePool> synapsePool_;
    std::unique_ptr<ThreadSafePool> threadSafePool_;

    // Biological characteristics
    BiologicalCharacteristics biologicalCharacteristics_;

    // Performance optimization
    PerformanceOptimization performanceOpt_;

    // Statistics
    OverallStats overallStats_;

    // Helper methods
    void setupNUMAAwareAllocation();
    void setupCacheAffinityOptimization();
    void setupLockFreeAllocation();
    void setupMemoryPooling();
};

} // namespace nlm

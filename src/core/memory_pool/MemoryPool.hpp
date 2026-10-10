// MemoryPool.hpp
// Efficient memory allocation for neural components

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Memory statistics for performance analysis
struct MemoryStats {
    size_t totalAllocated = 0;
    size_t totalUsed = 0;
    size_t peakUsage = 0;
    size_t allocationCount = 0;
    size_t deallocationCount = 0;
};

// Generic memory pool template for efficient allocation
// Uses pre-allocated blocks to eliminate malloc/free overhead
// and improve cache locality

template<typename T, size_t BLOCK_SIZE = 1024>
class MemoryPool {
public:
    MemoryPool() = default;
    explicit MemoryPool(size_t initialBlocks);
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Allocate from pool
    T* allocate();
    
    // Deallocate back to pool
    void deallocate(T* ptr);
    
    // Get current usage statistics
    size_t getUsedCount() const { return m_usedCount; }
    size_t getTotalCount() const { return m_totalAllocated; }
    size_t getBlockSize() const { return m_blockSize; }
    
    // Memory statistics
    const MemoryStats& getStats() const { return m_stats; }
    
    // Reset pool (clear all allocated blocks)
    void clear();
    
    // Pre-allocate additional blocks
    void reserve(size_t additionalBlocks);
    
    // Check if pointer belongs to this pool
    bool owns(T* ptr) const;
    
protected:
    // Block structure for cache-friendly allocation
    struct Block {
        alignas(64) char data[BLOCK_SIZE * sizeof(T)];
        Block* next = nullptr;
        size_t used = 0;
    };
    
    Block* m_firstBlock = nullptr;
    Block* m_lastBlock = nullptr;
    size_t m_blockSize = BLOCK_SIZE;
    size_t m_usedCount = 0;
    size_t m_totalAllocated = 0;
    MemoryStats m_stats;
};

// Neuron memory pool with optimized state access patterns
// Uses Structure-of-Arrays (SoA) layout for better SIMD access
class NeuronMemoryPool {
public:
    struct NeuronData {
        alignas(64) float membranePotential;
        alignas(64) float restingPotential;
        alignas(64) float threshold;
        alignas(64) float resetPotential;
        alignas(64) float leakConductance;
        alignas(64) uint32_t refractoryRemaining;
        alignas(64) uint32_t refractoryPeriod;
        alignas(64) float adaptationVariable;
        alignas(64) float lastSpikeTime;
        alignas(64) uint32_t id;
    };
    
    NeuronMemoryPool(size_t initialCapacity = 1024);
    ~NeuronMemoryPool();
    
    // Get neuron data by index
    NeuronData& getNeuronData(size_t index);
    const NeuronData& getNeuronData(size_t index) const;
    
    // Allocate new neuron
    size_t allocateNeuron();
    
    // Deallocate neuron
    void deallocateNeuron(size_t index);
    
    // Get current capacity
    size_t getCapacity() const { return m_capacity; }
    size_t getSize() const { return m_size; }
    
    // Reserve additional space
    void reserve(size_t additionalCapacity);
    
    // Get memory statistics
    const MemoryStats& getStats() const { return m_stats; }
    
    // Clear all neurons
    void clear();
    
private:
    std::vector<NeuronData> m_neurons;
    std::vector<bool> m_used;
    size_t m_capacity;
    size_t m_size;
    MemoryStats m_stats;
    
    // Free list for fast deallocation
    std::vector<size_t> m_freeList;
};

// Synapse memory pool for efficient synaptic connection management
class SynapseMemoryPool {
public:
    struct SynapseData {
        alignas(64) uint32_t sourceNeuron;
        alignas(64) uint32_t destinationNeuron;
        alignas(64) float weight;
        alignas(64) uint32_t delay;
        alignas(64) uint8_t synapseType;
        alignas(64) float eligibilityTrace;
        alignas(64) uint8_t plasticityFlags;
    };
    
    SynapseMemoryPool(size_t initialCapacity = 4096);
    ~SynapseMemoryPool();
    
    // Get synapse data by index
    SynapseData& getSynapseData(size_t index);
    const SynapseData& getSynapseData(size_t index) const;
    
    // Allocate new synapse
    size_t allocateSynapse();
    
    // Deallocate synapse
    void deallocateSynapse(size_t index);
    
    // Get current capacity
    size_t getCapacity() const { return m_capacity; }
    size_t getSize() const { return m_size; }
    
    // Reserve additional space
    void reserve(size_t additionalCapacity);
    
    // Get memory statistics
    const MemoryStats& getStats() const { return m_stats; }
    
    // Clear all synapses
    void clear();
    
private:
    std::vector<SynapseData> m_synapses;
    std::vector<bool> m_used;
    size_t m_capacity;
    size_t m_size;
    MemoryStats m_stats;
    
    // Free list for fast deallocation
    std::vector<size_t> m_freeList;
};

// Unified memory manager for all NLM memory pools
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Get singleton instance
    static MemoryManager& getInstance();
    
    // Disable copying
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    // Initialize with given capacities
    void initialize(size_t neuronCapacity = 1024, 
                   size_t synapseCapacity = 4096,
                   size_t memoryPoolBlockSize = 1024);
    
    // Get neuron memory pool
    NeuronMemoryPool& getNeuronPool() { return m_neuronPool; }
    const NeuronMemoryPool& getNeuronPool() const { return m_neuronPool; }
    
    // Get synapse memory pool
    SynapseMemoryPool& getSynapsePool() { return m_synapsePool; }
    const SynapseMemoryPool& getSynapsePool() const { return m_synapsePool; }
    
    // Get memory statistics
    MemoryStats getTotalStats() const;
    
    // Reset all pools
    void reset();
    
    // Set memory pool block size
    void setBlockSize(size_t blockSize);
    
    // Enable/disable memory tracking
    void setTrackMemory(bool track) { m_trackMemory = track; }
    
private:
    NeuronMemoryPool m_neuronPool;
    SynapseMemoryPool m_synapsePool;
    bool m_initialized = false;
    bool m_trackMemory = true;
};

// Helper function to allocate from memory pool
template<typename T, size_t BLOCK_SIZE>
T* allocateFromPool(MemoryPool<T, BLOCK_SIZE>& pool);

// Helper function to deallocate to memory pool
template<typename T, size_t BLOCK_SIZE>
void deallocateToPool(MemoryPool<T, BLOCK_SIZE>& pool, T* ptr);

} // namespace nlm

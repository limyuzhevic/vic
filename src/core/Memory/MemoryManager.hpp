#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include <iomanip>

namespace nlm {

// Memory statistics tracking
struct MemoryStats {
    uint64_t totalAllocated = 0;
    uint64_t peakAllocated = 0;
    uint64_t currentAllocated = 0;
    uint64_t neuronAllocations = 0;
    uint64_t synapseAllocations = 0;
    uint64_t neuronDeallocations = 0;
    uint64_t synapseDeallocations = 0;
    uint64_t poolHits = 0;
    uint64_t poolMisses = 0;
};

// Memory pool for Neuron objects
class NeuronPool {
public:
    NeuronPool(size_t initialCapacity = 1024, size_t maxCapacity = 1024 * 1024);
    ~NeuronPool();
    
    Neuron* allocate(NeuronId id, NeuronType type = NeuronType::Internal);
    void deallocate(Neuron* neuron);
    void clear();
    
    size_t getCapacity() const { return capacity; }
    size_t getSize() const { return size; }
    float getUtilization() const { return static_cast<float>(size) / capacity; }
    
    const MemoryStats& getStats() const { return stats; }
    void resetStats() { stats = MemoryStats{}; }
    
    // Pre-allocate memory
    void reserve(size_t count);
    
    // Alignment helper
    static void* alignedAllocate(size_t size, size_t alignment);
    static void alignedDeallocate(void* ptr, size_t size);
    
private:
    struct Chunk {
        Neuron* neurons;
        size_t capacity;
        Chunk* next;
        
        Chunk(size_t cap) : neurons(nullptr), capacity(cap), next(nullptr) {
            neurons = static_cast<Neuron*>(alignedAllocate(sizeof(Neuron) * cap, 64));
        }
        
        ~Chunk() {
            if (neurons) alignedDeallocate(neurons, sizeof(Neuron) * capacity);
        }
    };
    
    Chunk* head;
    Chunk* tail;
    size_t capacity;
    size_t size;
    MemoryStats stats;
    std::mutex mutex;
    bool initialized;
};

// Memory pool for Synapse objects
class SynapsePool {
public:
    SynapsePool(size_t initialCapacity = 1024, size_t maxCapacity = 1024 * 1024);
    ~SynapsePool();
    
    Synapse* allocate(SynapseId id, NeuronId source, NeuronId destination);
    void deallocate(Synapse* synapse);
    void clear();
    
    size_t getCapacity() const { return capacity; }
    size_t getSize() const { return size; }
    float getUtilization() const { return static_cast<float>(size) / capacity; }
    
    const MemoryStats& getStats() const { return stats; }
    void resetStats() { stats = MemoryStats{}; }
    
    // Pre-allocate memory
    void reserve(size_t count);
    
    // Global pool management
    static void initializeGlobalPools(size_t neuronPoolSize = 1024 * 1024,
                                     size_t synapsePoolSize = 10 * 1024 * 1024);
    static void shutdownGlobalPools();
    static NeuronPool& getGlobalNeuronPool();
    static SynapsePool& getGlobalSynapsePool();
    
private:
    struct Chunk {
        Synapse* synapses;
        size_t capacity;
        Chunk* next;
        
        Chunk(size_t cap) : synapses(nullptr), capacity(cap), next(nullptr) {
            synapses = static_cast<Synapse*>(alignedAllocate(sizeof(Synapse) * cap, 64));
        }
        
        ~Chunk() {
            if (synapses) alignedDeallocate(synapses, sizeof(Synapse) * capacity);
        }
    };
    
    Chunk* head;
    Chunk* tail;
    size_t capacity;
    size_t size;
    MemoryStats stats;
    std::mutex mutex;
    bool initialized;
    
    // Singleton instances
    static NeuronPool globalNeuronPool;
    static SynapsePool globalSynapsePool;
    static bool globalInitialized;
};

// Memory manager that coordinates all pools
class MemoryManager {
public:
    static MemoryManager& getInstance();
    
    void initialize(size_t neuronPoolSize = 1024 * 1024,
                   size_t synapsePoolSize = 10 * 1024 * 1024);
    void shutdown();
    
    // Get pool statistics
    const MemoryStats& getNeuronStats() const;
    const MemoryStats& getSynapseStats() const;
    uint64_t getTotalAllocated() const;
    uint64_t getPeakAllocated() const;
    float getMemoryUtilization() const;
    
    // Global pool access
    NeuronPool& getNeuronPool();
    SynapsePool& getSynapsePool();
    
    // Memory statistics reporting
    void printMemoryStats() const;
    std::string getMemoryReport() const;
    
    // Enable/disable memory tracking
    void setTrackingEnabled(bool enabled) { trackingEnabled = enabled; }
    bool isTrackingEnabled() const { return trackingEnabled; }
    
private:
    MemoryManager() : trackingEnabled(true), initialized(false) {}
    ~MemoryManager() = default;
    
    bool initialized;
    bool trackingEnabled;
    NeuronPool neuronPool;
    SynapsePool synapsePool;
};
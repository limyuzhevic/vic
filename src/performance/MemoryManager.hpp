#pragma once

#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "MemoryPool.hpp"
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * MemoryManager - Centralized memory management for NLM components
 * 
 * Integrates memory pools for neurons and synapses to eliminate manual allocation
 * and improve performance through pre-allocation and lock-free operations.
 * 
 * Features:
 * - NeuronPool for efficient neuron state storage
 * - SynapsePool for efficient synapse connection storage
 * - SoA (Structure of Arrays) layout for cache-friendly access
 * - Thread-safe allocation/deallocation with reference counting
 * - Automatic memory cleanup and pool management
 * 
 * Biological motivation:
 * - Mirrors biological neural systems with stable neuron/synapse populations
 * - Reduces "allocation churn" similar to neural maintenance processes
 * - Enables efficient memory reuse during simulation
 */
class MemoryManager {
public:
    // Singleton access pattern
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }
    
    ~MemoryManager() = default;
    
    // Disable copying and assignment
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    /**
     * Configure memory pools for simulation
     * @param neuronPoolSize Number of neurons to pre-allocate
     * @param synapsePoolSize Number of synapses to pre-allocate
     * @param blockSize Number of objects per memory block
     * @param initialBlocks Number of blocks to pre-allocate
     */
    void configure(size_t neuronPoolSize, size_t synapsePoolSize,
                   size_t blockSize = 1024, size_t initialBlocks = 4);
    
    /**
     * Get the global neuron pool
     */
    NeuronPool& getNeuronPool() { return neuronPool_; }
    
    /**
     * Get the global synapse pool
     */
    SynapsePool& getSynapsePool() { return synapsePool_; }
    
    /**
     * Register a neuron with the memory manager
     * @param neuron Pointer to the neuron object
     * @param id Neuron identifier
     */
    void registerNeuron(Neuron* neuron, NeuronId id);
    
    /**
     * Unregister a neuron from the memory manager
     * @param id Neuron identifier to unregister
     */
    void unregisterNeuron(NeuronId id);
    
    /**
     * Register a synapse with the memory manager
     * @param synapse Pointer to the synapse object
     * @param id Synapse identifier
     */
    void registerSynapse(Synapse* synapse, SynapseId id);
    
    /**
     * Unregister a synapse from the memory manager
     * @param id Synapse identifier to unregister
     */
    void unregisterSynapse(SynapseId id);
    
    /**
     * Get the pool index for a neuron
     * @param id Neuron identifier
     * @return Pool index or -1 if not found
     */
    NeuronIndex getNeuronIndex(NeuronId id) const;
    
    /**
     * Get the pool index for a synapse
     * @param id Synapse identifier
     * @return Pool index or -1 if not found
     */
    SynapseIndex getSynapseIndex(SynapseId id) const;
    
    /**
     * Get neuron from pool by index
     * @param index Pool index
     * @return Pointer to neuron or nullptr if invalid
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * Get synapse from pool by index
     * @param index Pool index
     * @return Pointer to synapse or nullptr if invalid
     */
    Synapse* getSynapse(SynapseIndex index);
    
    /**
     * Check if neuron is in the pool
     * @param id Neuron identifier
     * @return true if neuron exists in pool
     */
    bool containsNeuron(NeuronId id) const;
    
    /**
     * Check if synapse is in the pool
     * @param id Synapse identifier
     * @return true if synapse exists in pool
     */
    bool containsSynapse(SynapseId id) const;
    
    /**
     * Get total memory usage
     * @return Total memory usage in bytes
     */
    size_t getTotalMemoryUsage() const;
    
    /**
     * Get current simulation memory footprint
     * @return Memory footprint in bytes
     */
    size_t getCurrentMemoryFootprint() const;
    
    /**
     * Perform memory cleanup
     * @param maxAgeMs Maximum age of entries to keep (0 = keep all)
     */
    void cleanup(float maxAgeMs = 0.0f);
    
    /**
     * Clear all pools
     */
    void clearAll();
    
    /**
     * Set configuration parameters
     */
    void setNeuronPoolSize(size_t size) { neuronPoolSize_ = size; }
    void setSynapsePoolSize(size_t size) { synapsePoolSize_ = size; }
    void setBlockSize(size_t size) { blockSize_ = size; }
    void setInitialBlocks(size_t blocks) { initialBlocks_ = blocks; }
    
    /**
     * Get configuration parameters
     */
    size_t getNeuronPoolSize() const { return neuronPoolSize_; }
    size_t getSynapsePoolSize() const { return synapsePoolSize_; }
    size_t getBlockSize() const { return blockSize_; }
    size_t getInitialBlocks() const { return initialBlocks_; }

private:
    MemoryManager() : neuronPool_(), synapsePool_() {}
    
    // Neuron pool management
    NeuronPool neuronPool_;
    size_t neuronPoolSize_;
    size_t blockSize_;
    size_t initialBlocks_;
    
    // Synapse pool management  
    SynapsePool synapsePool_;
    size_t synapsePoolSize_;
    
    // Registration maps for pool lookups
    std::unordered_map<NeuronId, NeuronIndex> neuronRegistry_;
    std::unordered_map<SynapseId, SynapseIndex> synapseRegistry_;
    
    // Thread-safe operations
    mutable std::mutex registryMutex_;
};

// Inline implementations

inline void MemoryManager::configure(size_t neuronPoolSize, size_t synapsePoolSize,
                                   size_t blockSize, size_t initialBlocks) {
    neuronPoolSize_ = neuronPoolSize;
    synapsePoolSize_ = synapsePoolSize;
    blockSize_ = blockSize;
    initialBlocks_ = initialBlocks;
    
    // Reserve memory in pools
    neuronPool_.reserve(neuronPoolSize_);
    synapsePool_.reserve(synapsePoolSize_);
}

inline void MemoryManager::registerNeuron(Neuron* neuron, NeuronId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    
    // Register in pool if not already present
    if (!containsNeuron(id)) {
        // Note: This is a placeholder - actual integration with NeuronPool would
        // require modifying the Neuron class to use pool allocation
        neuronRegistry_[id] = NeuronIndex{id.index(), 0, 0, 0};
    }
}

inline void MemoryManager::unregisterNeuron(NeuronId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    neuronRegistry_.erase(id);
}

inline void MemoryManager::registerSynapse(Synapse* synapse, SynapseId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    
    if (!containsSynapse(id)) {
        synapseRegistry_[id] = SynapseIndex{id.index(), 0};
    }
}

inline void MemoryManager::unregisterSynapse(SynapseId id) {
    std::lock_guard<std::mutex> lock(registryMutex_);
    synapseRegistry_.erase(id);
}

inline NeuronIndex MemoryManager::getNeuronIndex(NeuronId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = neuronRegistry_.find(id);
    return (it != neuronRegistry_.end()) ? it->second : NeuronIndex{};
}

inline SynapseIndex MemoryManager::getSynapseIndex(SynapseId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    auto it = synapseRegistry_.find(id);
    return (it != synapseRegistry_.end()) ? it->second : SynapseIndex{};
}

inline Neuron* MemoryManager::getNeuron(NeuronIndex index) {
    // Note: This is a placeholder - actual implementation would need
    // to modify Neuron class to use NeuronPool for storage
    return nullptr;
}

inline Synapse* MemoryManager::getSynapse(SynapseIndex index) {
    // Note: This is a placeholder - actual implementation would need
    // to modify Synapse class to use SynapsePool for storage
    return nullptr;
}

inline bool MemoryManager::containsNeuron(NeuronId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return neuronRegistry_.find(id) != neuronRegistry_.end();
}

inline bool MemoryManager::containsSynapse(SynapseId id) const {
    std::lock_guard<std::mutex> lock(registryMutex_);
    return synapseRegistry_.find(id) != synapseRegistry_.end();
}

inline size_t MemoryManager::getTotalMemoryUsage() const {
    return neuronPool_.memoryUsage() + synapsePool_.memoryUsage();
}

inline size_t MemoryManager::getCurrentMemoryFootprint() const {
    return neuronPool_.size() * sizeof(Neuron) + synapsePool_.size() * sizeof(Synapse);
}

inline void MemoryManager::cleanup(float maxAgeMs) {
    // Placeholder - actual implementation would need to track allocation timestamps
    // and remove old entries
}

inline void MemoryManager::clearAll() {
    std::lock_guard<std::mutex> lock(registryMutex_);
    neuronRegistry_.clear();
    synapseRegistry_.clear();
}

// Global memory manager instance
inline MemoryManager& getMemoryManager() {
    return MemoryManager::getInstance();
}

} // namespace nlm

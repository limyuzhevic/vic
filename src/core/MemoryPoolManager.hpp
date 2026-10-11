#pragma once

#include "../core/Types/Types.hpp"
#include "../core/memory_pool.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "../performance/MemoryPool.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Memory pool configuration
struct MemoryPoolConfig {
    // Pool sizes
    size_t neuronPoolBlockSize = 1024;    // Number of neurons per block
    size_t neuronInitialBlocks = 4;       // Initial neuron blocks
    size_t synapsePoolBlockSize = 4096;    // Number of synapses per block
    size_t synapseInitialBlocks = 4;      // Initial synapse blocks
    
    // Performance tuning
    bool enableMemoryPool = true;          // Enable memory pools
    bool enableSoaAccess = true;           // Enable Structure of Arrays access
    bool enableStatistics = true;          // Enable memory statistics tracking
    
    // Cache optimization
    size_t neuronCacheLineSize = 64;       // Cache line size for neurons
    size_t synapseCacheLineSize = 64;      // Cache line size for synapses
    
    // Memory limit (optional)
    size_t maxMemoryUsage = 0;            // 0 = unlimited
};

// Global memory pool manager
class MemoryPoolManager {
public:
    static MemoryPoolManager& getInstance() {
        static MemoryPoolManager instance;
        return instance;
    }
    
    // Initialize memory pools
    void initialize(const MemoryPoolConfig& config) {
        config_ = config;
        
        if (config_.enableMemoryPool) {
            // Create neuron pool
            neuronPool_ = std::make_unique<NeuronPool>(
                config_.neuronPoolBlockSize,
                config_.neuronInitialBlocks
            );
            
            // Create synapse pool
            synapsePool_ = std::make_unique<SynapsePool>(
                config_.synapsePoolBlockSize,
                config_.synapseInitialBlocks
            );
        }
        
        // Initialize statistics
        stats_ = MemoryPoolStats();
    }
    
    // Neuron allocation
    NeuronId allocateNeuron() {
        if (!config_.enableMemoryPool || !neuronPool_) {
            // Fallback to dynamic allocation
            static NeuronId nextId = 1;
            return NeuronId(nextId++);
        }
        
        NeuronId id = neuronPool_->allocate();
        if (id.value > 0) {
            stats_.neuronsAllocated++;
        }
        return id;
    }
    
    // Neuron deallocation
    void deallocateNeuron(NeuronId id) {
        if (config_.enableMemoryPool && neuronPool_ && id.value > 0) {
            neuronPool_->deallocate(id);
            stats_.neuronsDeallocated++;
        }
    }
    
    // Synapse allocation
    SynapseId allocateSynapse(NeuronId source, NeuronId destination) {
        if (!config_.enableMemoryPool || !synapsePool_) {
            // Fallback to dynamic allocation
            static SynapseId nextId = 1;
            return SynapseId(nextId++);
        }
        
        SynapseId id = synapsePool_->allocate(source, destination);
        if (id.value > 0) {
            stats_.synapsesAllocated++;
        }
        return id;
    }
    
    // Synapse deallocation
    void deallocateSynapse(SynapseId id) {
        if (config_.enableMemoryPool && synapsePool_ && id.value > 0) {
            synapsePool_->deallocate(id);
            stats_.synapsesDeallocated++;
        }
    }
    
    // Memory statistics
    const MemoryPoolStats& getStats() const { return stats_; }
    
    // Get memory usage
    size_t getMemoryUsage() const {
        size_t total = 0;
        if (neuronPool_) total += neuronPool_->memoryUsage();
        if (synapsePool_) total += synapsePool_->memoryUsage();
        return total;
    }
    
    // Reset all pools
    void reset() {
        if (neuronPool_) neuronPool_->reset();
        if (synapsePool_) synapsePool_->reset();
        
        stats_ = MemoryPoolStats();
    }
    
    // Configure pools
    void configure(const MemoryPoolConfig& config) {
        // Update configuration
        config_ = config;
        
        if (config_.enableMemoryPool) {
            // Reconfigure pools
            if (neuronPool_) {
                neuronPool_->configure(
                    config_.neuronPoolBlockSize,
                    config_.neuronInitialBlocks
                );
            }
            if (synapsePool_) {
                synapsePool_->configure(
                    config_.synapsePoolBlockSize,
                    config_.synapseInitialBlocks
                );
            }
        }
    }
    
private:
    MemoryPoolManager() = default;
    ~MemoryPoolManager() = default;
    
    // Disable copying
    MemoryPoolManager(const MemoryPoolManager&) = delete;
    MemoryPoolManager& operator=(const MemoryPoolManager&) = delete;
    
    // Configuration
    MemoryPoolConfig config_;
    
    // Memory pools
    std::unique_ptr<NeuronPool> neuronPool_;
    std::unique_ptr<SynapsePool> synapsePool_;
    
    // Statistics
    MemoryPoolStats stats_;
};

// Convenience functions for common operations
inline NeuronId allocNeuron() {
    return MemoryPoolManager::getInstance().allocateNeuron();
}

inline void freeNeuron(NeuronId id) {
    MemoryPoolManager::getInstance().deallocateNeuron(id);
}

inline SynapseId allocSynapse(NeuronId source, NeuronId destination) {
    return MemoryPoolManager::getInstance().allocateSynapse(source, destination);
}

inline void freeSynapse(SynapseId id) {
    MemoryPoolManager::getInstance().deallocateSynapse(id);
}

// Initialize memory pools from configuration
inline void initializeMemoryPools(const MemoryPoolConfig& config) {
    MemoryPoolManager::getInstance().initialize(config);
}

// Reset all memory pools
inline void resetMemoryPools() {
    MemoryPoolManager::getInstance().reset();
}

// Get memory pool statistics
inline const MemoryPoolStats& getMemoryPoolStats() {
    return MemoryPoolManager::getInstance().getStats();
}

} // namespace nlm

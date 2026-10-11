#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../performance/MemoryPool.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Memory pool configuration for the brain system
struct BrainMemoryPoolConfig {
    // Pool sizes
    size_t neuronPoolBlockSize = 1024;      // Neurons per block
    size_t neuronInitialBlocks = 4;         // Initial neuron blocks
    size_t synapsePoolBlockSize = 4096;      // Synapses per block
    size_t synapseInitialBlocks = 4;        // Initial synapse blocks
    size_t populationPoolBlockSize = 256;    // Populations per block
    size_t populationInitialBlocks = 2;     // Initial population blocks
    size_t regionPoolBlockSize = 128;        // Regions per block
    size_t regionInitialBlocks = 2;         // Initial region blocks
    
    // Performance tuning
    bool enableMemoryPools = true;           // Enable memory pools
    bool enableStatistics = true;            // Track allocation statistics
    bool enableOptimizations = true;         // Enable additional optimizations
    
    // Cache optimization
    size_t cacheLineSize = 64;               // Cache line size
    
    // Memory limits (0 = unlimited)
    size_t maxMemoryUsage = 0;
};

// Memory pool statistics
struct MemoryPoolStats {
    // Allocation statistics
    size_t neuronsAllocated = 0;
    size_t neuronsDeallocated = 0;
    size_t synapsesAllocated = 0;
    size_t synapsesDeallocated = 0;
    size_t populationsAllocated = 0;
    size_t populationsDeallocated = 0;
    size_t regionsAllocated = 0;
    size_t regionsDeallocated = 0;
    
    // Performance metrics
    size_t peakMemoryUsage = 0;
    size_t totalAllocations = 0;
    size_t totalDeallocations = 0;
    double allocationOverhead = 0.0;        // Percentage overhead
    
    // Cache efficiency
    size_t cacheHits = 0;
    size_t cacheMisses = 0;
    double cacheEfficiency = 0.0;
    
    // Memory fragmentation
    size_t memoryFragments = 0;
    double fragmentationRatio = 0.0;
    
    // Pool state
    bool poolsInitialized = false;
    bool optimizationsEnabled = false;
};

// Memory pool manager for the brain system
class BrainMemoryPoolManager {
public:
    static BrainMemoryPoolManager& getInstance() {
        static BrainMemoryPoolManager instance;
        return instance;
    }
    
    // Initialize memory pools
    void initialize(const BrainMemoryPoolConfig& config) {
        config_ = config;
        stats_ = MemoryPoolStats();
        
        if (config_.enableMemoryPools) {
            // Create specialized memory pools
            neuronPool_ = std::make_unique<NeuronPool>(
                config_.neuronPoolBlockSize,
                config_.neuronInitialBlocks
            );
            
            synapsePool_ = std::make_unique<SynapsePool>(
                config_.synapsePoolBlockSize,
                config_.synapseInitialBlocks
            );
            
            // TODO: Add population and region pools when implemented
            // populationPool_ = std::make_unique<PopulationPool>(...);
            // regionPool_ = std::make_unique<RegionPool>(...);
            
            stats_.poolsInitialized = true;
        }
        
        if (config_.enableOptimizations) {
            // Enable additional optimizations
            enableOptimizations();
        }
    }
    
    // Neuron allocation
    NeuronId allocateNeuron() {
        if (!config_.enableMemoryPools || !neuronPool_) {
            // Fallback to dynamic allocation
            static NeuronId nextId = 1;
            return NeuronId(nextId++);
        }
        
        NeuronId id = neuronPool_->allocate();
        if (id.value > 0) {
            stats_.neuronsAllocated++;
            stats_.totalAllocations++;
        }
        return id;
    }
    
    // Neuron deallocation
    void deallocateNeuron(NeuronId id) {
        if (config_.enableMemoryPools && neuronPool_ && id.value > 0) {
            neuronPool_->deallocate(id);
            stats_.neuronsDeallocated++;
            stats_.totalDeallocations++;
        }
    }
    
    // Synapse allocation
    SynapseId allocateSynapse(NeuronId source, NeuronId destination) {
        if (!config_.enableMemoryPools || !synapsePool_) {
            // Fallback to dynamic allocation
            static SynapseId nextId = 1;
            return SynapseId(nextId++);
        }
        
        SynapseId id = synapsePool_->allocate(source, destination);
        if (id.value > 0) {
            stats_.synapsesAllocated++;
            stats_.totalAllocations++;
        }
        return id;
    }
    
    // Synapse deallocation
    void deallocateSynapse(SynapseId id) {
        if (config_.enableMemoryPools && synapsePool_ && id.value > 0) {
            synapsePool_->deallocate(id);
            stats_.synapsesDeallocated++;
            stats_.totalDeallocations++;
        }
    }
    
    // Population allocation (placeholder)
    PopulationId allocatePopulation() {
        // TODO: Implement population pool when available
        static PopulationId nextId = 1;
        stats_.populationsAllocated++;
        stats_.totalAllocations++;
        return PopulationId(nextId++);
    }
    
    void deallocatePopulation(PopulationId id) {
        stats_.populationsDeallocated++;
        stats_.totalDeallocations++;
    }
    
    // Region allocation (placeholder)
    RegionId allocateRegion() {
        // TODO: Implement region pool when available
        static RegionId nextId = 1;
        stats_.regionsAllocated++;
        stats_.totalAllocations++;
        return RegionId(nextId++);
    }
    
    void deallocateRegion(RegionId id) {
        stats_.regionsDeallocated++;
        stats_.totalDeallocations++;
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
        if (neuronPool_) neuronPool_->clear();
        if (synapsePool_) synapsePool_->clear();
        
        // Reset statistics
        stats_ = MemoryPoolStats();
        stats_.poolsInitialized = (config_.enableMemoryPools);
    }
    
    // Configure pools
    void configure(const BrainMemoryPoolConfig& config) {
        config_ = config;
        
        if (config_.enableMemoryPools) {
            if (neuronPool_) {
                // TODO: Configure neuron pool
            }
            if (synapsePool_) {
                // TODO: Configure synapse pool
            }
        }
    }
    
    // Check if memory pools are enabled
    bool areMemoryPoolsEnabled() const { return config_.enableMemoryPools; }
    
    // Enable/disable optimizations
    void enableOptimizations() {
        // TODO: Enable various optimizations (SIMD, cache-friendly access, etc.)
        stats_.optimizationsEnabled = true;
    }
    
    // Disable optimizations
    void disableOptimizations() {
        stats_.optimizationsEnabled = false;
    }
    
    // Get current configuration
    const BrainMemoryPoolConfig& getConfig() const { return config_; }
    
private:
    BrainMemoryPoolManager() = default;
    ~BrainMemoryPoolManager() = default;
    
    // Disable copying
    BrainMemoryPoolManager(const BrainMemoryPoolManager&) = delete;
    BrainMemoryPoolManager& operator=(const BrainMemoryPoolManager&) = delete;
    
    // Configuration
    BrainMemoryPoolConfig config_;
    
    // Memory pools
    std::unique_ptr<NeuronPool> neuronPool_;
    std::unique_ptr<SynapsePool> synapsePool_;
    // TODO: Add population and region pools when implemented
    
    // Statistics
    MemoryPoolStats stats_;
};

// Convenience functions for common operations
inline NeuronId allocNeuron() {
    return BrainMemoryPoolManager::getInstance().allocateNeuron();
}

inline void freeNeuron(NeuronId id) {
    BrainMemoryPoolManager::getInstance().deallocateNeuron(id);
}

inline SynapseId allocSynapse(NeuronId source, NeuronId destination) {
    return BrainMemoryPoolManager::getInstance().allocateSynapse(source, destination);
}

inline void freeSynapse(SynapseId id) {
    BrainMemoryPoolManager::getInstance().deallocateSynapse(id);
}

inline PopulationId allocPopulation() {
    return BrainMemoryPoolManager::getInstance().allocatePopulation();
}

inline void freePopulation(PopulationId id) {
    BrainMemoryPoolManager::getInstance().deallocatePopulation(id);
}

inline RegionId allocRegion() {
    return BrainMemoryPoolManager::getInstance().allocateRegion();
}

inline void freeRegion(RegionId id) {
    BrainMemoryPoolManager::getInstance().deallocateRegion(id);
}

// Initialize memory pools from configuration
inline void initializeMemoryPools(const BrainMemoryPoolConfig& config) {
    BrainMemoryPoolManager::getInstance().initialize(config);
}

// Reset all memory pools
inline void resetMemoryPools() {
    BrainMemoryPoolManager::getInstance().reset();
}

// Get memory pool statistics
inline const MemoryPoolStats& getMemoryPoolStats() {
    return BrainMemoryPoolManager::getInstance().getStats();
}

// Configure memory pools
inline void configureMemoryPools(const BrainMemoryPoolConfig& config) {
    BrainMemoryPoolManager::getInstance().configure(config);
}

// Check if memory pools are enabled
inline bool areMemoryPoolsEnabled() {
    return BrainMemoryPoolManager::getInstance().areMemoryPoolsEnabled();
}

} // namespace nlm

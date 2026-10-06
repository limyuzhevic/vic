#pragma once

// Memory pool headers for NLM brain simulation
// This file includes all memory pool related headers and provides the main memory management interface

#include "core/MemoryPool.hpp"
#include "core/VectorPool.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <iostream>

namespace nlm {

// ==================== FORWARD DECLARATIONS ====================

class Neuron;
class Synapse;
class NeuralRegion;
class NeuralPopulation;
class InterRegionConnection;

// ==================== MEMORY POOL FACTORY ====================

class MemoryPoolFactory {
public:
    static MemoryPoolFactory& getInstance() {
        static MemoryPoolFactory instance;
        return instance;
    }
    
    // Create memory pools for all standard types
    void createDefaultPools() {
        // Neuron pool
        neuronPool = std::make_unique<NeuronMemoryPool>();
        
        // Synapse pool
        synapsePool = std::make_unique<SynapseMemoryPool>();
        
        // Region pool
        regionPool = std::make_unique<RegionMemoryPool>();
        
        // Population pool
        populationPool = std::make_unique<PopulationMemoryPool>();
        
        // Connection pool
        connectionPool = std::make_unique<ConnectionMemoryPool>();
        
        // Vector pools for common types
        vectorPools["Neuron*"] = std::make_unique<VectorMemoryPool<Neuron*>>();
        vectorPools["Synapse*"] = std::make_unique<VectorMemoryPool<Synapse*>>();
        vectorPools["float"] = std::make_unique<VectorMemoryPool<float>>();
        vectorPools["double"] = std::make_unique<VectorMemoryPool<double>>();
        vectorPools["string"] = std::make_unique<VectorMemoryPool<std::string>>();
        vectorPools["Timestamp"] = std::make_unique<VectorMemoryPool<Timestamp>>();
        vectorPools["size_t"] = std::make_unique<VectorMemoryPool<size_t>>();
        vectorPools["int"] = std::make_unique<VectorMemoryPool<int>>();
    }
    
    // Get memory pools
    NeuronMemoryPool& getNeuronPool() { return *neuronPool; }
    SynapseMemoryPool& getSynapsePool() { return *synapsePool; }
    RegionMemoryPool& getRegionPool() { return *regionPool; }
    PopulationMemoryPool& getPopulationPool() { return *populationPool; }
    ConnectionMemoryPool& getConnectionPool() { return *connectionPool; }
    
    VectorMemoryPool<Neuron*>& getNeuronVectorPool() { return *vectorPools["Neuron*"]; }
    VectorMemoryPool<Synapse*>& getSynapseVectorPool() { return *vectorPools["Synapse*"]; }
    VectorMemoryPool<float>& getFloatVectorPool() { return *vectorPools["float"]; }
    VectorMemoryPool<double>& getDoubleVectorPool() { return *vectorPools["double"]; }
    VectorMemoryPool<std::string>& getStringVectorPool() { return *vectorPools["string"]; }
    VectorMemoryPool<Timestamp>& getTimestampVectorPool() { return *vectorPools["Timestamp"]; }
    VectorMemoryPool<size_t>& getSizeTVectorPool() { return *vectorPools["size_t"]; }
    VectorMemoryPool<int>& getIntVectorPool() { return *vectorPools["int"]; }
    
    // Clear all pools
    void clearAllPools() {
        if (neuronPool) neuronPool->clear();
        if (synapsePool) synapsePool->clear();
        if (regionPool) regionPool->clear();
        if (populationPool) populationPool->clear();
        if (connectionPool) connectionPool->clear();
        
        for (auto& pair : vectorPools) {
            pair.second->clear();
        }
    }
    
    // Get memory statistics report
    std::string getMemoryReport() {
        std::ostringstream oss;
        oss << "Memory Pool Report:" << std::endl;
        
        if (neuronPool) {
            oss << "Neuron Pool: " << neuronPool->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        if (synapsePool) {
            oss << "Synapse Pool: " << synapsePool->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        if (regionPool) {
            oss << "Region Pool: " << regionPool->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        if (populationPool) {
            oss << "Population Pool: " << populationPool->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        if (connectionPool) {
            oss << "Connection Pool: " << connectionPool->getStatistics().allocatedCount << " allocated" << std::endl;
        }
        
        return oss.str();
    }
    
    // Check if memory limit is exceeded
    bool isMemoryLimitExceeded() {
        if (!neuronPool || !synapsePool || !regionPool) return false;
        
        size_t totalMemory = neuronPool->getStatistics().allocatedBytes +
                            synapsePool->getStatistics().allocatedBytes +
                            regionPool->getStatistics().allocatedBytes;
        
        return totalMemory > MemoryPoolConfig::MAX_TOTAL_MEMORY_MB * 1024 * 1024;
    }
    
private:
    MemoryPoolFactory() {}
    ~MemoryPoolFactory() {}
    
    std::unique_ptr<NeuronMemoryPool> neuronPool;
    std::unique_ptr<SynapseMemoryPool> synapsePool;
    std::unique_ptr<RegionMemoryPool> regionPool;
    std::unique_ptr<PopulationMemoryPool> populationPool;
    std::unique_ptr<ConnectionMemoryPool> connectionPool;
    std::unordered_map<std::string, std::unique_ptr<VectorMemoryPoolBase>> vectorPools;
};

// ==================== GLOBAL MEMORY POOL ACCESSORS ====================

// Get global memory pool factory instance
inline MemoryPoolFactory& getMemoryPoolFactory() {
    return MemoryPoolFactory::getInstance();
}

// Get neuron memory pool (convenience function)
inline NeuronMemoryPool& getNeuronMemoryPool() {
    return getMemoryPoolFactory().getNeuronPool();
}

// Get synapse memory pool (convenience function)
inline SynapseMemoryPool& getSynapseMemoryPool() {
    return getMemoryPoolFactory().getSynapsePool();
}

// Get region memory pool (convenience function)
inline RegionMemoryPool& getRegionMemoryPool() {
    return getMemoryPoolFactory().getRegionPool();
}

// Get population memory pool (convenience function)
inline PopulationMemoryPool& getPopulationMemoryPool() {
    return getMemoryPoolFactory().getPopulationPool();
}

// Get connection memory pool (convenience function)
inline ConnectionMemoryPool& getConnectionMemoryPool() {
    return getMemoryPoolFactory().getConnectionPool();
}

// Get neuron vector pool (convenience function)
inline VectorMemoryPool<Neuron*>& getNeuronVectorPool() {
    return getMemoryPoolFactory().getNeuronVectorPool();
}

// Get synapse vector pool (convenience function)
inline VectorMemoryPool<Synapse*>& getSynapseVectorPool() {
    return getMemoryPoolFactory().getSynapseVectorPool();
}

// Get float vector pool (convenience function)
inline VectorMemoryPool<float>& getFloatVectorPool() {
    return getMemoryPoolFactory().getFloatVectorPool();
}

// Get double vector pool (convenience function)
inline VectorMemoryPool<double>& getDoubleVectorPool() {
    return getMemoryPoolFactory().getDoubleVectorPool();
}

// Get string vector pool (convenience function)
inline VectorMemoryPool<std::string>& getStringVectorPool() {
    return getMemoryPoolFactory().getStringVectorPool();
}

// Get timestamp vector pool (convenience function)
inline VectorMemoryPool<Timestamp>& getTimestampVectorPool() {
    return getMemoryPoolFactory().getTimestampVectorPool();
}

// Get size_t vector pool (convenience function)
inline VectorMemoryPool<size_t>& getSizeTVectorPool() {
    return getMemoryPoolFactory().getSizeTVectorPool();
}

// Get int vector pool (convenience function)
inline VectorMemoryPool<int>& getIntVectorPool() {
    return getMemoryPoolFactory().getIntVectorPool();
}

// ==================== MEMORY POOL INITIALIZATION AND CLEANUP ====================

// Initialize memory pools (call at program startup)
inline void initializeMemoryPools() {
    getMemoryPoolFactory().createDefaultPools();
}

// Clear all memory pools (call for cleanup)
inline void clearMemoryPools() {
    getMemoryPoolFactory().clearAllPools();
}

// Get memory usage report
inline std::string getMemoryReport() {
    return getMemoryPoolFactory().getMemoryReport();
}

// Check if memory limit is exceeded
inline bool isMemoryLimitExceeded() {
    return getMemoryPoolFactory().isMemoryLimitExceeded();
}

// ==================== CUSTOM ALLOCATOR DELEGATES ====================

// Custom allocator for neurons using memory pool
struct NeuronAllocator {
    typedef Neuron value_type;
    
    Neuron* allocate(std::size_t count) {
        if (count == 1) {
            return getNeuronMemoryPool().allocateNeuron();
        } else {
            return getNeuronMemoryPool().allocateNeuron(); // For now, handle single allocation
        }
    }
    
    void deallocate(Neuron* ptr, std::size_t count) {
        if (count == 1) {
            getNeuronMemoryPool().deallocateNeuron(ptr);
        }
    }
    
    template<typename U>
    struct rebind {
        typedef typename std::allocator_traits<U>::template rebind_alloc<Neuron> other;
    };
};

// Custom allocator for synapses using memory pool
struct SynapseAllocator {
    typedef Synapse value_type;
    
    Synapse* allocate(std::size_t count) {
        if (count == 1) {
            return getSynapseMemoryPool().allocateSynapse();
        } else {
            return getSynapseMemoryPool().allocateSynapse(); // For now, handle single allocation
        }
    }
    
    void deallocate(Synapse* ptr, std::size_t count) {
        if (count == 1) {
            getSynapseMemoryPool().deallocateSynapse(ptr);
        }
    }
    
    template<typename U>
    struct rebind {
        typedef typename std::allocator_traits<U>::template rebind_alloc<Synapse> other;
    };
};

// ==================== MEMORY POOL MONITORING ====================

// Memory pool monitoring functions
namespace MemoryMonitor {
    // Get current memory usage
    inline size_t getCurrentMemoryUsage() {
        size_t total = 0;
        if (auto& pool = getNeuronMemoryPool()) {
            total += pool.getStatistics().allocatedBytes;
        }
        if (auto& pool = getSynapseMemoryPool()) {
            total += pool.getStatistics().allocatedBytes;
        }
        if (auto& pool = getRegionMemoryPool()) {
            total += pool.getStatistics().allocatedBytes;
        }
        return total;
    }
    
    // Get peak memory usage
    inline size_t getPeakMemoryUsage() {
        size_t peak = 0;
        if (auto& pool = getNeuronMemoryPool()) {
            peak = std::max(peak, pool.getStatistics().peakAllocated);
        }
        if (auto& pool = getSynapseMemoryPool()) {
            peak = std::max(peak, pool.getStatistics().peakAllocated);
        }
        if (auto& pool = getRegionMemoryPool()) {
            peak = std::max(peak, pool.getStatistics().peakAllocated);
        }
        return peak;
    }
    
    // Get memory usage by pool type
    inline std::unordered_map<std::string, size_t> getMemoryUsageByPool() {
        std::unordered_map<std::string, size_t> usage;
        
        if (auto& pool = getNeuronMemoryPool()) {
            usage["Neuron"] = pool.getStatistics().allocatedBytes;
        }
        if (auto& pool = getSynapseMemoryPool()) {
            usage["Synapse"] = pool.getStatistics().allocatedBytes;
        }
        if (auto& pool = getRegionMemoryPool()) {
            usage["Region"] = pool.getStatistics().allocatedBytes;
        }
        
        return usage;
    }
    
    // Print memory usage report
    inline void printMemoryReport() {
        std::cout << getMemoryPoolFactory().getMemoryReport();
    }
}

} // namespace nlm

// Include the original memory pool headers for backward compatibility
#include "core/MemoryPool.hpp"
#include "core/VectorPool.hpp"
#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Memory pool for neuron allocations
template<typename T>
class NeuronMemoryPool {
public:
    explicit NeuronMemoryPool(size_t initialCapacity = 1024, size_t maxCapacity = 65536)
        : initialCapacity(initialCapacity), maxCapacity(maxCapacity), allocated(0), freed(0) {
        buffer.reserve(initialCapacity);
    }
    
    ~NeuronMemoryPool() {
        // All memory will be released when buffer goes out of scope
    }
    
    T* allocate() {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (buffer.size() < initialCapacity) {
            buffer.resize(initialCapacity * 2);
        }
        if (allocated < buffer.size()) {
            return &buffer[allocated++];
        }
        throw std::bad_alloc();
    }
    
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(poolMutex);
        // Find the position in buffer (simple linear search, could be optimized with hash map)
        for (size_t i = 0; i < allocated; ++i) {
            if (&buffer[i] == ptr) {
                // Mark as freed by swapping with last element and decreasing allocated count
                buffer[i] = buffer[allocated - 1];
                allocated--;
                freed++;
                return;
            }
        }
        // Pointer not found in buffer
        throw std::runtime_error("Attempted to deallocate invalid pointer");
    }
    
    size_t getAllocatedCount() const { return allocated; }
    size_t getTotalCapacity() const { return buffer.capacity(); }
    size_t getFreedCount() const { return freed; }
    
    // Enable placement new to construct objects in pre-allocated memory
    template<typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        return new(ptr) T(std::forward<Args>(args)...);
    }
    
    // Destroy and deallocate
    void destroyAndDeallocate(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }
    
    // Resize pool to accommodate more objects
    void resize(size_t newCapacity) {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (newCapacity > maxCapacity) {
            throw std::runtime_error("Cannot exceed maximum pool capacity");
        }
        buffer.resize(newCapacity);
    }
    
private:
    std::vector<T> buffer;
    mutable std::mutex poolMutex;
    size_t initialCapacity;
    size_t maxCapacity;
    size_t allocated;
    size_t freed;
};

// Memory pool for synapse allocations
template<typename T>
class SynapseMemoryPool {
public:
    explicit SynapseMemoryPool(size_t initialCapacity = 4096, size_t maxCapacity = 32768)
        : initialCapacity(initialCapacity), maxCapacity(maxCapacity), allocated(0), freed(0) {
        buffer.reserve(initialCapacity);
    }
    
    ~SynapseMemoryPool() {
        // All memory will be released when buffer goes out of scope
    }
    
    T* allocate() {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (buffer.size() < initialCapacity) {
            buffer.resize(initialCapacity * 2);
        }
        if (allocated < buffer.size()) {
            return &buffer[allocated++];
        }
        throw std::bad_alloc();
    }
    
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(poolMutex);
        // Find the position in buffer (simple linear search, could be optimized with hash map)
        for (size_t i = 0; i < allocated; ++i) {
            if (&buffer[i] == ptr) {
                // Mark as freed by swapping with last element and decreasing allocated count
                buffer[i] = buffer[allocated - 1];
                allocated--;
                freed++;
                return;
            }
        }
        // Pointer not found in buffer
        throw std::runtime_error("Attempted to deallocate invalid pointer");
    }
    
    size_t getAllocatedCount() const { return allocated; }
    size_t getTotalCapacity() const { return buffer.capacity(); }
    size_t getFreedCount() const { return freed; }
    
    // Enable placement new to construct objects in pre-allocated memory
    template<typename... Args>
    T* construct(Args&&... args) {
        T* ptr = allocate();
        return new(ptr) T(std::forward<Args>(args)...);
    }
    
    // Destroy and deallocate
    void destroyAndDeallocate(T* ptr) {
        if (ptr) {
            ptr->~T();
            deallocate(ptr);
        }
    }
    
    // Resize pool to accommodate more objects
    void resize(size_t newCapacity) {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (newCapacity > maxCapacity) {
            throw std::runtime_error("Cannot exceed maximum pool capacity");
        }
        buffer.resize(newCapacity);
    }
    
private:
    std::vector<T> buffer;
    mutable std::mutex poolMutex;
    size_t initialCapacity;
    size_t maxCapacity;
    size_t allocated;
    size_t freed;
};

// Central memory pool manager that coordinates all memory pools
class MemoryPoolManager {
public:
    MemoryPoolManager() {
        // Initialize pools with reasonable default capacities
        neuronPool = std::make_unique<NeuronMemoryPool<Neuron>>("default_neuron_pool");
        synapsePool = std::make_unique<SynapseMemoryPool<Synapse>>("default_synapse_pool");
        
        // Pre-allocate some memory for initial use
        neuronPool->resize(1024);
        synapsePool->resize(2048);
    }
    
    ~MemoryPoolManager() {
        // Clean up all pools
    }
    
    // Get or create a neuron memory pool for a specific region/population
    NeuronMemoryPool<Neuron>* getNeuronPool(const std::string& key) {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (neuronPools.find(key) == neuronPools.end()) {
            // Create new pool with region/population-specific size
            size_t poolSize = calculatePoolSize(key, "neuron");
            auto pool = std::make_unique<NeuronMemoryPool<Neuron>>(key);
            pool->resize(poolSize);
            neuronPools[key] = std::move(pool);
        }
        return neuronPools[key].get();
    }
    
    // Get or create a synapse memory pool for a specific region/population
    SynapseMemoryPool<Synapse>* getSynapsePool(const std::string& key) {
        std::lock_guard<std::mutex> lock(poolMutex);
        if (synapsePools.find(key) == synapsePools.end()) {
            // Create new pool with region/population-specific size
            size_t poolSize = calculatePoolSize(key, "synapse");
            auto pool = std::make_unique<SynapseMemoryPool<Synapse>>(key);
            pool->resize(poolSize);
            synapsePools[key] = std::move(pool);
        }
        return synapsePools[key].get();
    }
    
    // Global neuron pool access (for backward compatibility)
    NeuronMemoryPool<Neuron>* getGlobalNeuronPool() {
        return neuronPool.get();
    }
    
    // Global synapse pool access (for backward compatibility)
    SynapseMemoryPool<Synapse>* getGlobalSynapsePool() {
        return synapsePool.get();
    }
    
    // Calculate pool size based on key type and characteristics
    size_t calculatePoolSize(const std::string& key, const std::string& type) {
        // Base pool size
        size_t baseSize = 512;
        
        // Scale based on key characteristics
        if (type == "neuron") {
            // Neural regions typically have more neurons than synapses
            if (key.find("cortical") != std::string::npos) {
                return baseSize * 4;  // Cortical regions are larger
            } else if (key.find("hippocampal") != std::string::npos) {
                return baseSize * 2;
            } else {
                return baseSize;
            }
        } else if (type == "synapse") {
            // Synapses are typically more numerous than neurons
            if (key.find("cortical") != std::string::npos) {
                return baseSize * 8;
            } else if (key.find("hippocampal") != std::string::npos) {
                return baseSize * 4;
            } else {
                return baseSize * 2;
            }
        }
        
        return baseSize;
    }
    
    // Get memory statistics for monitoring
    struct MemoryStats {
        size_t totalNeuronMemoryAllocated;
        size_t totalSynapseMemoryAllocated;
        size_t peakNeuronMemoryUsage;
        size_t peakSynapseMemoryUsage;
        double memoryFragmentationRatio;
    };
    
    MemoryStats getMemoryStats() const {
        MemoryStats stats;
        stats.totalNeuronMemoryAllocated = globalNeuronPool.getAllocatedCount();
        stats.totalSynapseMemoryAllocated = globalSynapsePool.getAllocatedCount();
        stats.peakNeuronMemoryUsage = globalNeuronPool.getTotalCapacity();
        stats.peakSynapseMemoryUsage = globalSynapsePool.getTotalCapacity();
        
        // Calculate fragmentation ratio (simplified)
        size_t totalAllocated = stats.totalNeuronMemoryAllocated + stats.totalSynapseMemoryAllocated;
        size_t totalCapacity = stats.peakNeuronMemoryUsage + stats.peakSynapseMemoryUsage;
        stats.memoryFragmentationRatio = totalAllocated > 0 ? 
            static_cast<double>(totalCapacity - totalAllocated) / totalCapacity : 0.0;
        
        return stats;
    }
    
    // Clean up unused pools to free memory
    void cleanupUnusedPools() {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        // Remove pools with zero allocated objects
        auto neuronIt = neuronPools.begin();
        while (neuronIt != neuronPools.end()) {
            if (neuronIt->second->getAllocatedCount() == 0) {
                neuronIt = neuronPools.erase(neuronIt);
            } else {
                ++neuronIt;
            }
        }
        
        auto synapseIt = synapsePools.begin();
        while (synapseIt != synapsePools.end()) {
            if (synapseIt->second->getAllocatedCount() == 0) {
                synapseIt = synapsePools.erase(synapseIt);
            } else {
                ++synapseIt;
            }
        }
    }
    
private:
    // Global default pools
    std::unique_ptr<NeuronMemoryPool<Neuron>> neuronPool;
    std::unique_ptr<SynapseMemoryPool<Synapse>> synapsePool;
    
    // Per-region/population pools
    std::unordered_map<std::string, std::unique_ptr<NeuronMemoryPool<Neuron>>> neuronPools;
    std::unordered_map<std::string, std::unique_ptr<SynapseMemoryPool<Synapse>>> synapsePools;
    mutable std::mutex poolMutex;
};

// Global memory pool manager instance
extern MemoryPoolManager& getMemoryPoolManager();

} // namespace nlm

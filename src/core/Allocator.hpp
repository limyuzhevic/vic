#pragma once

/**
 * Custom allocators for neural system types
 * 
 * Provides optimized memory allocators for frequently used types in the neural system,
 * reducing allocation overhead and improving cache locality.
 * 
 * Features:
 * - Pool-based allocation for frequently used types
 * - Thread-local storage for allocation efficiency
 * - Alignment optimizations for better cache performance
 * - Integration with existing MemoryPool infrastructure
 * 
 * Motivation:
 * - Neural systems have predictable allocation patterns
 * - Custom allocators reduce memory fragmentation
 * - Improves allocation performance and cache locality
 */

#include "../performance/MemoryPool.hpp"
#include "Types/Types.hpp"
#include <memory>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace nlm {

/**
 * Allocator for Neuron objects with cache-aware alignment
 */
class NeuronAllocator {
public:
    static Neuron* allocate();
    static void deallocate(Neuron* ptr);
    static void reserve(size_t count);
    static size_t allocatedCount();
    static size_t availableCount();
    
private:
    static NeuronPool neuronPool_;
    static std::mutex mutex_;
    
    friend class AllocatorRegistry;
};

/**
 * Allocator for Synapse objects with optimization for synaptic connections
 */
class SynapseAllocator {
public:
    static Synapse* allocate(NeuronId source, NeuronId destination);
    static void deallocate(Synapse* ptr);
    static void reserve(size_t count);
    static size_t allocatedCount();
    static size_t availableCount();
    
private:
    static SynapsePool synapsePool_;
    static std::mutex mutex_;
    
    friend class AllocatorRegistry;
};

/**
 * Allocator for NeuralRegion objects
 */
class NeuralRegionAllocator {
public:
    static NeuralRegion* allocate(const std::string& name);
    static void deallocate(NeuralRegion* ptr);
    static void reserve(size_t count);
    static size_t allocatedCount();
    static size_t availableCount();
    
private:
    static MemoryPool<NeuralRegion> regionPool_;
    static std::mutex mutex_;
    
    friend class AllocatorRegistry;
};

/**
 * Allocator for std::vector<float> with SIMD alignment
 */
class VectorFloatAllocator {
public:
    static float* allocate(size_t count);
    static void deallocate(float* ptr, size_t count);
    static void reserve(size_t count);
    static size_t allocatedCount();
    static size_t availableCount();
    
private:
    static MemoryPool<float> vectorPool_;
    static std::mutex mutex_;
    
    friend class AllocatorRegistry;
};

/**
 * Allocator for std::vector<NeuronId> with cache-friendly layout
 */
class NeuronIdVectorAllocator {
public:
    static NeuronId* allocate(size_t count);
    static void deallocate(NeuronId* ptr, size_t count);
    static void reserve(size_t count);
    static size_t allocatedCount();
    static size_t availableCount();
    
private:
    static MemoryPool<NeuronId> neuronIdPool_;
    static std::mutex mutex_;
    
    friend class AllocatorRegistry;
};

/**
 * Registry for managing all allocators
 */
class AllocatorRegistry {
public:
    static AllocatorRegistry& getInstance();
    
    // Register allocators
    void registerAllocator(const std::string& typeName, void* allocator);
    void unregisterAllocator(const std::string& typeName);
    
    // Get allocator by type
    template<typename AllocatorType>
    static AllocatorType* getAllocator() {
        return reinterpret_cast<AllocatorType*>(getInstance().getAllocatorImpl(typeid(AllocatorType).name()));
    }
    
    // Global allocation functions using registered allocators
    static void* allocate(const std::string& typeName, size_t size = 1);
    static void deallocate(const std::string& typeName, void* ptr, size_t size = 1);
    
    // Statistics and monitoring
    static size_t getTotalAllocated(const std::string& typeName);
    static size_t getAvailable(const std::string& typeName);
    static size_t getMemoryUsage(const std::string& typeName);
    
    // Reserve memory for specific types
    static void reserve(const std::string& typeName, size_t count);
    
    // Clear all allocators
    static void clearAll();
    
private:
    AllocatorRegistry();
    ~AllocatorRegistry();
    
    void* getAllocatorImpl(const std::string& typeName);
    
    std::unordered_map<std::string, void*> allocators_;
    std::mutex registryMutex_;
    
    // Performance counters
    std::unordered_map<std::string, std::atomic<size_t>> allocatedCounts_;
    std::unordered_map<std::string, std::atomic<size_t>> availableCounts_;
    std::mutex countersMutex_;
};

/**
 * Helper function to enable custom allocators for std::vector
 * 
 * Note: This is a template that should be specialized for each type
 * that needs custom allocation. In practice, you'd use specific
 * allocator types for your use cases.
 */
template<typename T>
class VectorAllocator {
public:
    using value_type = T;
    
    VectorAllocator() = default;
    
    template<typename U>
    VectorAllocator(const VectorAllocator<U>&) {}
    
    value_type* allocate(std::size_t n) {
        return static_cast<value_type*>(AllocatorRegistry::allocate(typeid(value_type).name(), n * sizeof(value_type)));
    }
    
    void deallocate(value_type* p, std::size_t n) {
        AllocatorRegistry::deallocate(typeid(value_type).name(), p, n * sizeof(value_type));
    }
    
    std::size_t max_size() const {
        return std::numeric_limits<std::size_t>::max() / sizeof(value_type);
    }
    
    template<typename U>
    bool operator==(const VectorAllocator<U>&) const { return true; }
    
    template<typename U>
    bool operator!=(const VectorAllocator<U>&) const { return false; }
};

/**
 * Convenience functions for common allocation patterns
 */
namespace Alloc {
    // Neuron allocation
    inline Neuron* neuron() { return NeuronAllocator::allocate(); }
    inline void neuron(Neuron* ptr) { NeuronAllocator::deallocate(ptr); }
    inline void neuronReserve(size_t count) { NeuronAllocator::reserve(count); }
    
    // Synapse allocation
    inline Synapse* synapse(NeuronId source, NeuronId destination) { return SynapseAllocator::allocate(source, destination); }
    inline void synapse(Synapse* ptr) { SynapseAllocator::deallocate(ptr); }
    inline void synapseReserve(size_t count) { SynapseAllocator::reserve(count); }
    
    // NeuralRegion allocation
    inline NeuralRegion* neuralRegion(const std::string& name) { return NeuralRegionAllocator::allocate(name); }
    inline void neuralRegion(NeuralRegion* ptr) { NeuralRegionAllocator::deallocate(ptr); }
    inline void neuralRegionReserve(size_t count) { NeuralRegionAllocator::reserve(count); }
    
    // Vector allocation
    template<typename T>
    inline T* vector(size_t count) { return static_cast<T*>(AllocatorRegistry::allocate(typeid(T).name(), count * sizeof(T))); }
    
    template<typename T>
    inline void vector(T* ptr, size_t count) { AllocatorRegistry::deallocate(typeid(T).name(), ptr, count * sizeof(T)); }
    
    // SIMD-aligned vector allocation
    inline float* simdVector(size_t count) {
        // Allocate with SIMD alignment for better vectorization
        return VectorFloatAllocator::allocate(count);
    }
    
    inline void simdVector(float* ptr, size_t count) {
        VectorFloatAllocator::deallocate(ptr, count);
    }
}

} // namespace nlm
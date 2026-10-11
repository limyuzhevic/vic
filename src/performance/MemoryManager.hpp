#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <algorithm>

namespace nlm {

// Memory pool for efficient allocation/deallocation
// Reduces memory fragmentation and improves cache locality

template<typename T>
class ObjectPool {
public:
    ObjectPool(size_t initialCapacity = 1024) 
        : capacity_(initialCapacity) {
        objects_.reserve(capacity_);
    }
    
    ~ObjectPool() {
        // All objects should be returned to the pool
        clear();
    }
    
    // Allocate new object (constructs in place)
    T* allocate() {
        if (freeList_.empty()) {
            grow();
        }
        T* obj = freeList_.back();
        freeList_.pop_back();
        return obj;
    }
    
    // Deallocate object (returns to free list)
    void deallocate(T* obj) {
        freeList_.push_back(obj);
    }
    
    // Reset all objects to pool state
    void clear() {
        for (size_t i = 0; i < objects_.size(); ++i) {
            objects_[i]->~T();
        }
        objects_.clear();
        freeList_.clear();
    }
    
    // Get total number of objects allocated
    size_t size() const { return objects_.size() - freeList_.size(); }
    
    // Get current capacity
    size_t capacity() const { return objects_.size(); }
    
private:
    void grow() {
        size_t newSize = objects_.size() + capacity_;
        objects_.resize(newSize);
        
        for (size_t i = objects_.size() - capacity_; i < newSize; ++i) {
            new (&objects_[i]) T();
            freeList_.push_back(&objects_[i]);
        }
    }
    
    std::vector<T> objects_;
    std::vector<T*> freeList_;
    size_t capacity_;
};

// SIMD-optimized vector operations for neural computations
namespace simd {

// SIMD helper functions for neural dynamics
class SimdUtils {
public:
    // Apply leaky integrate-and-fire dynamics to membrane potential array
    static void applyLIFDynamics(float* potentials, float* currents, float* thresholds,
                                float leakConductance, float restingPotential,
                                float dt, size_t count);
    
    // Compute synaptic inputs using vector operations
    static void computeSynapticInputs(float* inputs, const float* weights, const float* activities,
                                     size_t count);
    
    // Apply STDP learning rule using SIMD
    static void applySTDP(float* preTraces, float* postTraces, float* weights,
                         const float* preSpikes, const float* postSpikes,
                         float ltpWeight, float ltdWeight, float tau,
                         size_t count);
    
    // Clamp values to range using SIMD
    static void clampValues(float* values, float min, float max, size_t count);
    
    // Copy array with SIMD
    static void copyArray(float* dest, const float* src, size_t count);
    
    // Compare arrays for equality
    static bool compareArrays(const float* a, const float* b, size_t count, float epsilon = 0.001f);
};

// Memory-efficient SIMD vector allocator
class SimdVectorAllocator {
public:
    // Allocate aligned memory for SIMD operations
    static float* allocateAligned(size_t count);
    
    // Deallocate aligned memory
    static void deallocateAligned(float* ptr);
    
    // Check if pointer is aligned
    static bool isAligned(const void* ptr, size_t alignment = 32);
    
    // Prefetch memory for better cache performance
    static void prefetchRead(const void* ptr);
    static void prefetchWrite(void* ptr);
};

} // namespace simd

// Neural network memory manager
class NeuralMemoryManager {
public:
    NeuralMemoryManager();
    ~NeuralMemoryManager();
    
    // Get singleton instance
    static NeuralMemoryManager& instance();
    
    // Neuron pool
    ObjectPool<Neuron>* getNeuronPool() { return neuronPool_.get(); }
    
    // Synapse pool  
    ObjectPool<Synapse>* getSynapsePool() { return synapsePool_.get(); }
    
    // Memory statistics
    struct MemoryStats {
        size_t totalMemory;      // Total allocated memory in bytes
        size_t neuronCount;      // Number of neurons
        size_t synapseCount;     // Number of synapses
        size_t fragmentation;    // Memory fragmentation estimate
    };
    
    MemoryStats getStats() const;
    
    // Performance monitoring
    struct PerformanceMetrics {
        double totalAllocationTime;    // Time spent allocating memory
        double totalDeallocationTime;  // Time spent deallocating memory
        size_t allocationCount;        // Number of allocations
        size_t deallocationCount;      // Number of deallocations
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Enable/disable SIMD optimizations
    void setEnableSimd(bool enable) { enableSimd_ = enable; }
    bool isSimdEnabled() const { return enableSimd_; }
    
    // Optimize memory layout for cache efficiency
    void optimizeMemoryLayout();
    
private:
    std::unique_ptr<ObjectPool<Neuron>> neuronPool_;
    std::unique_ptr<ObjectPool<Synapse>> synapsePool_;
    bool enableSimd_;
    
    // Prevent copying
    NeuralMemoryManager(const NeuralMemoryManager&) = delete;
    NeuralMemoryManager& operator=(const NeuralMemoryManager&) = delete;
};

// SIMD-optimized neural state container
class SimdNeuralState {
public:
    SimdNeuralState();
    ~SimdNeuralState();
    
    // Add neuron to state
    void addNeuron(Neuron* neuron);
    
    // Remove neuron from state
    void removeNeuron(Neuron* neuron);
    
    // Update all neurons using SIMD
    void updateAllNeurons(double timestep);
    
    // Get spike count using SIMD
    size_t getSpikeCount() const;
    
    // Get average firing rate using SIMD
    float getAverageFiringRate() const;
    
    // Check if any neuron is firing
    bool hasSpikes() const;
    
    // Get all neurons
    const std::vector<Neuron*>& getNeurons() const { return neurons_; }
    
private:
    std::vector<Neuron*> neurons_;
    float* simdPotentialBuffer_;      // Aligned buffer for SIMD operations
    float* simdCurrentBuffer_;        // Aligned buffer for SIMD operations
    float* simdThresholdBuffer_;      // Aligned buffer for SIMD operations
    size_t neuronCount_;
};

// Performance monitoring decorator for memory operations
class PerformanceMonitoringMemoryPool {
public:
    template<typename T>
    class DecoratedPool {
    public:
        DecoratedPool(std::unique_ptr<ObjectPool<T>> pool)
            : pool_(std::move(pool)), allocationCount_(0), deallocationCount_(0) {}
        
        T* allocate() {
            allocationCount_++;
            return pool_->allocate();
        }
        
        void deallocate(T* obj) {
            deallocationCount_++;
            pool_->deallocate(obj);
        }
        
        // Forward other methods
        void clear() { pool_->clear(); }
        size_t size() const { return pool_->size(); }
        size_t capacity() const { return pool_->capacity(); }
        
        // Get performance metrics
        struct Metrics {
            size_t allocationCount;
            size_t deallocationCount;
            double allocationTime;
            double deallocationTime;
        };
        
        Metrics getMetrics() const {
            return {allocationCount_, deallocationCount_, 0.0, 0.0};
        }
        
    private:
        std::unique_ptr<ObjectPool<T>> pool_;
        size_t allocationCount_;
        size_t deallocationCount_;
    };
};

} // namespace nlm

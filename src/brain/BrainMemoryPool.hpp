#pragma once

#include "../performance/MemoryPool.hpp"
#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <atomic>

namespace nlm {

/**
 * Specialized memory pools for brain system objects
 * 
 * This file provides optimized memory pools for frequently allocated
 * brain system objects to reduce heap allocation overhead and improve
 * cache locality during neural simulation.
 */

/**
 * Pool for SpikeEvent objects used by SpikeSystem
 */
class SpikeEventPool {
public:
    SpikeEventPool(size_t poolSize = 1024, size_t blockSize = 128);
    ~SpikeEventPool();
    
    // Disable copying
    SpikeEventPool(const SpikeEventPool&) = delete;
    SpikeEventPool& operator=(const SpikeEventPool&) = delete;
    
    // Allocate a spike event
    SpikeEvent* allocate(NeuronId sourceNeuron, Timestamp timestamp, SimulationStep step);
    
    // Deallocate a spike event
    void deallocate(SpikeEvent* event);
    
    // Get count of allocated events
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    
    // Reserve capacity
    void reserve(size_t count) { pool_.reserve(count); }
    
    // Clear pool (mark all objects as free)
    void clear();
    
private:
    MemoryPool<SpikeEvent> pool_;
};

/**
 * Pool for NeuronState objects used by Neuron
 */
class NeuronStatePool {
public:
    NeuronStatePool(size_t poolSize = 1024, size_t blockSize = 128);
    ~NeuronStatePool();
    
    // Disable copying
    NeuronStatePool(const NeuronStatePool&) = delete;
    NeuronStatePool& operator=(const NeuronStatePool&) = delete;
    
    // Allocate a neuron state
    NeuronState* allocate();
    
    // Deallocate a neuron state
    void deallocate(NeuronState* state);
    
    // Get count of allocated states
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    
    // Reserve capacity
    void reserve(size_t count) { pool_.reserve(count); }
    
    // Clear pool (mark all objects as free)
    void clear();
    
    // Initialize state with default values
    void initialize(NeuronState* state);
    
private:
    MemoryPool<NeuronState> pool_;
};

/**
 * Pool for EpisodicMemoryItem objects used by NeuralEpisodicMemory
 */
class EpisodicMemoryItemPool {
public:
    EpisodicMemoryItemPool(size_t poolSize = 512, size_t blockSize = 64);
    ~EpisodicMemoryItemPool();
    
    // Disable copying
    EpisodicMemoryItemPool(const EpisodicMemoryItemPool&) = delete;
    EpisodicMemoryItemPool& operator=(const EpisodicMemoryItemPool&) = delete;
    
    // Allocate a memory item
    EpisodicMemoryItem* allocate();
    
    // Deallocate a memory item
    void deallocate(EpisodicMemoryItem* item);
    
    // Get count of allocated items
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    
    // Reserve capacity
    void reserve(size_t count) { pool_.reserve(count); }
    
    // Clear pool (mark all objects as free)
    void clear();
    
    // Initialize item with default values
    void initialize(EpisodicMemoryItem* item);
    
private:
    MemoryPool<EpisodicMemoryItem> pool_;
};

/**
 * Pool for NeuralPrediction objects used by NeuralPrediction
 * 
 * Note: NeuralPrediction is a complex object with multiple members.
 * This pool handles the main prediction data structure.
 */
class NeuralPredictionDataPool {
public:
    NeuralPredictionDataPool(size_t poolSize = 256, size_t blockSize = 32);
    ~NeuralPredictionDataPool();
    
    // Disable copying
    NeuralPredictionDataPool(const NeuralPredictionDataPool&) = delete;
    NeuralPredictionDataPool& operator=(const NeuralPredictionDataPool&) = delete;
    
    // Allocate prediction data
    NeuralPrediction* allocate();
    
    // Deallocate prediction data
    void deallocate(NeuralPrediction* data);
    
    // Get count of allocated items
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    
    // Reserve capacity
    void reserve(size_t count) { pool_.reserve(count); }
    
    // Clear pool (mark all objects as free)
    void clear();
    
    // Initialize prediction data with default values
    void initialize(NeuralPrediction* data);
    
private:
    MemoryPool<NeuralPrediction> pool_;
};

/**
 * Pool for DelayedSpikeEvent objects used by SpikeSystem
 */
class DelayedSpikeEventPool {
public:
    DelayedSpikeEventPool(size_t poolSize = 1024, size_t blockSize = 128);
    ~DelayedSpikeEventPool();
    
    // Disable copying
    DelayedSpikeEventPool(const DelayedSpikeEventPool&) = delete;
    DelayedSpikeEventPool& operator=(const DelayedSpikeEventPool&) = delete;
    
    // Allocate a delayed spike event
    DelayedSpikeEvent* allocate(NeuronId source, NeuronId dest, SynapseId syn,
                               SynapticWeight weight, SynapseType type,
                               Timestamp ts, Timestamp delivery,
                               SimulationStep step, SimulationStep deliveryStep);
    
    // Deallocate a delayed spike event
    void deallocate(DelayedSpikeEvent* event);
    
    // Get count of allocated events
    size_t allocatedCount() const { return pool_.allocatedCount(); }
    size_t availableCount() const { return pool_.availableCount(); }
    
    // Reserve capacity
    void reserve(size_t count) { pool_.reserve(count); }
    
    // Clear pool (mark all objects as free)
    void clear();
    
private:
    MemoryPool<DelayedSpikeEvent> pool_;
};

/**
 * Vector pool for pre-allocating reusable temporary vectors
 * 
 * Used in Brain::step() loop to avoid repeated vector allocations
 */
class VectorPool {
public:
    VectorPool(size_t poolSize = 512, size_t maxVectorSize = 10000);
    ~VectorPool();
    
    // Disable copying
    VectorPool(const VectorPool&) = delete;
    VectorPool& operator=(const VectorPool&) = delete;
    
    template<typename T>
    std::vector<T>* getVector(size_t capacity = 0) {
        size_t index = poolIndex_.fetch_add(1) % poolSize_;
        
        if (pools_[index].empty()) {
            pools_[index].reserve(maxVectorSize_);
        }
        
        void* mem = nullptr;
        if (!pools_[index].empty()) {
            mem = pools_[index].back();
            pools_[index].pop_back();
        } else {
            // Create a new vector
            mem = new std::vector<uint8_t>(capacity * sizeof(T));
        }
        
        // Reinterpret the memory as a vector
        return reinterpret_cast<std::vector<T>*>(mem);
    }
    
    template<typename T>
    void returnVector(std::vector<T>* vector) {
        if (!vector) return;
        
        if (vector->capacity() > maxVectorSize_) {
            delete vector;
            return;
        }
        
        void* mem = vector->data();
        std::vector<void*>& pool = pools_[poolIndex_.load() % poolSize_];
        pool.push_back(mem);
        vector->~vector<T>();
    }

/**
 * Composite memory pool manager for brain systems
 * 
 * Provides access to all specialized pools through a single interface
 */
class BrainMemoryPool {
public:
    BrainMemoryPool(const BrainMemoryPool&) = delete;
    BrainMemoryPool& operator=(const BrainMemoryPool&) = delete;
    
    static BrainMemoryPool& getInstance();
    
    // Get individual pools
    SpikeEventPool& getSpikeEventPool() { return spikeEventPool_; }
    NeuronStatePool& getNeuronStatePool() { return neuronStatePool_; }
    EpisodicMemoryItemPool& getEpisodicMemoryItemPool() { return episodicMemoryItemPool_; }
    NeuralPredictionDataPool& getNeuralPredictionDataPool() { return neuralPredictionDataPool_; }
    DelayedSpikeEventPool& getDelayedSpikeEventPool() { return delayedSpikeEventPool_; }
    VectorPool& getVectorPool() { return vectorPool_; }
    
    // Initialize all pools
    void initialize(size_t neuronCount, size_t synapseCount, size_t spikeCount,
                    size_t memoryCount, size_t predictionCount);
    
    // Clear all pools
    void clearAllPools();
    
    // Get total memory usage
    size_t getTotalMemoryUsage() const;
    
    // Statistics
    struct Stats {
        size_t spikeEvents{0};
        size_t neuronStates{0};
        size_t memoryItems{0};
        size_t predictionData{0};
        size_t delayedSpikes{0};
    };
    
    Stats getStats() const;
    
private:
    BrainMemoryPool();
    ~BrainMemoryPool();
    
    SpikeEventPool spikeEventPool_;
    NeuronStatePool neuronStatePool_;
    EpisodicMemoryItemPool episodicMemoryItemPool_;
    NeuralPredictionDataPool neuralPredictionDataPool_;
    DelayedSpikeEventPool delayedSpikeEventPool_;
    VectorPool vectorPool_;
};

} // namespace nlm
#pragma once

/**
 * MemoryPoolManager - Global memory pool manager for neural simulation
 * 
 * Manages specialized memory pools for neurons and synapses based on their types.
 * Provides centralized allocation/deallocation with statistics and performance monitoring.
 * 
 * Features:
 * - Specialized pools for each neuron type (excitatory, inhibitory, sensory, motor, internal)
 * - Specialized pools for each synapse type (excitatory, inhibitory, electrical)
 * - Global allocation/deallocation with type tracking
 * - Memory usage statistics and performance monitoring
 * - Thread-safe operations
 * 
 * Biological motivation:
 * - Different neuron types have different functions and memory requirements
 * - Synapses have different properties (excitatory vs inhibitory vs electrical)
 * - Efficient memory management supports large-scale neural simulations
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <chrono>
#include <deque>
#include <string>

namespace nlm {

class Config;
class Logger;

class MemoryPoolManager {
public:
    /**
     * Create memory pool manager with configuration
     */
    explicit MemoryPoolManager(std::shared_ptr<Config> config);
    
    ~MemoryPoolManager();
    
    // Disable copying
    MemoryPoolManager(const MemoryPoolManager&) = delete;
    MemoryPoolManager& operator=(const MemoryPoolManager&) = delete;
    
    /**
     * Get singleton instance
     */
    static MemoryPoolManager& getInstance();
    
    // Neuron pool management
    /**
     * Allocate a neuron from the appropriate pool based on type
     */
    NeuronId allocateNeuron(NeuronType type);
    
    /**
     * Deallocate a neuron back to its pool
     */
    void deallocateNeuron(NeuronId id);
    
    /**
     * Register neuron type for later deallocation tracking
     */
    void registerNeuronType(NeuronId id, NeuronType type);
    
    // Synapse pool management
    /**
     * Allocate a synapse from the appropriate pool based on type
     */
    SynapseId allocateSynapse(SynapseType type);
    
    /**
     * Deallocate a synapse back to its pool
     */
    void deallocateSynapse(SynapseId id);
    
    /**
     * Register synapse type for later deallocation tracking
     */
    void registerSynapseType(SynapseId id, SynapseType type);
    
    // Global allocation/deallocation
    struct AllocationInfo {
        void* ptr;
        size_t size;
        size_t alignment;
    };
    
    template<typename T>
    AllocationInfo allocate();
    
    template<typename T>
    void deallocate(T* ptr);
    
    // Performance monitoring
    /**
     * Report memory pool performance statistics
     */
    void reportPerformance();
    
    /**
     * Reset all performance statistics
     */
    void resetStats();
    
    // Individual pool accessors
    /**
     * Get specialized pool for excitatory neurons
     */
    NeuronPool& getExcitatoryNeuronPool();
    
    /**
     * Get specialized pool for inhibitory neurons
     */
    NeuronPool& getInhibitoryNeuronPool();
    
    /**
     * Get specialized pool for sensory neurons
     */
    NeuronPool& getSensoryNeuronPool();
    
    /**
     * Get specialized pool for motor neurons
     */
    NeuronPool& getMotorNeuronPool();
    
    /**
     * Get specialized pool for internal neurons
     */
    NeuronPool& getInternalNeuronPool();
    
    /**
     * Get specialized pool for excitatory synapses
     */
    SynapsePool& getExcitatorySynapsePool();
    
    /**
     * Get specialized pool for inhibitory synapses
     */
    SynapsePool& getInhibitorySynapsePool();
    
    /**
     * Get specialized pool for electrical synapses
     */
    SynapsePool& getElectricalSynapsePool();
    
    // Statistics
    size_t getAllocatedCount() const;
    size_t getPeakUsage() const;
    size_t getTotalAllocated() const;
    
    // Performance metrics
    double getAllocationTime() const;
    double getDeallocationTime() const;
    
private:
    /**
     * Record allocation timing for performance monitoring
     */
    void recordAllocationTime();
    
    /**
     * Record deallocation timing for performance monitoring
     */
    void recordDeallocationTime();
    
    std::shared_ptr<Config> config_;
    
    // Neuron pools
    std::unique_ptr<NeuronPool> excitatoryNeuronPool_;
    std::unique_ptr<NeuronPool> inhibitoryNeuronPool_;
    std::unique_ptr<NeuronPool> sensoryNeuronPool_;
    std::unique_ptr<NeuronPool> motorNeuronPool_;
    std::unique_ptr<NeuronPool> internalNeuronPool_;
    
    // Synapse pools
    std::unique_ptr<SynapsePool> excitatorySynapsePool_;
    std::unique_ptr<SynapsePool> inhibitorySynapsePool_;
    std::unique_ptr<SynapsePool> electricalSynapsePool_;
    
    // Type tracking for deallocation
    std::unordered_map<NeuronId, NeuronType> neuronIdToType_;
    std::unordered_map<SynapseId, SynapseType> synapseIdToType_;
    
    // Statistics
    std::atomic<size_t> totalAllocated_;
    std::atomic<size_t> peakUsage_;
    std::atomic<size_t> totalOperations_;
    bool enableMemoryStats_;
    size_t neuronPoolSize_;
    size_t synapsePoolSize_;
    
    // Timing statistics
    std::deque<std::chrono::high_resolution_clock::time_point> allocationTime_;
    std::deque<std::chrono::high_resolution_clock::time_point> deallocationTime_;
};

} // namespace nlm
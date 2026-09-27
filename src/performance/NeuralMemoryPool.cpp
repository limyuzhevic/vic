/**
 * NeuralMemoryPool - Specialized memory pool for neural simulation
 * 
 * Optimized memory allocator for neural data structures including neurons,
 * synapses, and neural state variables. Provides better cache locality
 * and reduced allocation overhead for neural computations.
 * 
 * Features:
 * - Specialized pools for neural types (Neuron, Synapse, etc.)
 * - Cache-aligned allocations
 * - Pre-allocated capacity
 * - Thread-safe allocation/deallocation
 * - Memory reuse and recycling
 */

#include "NeuralMemoryPool.hpp"
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>

namespace nlm {

// Static member definition
NeuralMemoryPool* NeuralMemoryPool::instance = nullptr;
std::mutex NeuralMemoryPool::instanceMutex;

/**
 * Get the singleton NeuralMemoryPool instance
 */

/**
 * Allocate neurons from neural pool
 * @param count Number of neurons to allocate
 * @return Pointer to allocated neuron array
 */

/**
 * Allocate synapses from neural pool
 * @param count Number of synapses to allocate
 * @return Pointer to allocated synapse array
 */

/**
 * Deallocate neurons
 * @param neurons Pointer to neuron array
 * @param count Number of neurons to deallocate
 */

/**
 * Deallocate synapses
 * @param synapses Pointer to synapse array
 * @param count Number of synapses to deallocate
 */

/**
 * Pre-allocate memory pools
 * @param neuronCapacity Number of neurons to pre-allocate
 * @param synapseCapacity Number of synapses to pre-allocate
 */

/**
 * Get memory statistics
 * @param totalMemory Total allocated memory in bytes
 * @param freeMemory Free memory in bytes
 * @param allocatedCount Number of allocated objects
 */

/**
 * Initialize neural memory pools with optimized sizes
 */
void NeuralMemoryPool::initializeNeuralPools() {
    // Calculate optimal pool sizes based on typical neural network sizes
    // Default sizes for medium-scale networks
    
    // Neuron pool: maintain 10% extra capacity for dynamic allocations
    size_t neuronPoolSize = 1000; // Base capacity
    neuronPoolSize += (neuronPoolSize * 0.1); // 10% extra
    neuronPoolSize = ((neuronPoolSize + 1023) / 1024) * 1024; // Round to 1KB
    
    // Synapse pool: sparse connectivity, maintain 5% extra capacity
    size_t synapsePoolSize = 100000; // Base capacity for dense connections
    synapsePoolSize += (synapsePoolSize * 0.05); // 5% extra
    synapsePoolSize = ((synapsePoolSize + 511) / 512) * 512; // Round to 512
    
    // Pre-allocate the pools
    neuronPool.reserve(neuronPoolSize);
    synapsePool.reserve(synapsePoolSize);
    
    // Allocate initial objects to reduce first-time allocation overhead
    for (size_t i = 0; i < 100; ++i) {
        Neuron* neuron = neuronPool.allocate();
        if (neuron) {
            // Initialize neuron with default values
            neuron->id = i;
            neuron->potential = 0.0f;
            neuron->refractoryTime = 0.0f;
            neuron->isSpiking = false;
        }
    }
    
    for (size_t i = 0; i < 1000; ++i) {
        Synapse* synapse = synapsePool.allocate();
        if (synapse) {
            // Initialize synapse with default values
            synapse->source = i % 1000;
            synapse->target = (i + 1) % 1000;
            synapse->weight = 1.0f;
            synapse->delay = 0;
            synapse->isActive = false;
        }
    }
}

/**
 * Cleanup neural memory pools
 */
void NeuralMemoryPool::cleanupNeuralPools() {
    // Invalidate all pointers
    neuronPool = MemoryPool<Neuron>();
    synapsePool = MemoryPool<Synapse>();
}

/**
 * Get neural memory pool instance (thread-safe singleton)
 */

/**
 * Allocate neurons from neural pool (implementation)
 */

/**
 * Allocate synapses from neural pool (implementation)
 */

/**
 * Deallocate neurons (implementation)
 */

/**
 * Deallocate synapses (implementation)
 */

/**
 * Pre-allocate neural pools (implementation)
 */

/**
 * Get neural memory statistics (implementation)
 */

/**
 * Thread-local neuron allocator with cache locality
 */
namespace {
    thread_local std::vector<Neuron*> threadLocalNeurons;
    thread_local std::vector<Synapse*> threadLocalSynapses;
    thread_local size_t threadLocalNeuronIndex = 0;
    thread_local size_t threadLocalSynapseIndex = 0;
    
    const size_t THREAD_LOCAL_POOL_SIZE = 256;
}

/**
 * Get thread-local neuron allocator
 */
Neuron* NeuralMemoryPool::getThreadLocalNeuron() {
    if (threadLocalNeuronIndex >= threadLocalNeurons.size()) {
        // Refill thread-local pool
        threadLocalNeurons.clear();
        threadLocalNeuronIndex = 0;
        
        // Allocate up to THREAD_LOCAL_POOL_SIZE neurons
        for (size_t i = 0; i < THREAD_LOCAL_POOL_SIZE && i + threadLocalNeuronIndex < 1000; ++i) {
            Neuron* neuron = neuronPool.allocate();
            if (neuron) {
                threadLocalNeurons.push_back(neuron);
            }
        }
    }
    
    return threadLocalNeurons[threadLocalNeuronIndex++];
}

/**
 * Get thread-local synapse allocator
 */
Synapse* NeuralMemoryPool::getThreadLocalSynapse() {
    if (threadLocalSynapseIndex >= threadLocalSynapses.size()) {
        // Refill thread-local pool
        threadLocalSynapses.clear();
        threadLocalSynapseIndex = 0;
        
        // Allocate up to THREAD_LOCAL_POOL_SIZE synapses
        for (size_t i = 0; i < THREAD_LOCAL_POOL_SIZE && i + threadLocalSynapseIndex < 1000; ++i) {
            Synapse* synapse = synapsePool.allocate();
            if (synapse) {
                threadLocalSynapses.push_back(synapse);
            }
        }
    }
    
    return threadLocalSynapses[threadLocalSynapseIndex++];
}

/**
 * Release thread-local neuron to pool
 */
void NeuralMemoryPool::releaseThreadLocalNeuron(Neuron* neuron) {
    // Return to global pool
    neuronPool.deallocate(neuron);
    
    // Remove from thread-local pool if present
    auto it = std::find(threadLocalNeurons.begin(), threadLocalNeurons.end(), neuron);
    if (it != threadLocalNeurons.end()) {
        threadLocalNeurons.erase(it);
        if (threadLocalNeuronIndex > threadLocalNeurons.size()) {
            threadLocalNeuronIndex = threadLocalNeurons.size();
        }
    }
}

/**
 * Release thread-local synapse to pool
 */
void NeuralMemoryPool::releaseThreadLocalSynapse(Synapse* synapse) {
    // Return to global pool
    synapsePool.deallocate(synapse);
    
    // Remove from thread-local pool if present
    auto it = std::find(threadLocalSynapses.begin(), threadLocalSynapses.end(), synapse);
    if (it != threadLocalSynapses.end()) {
        threadLocalSynapses.erase(it);
        if (threadLocalSynapseIndex > threadLocalSynapses.size()) {
            threadLocalSynapseIndex = threadLocalSynapses.size();
        }
    }
}

/**
 * Bulk allocation for batch processing
 */
std::vector<Neuron*> NeuralMemoryPool::allocateNeuronBatch(size_t count) {
    std::vector<Neuron*> neurons;
    neurons.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        Neuron* neuron = allocateNeurons(1);
        if (neuron) {
            neurons.push_back(neuron);
        } else {
            break; // Pool exhausted
        }
    }
    
    return neurons;
}

/**
 * Bulk allocation for batch processing
 */
std::vector<Synapse*> NeuralMemoryPool::allocateSynapseBatch(size_t count) {
    std::vector<Synapse*> synapses;
    synapses.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        Synapse* synapse = allocateSynapses(1);
        if (synapse) {
            synapses.push_back(synapse);
        } else {
            break; // Pool exhausted
        }
    }
    
    return synapses;
}

/**
 * Get thread-local neuron allocator (implementation)
 */

/**
 * Get thread-local synapse allocator (implementation)
 */

/**
 * Bulk neuron allocation (implementation)
 */

/**
 * Bulk synapse allocation (implementation)
 */

} // namespace nlm

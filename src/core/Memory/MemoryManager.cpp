#include "MemoryManager.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

NeuronPool NeuronPool::globalNeuronPool;
SynapsePool SynapsePool::globalSynapsePool;
bool NeuronPool::globalInitialized = false;
bool SynapsePool::globalInitialized = false;

NeuronPool::NeuronPool(size_t initialCapacity, size_t maxCapacity)
    : head(nullptr), tail(nullptr), capacity(initialCapacity), size(0), 
      initialized(false) {
    if (initialCapacity > 0) {
        reserve(initialCapacity);
        initialized = true;
    }
}

NeuronPool::~NeuronPool() {
    clear();
}

void* NeuronPool::alignedAllocate(size_t size, size_t alignment) {
    void* ptr = nullptr;
#ifdef _MSC_VER
    ptr = _aligned_malloc(size, alignment);
#else
    if (posix_memalign(&ptr, alignment, size) != 0) {
        ptr = nullptr;
    }
#endif
    return ptr;
}

void NeuronPool::alignedDeallocate(void* ptr, size_t size) {
#ifdef _MSC_VER
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void NeuronPool::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    
    size_t additionalChunks = (count + capacity - 1) / capacity;
    for (size_t i = 0; i < additionalChunks; ++i) {
        Chunk* newChunk = new Chunk(capacity);
        if (!head) {
            head = tail = newChunk;
        } else {
            tail->next = newChunk;
            tail = newChunk;
        }
    }
    capacity = initialCapacity + additionalChunks * capacity;
    initialized = true;
}

Neuron* NeuronPool::allocate(NeuronId id, NeuronType type) {
    std::lock_guard<std::mutex> lock(mutex);
    
    stats.poolMisses++;
    
    if (!head || size >= capacity) {
        return nullptr;
    }
    
    // Find a chunk with free space
    Chunk* current = head;
    while (current) {
        // In a real implementation, we would track free slots
        // For now, just return the first neuron in the first chunk
        if (current->neurons && size < current->capacity) {
            Neuron* neuron = current->neurons + size;
            // Initialize the neuron
            new(neuron) Neuron(id);
            neuron->setType(type);
            size++;
            stats.poolHits++;
            stats.neuronAllocations++;
            stats.currentAllocated += sizeof(Neuron);
            if (stats.currentAllocated > stats.peakAllocated) {
                stats.peakAllocated = stats.currentAllocated;
            }
            stats.totalAllocated++;
            return neuron;
        }
        current = current->next;
    }
    
    return nullptr;
}

void NeuronPool::deallocate(Neuron* neuron) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // In a real implementation, we would track the exact location
    // and update statistics accordingly
    stats.neuronDeallocations++;
    stats.currentAllocated -= sizeof(Neuron);
    stats.totalAllocated++;
    
    // Call destructor (in real implementation)
    neuron->~Neuron();
    size--;
}

void NeuronPool::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    
    Chunk* current = head;
    while (current) {
        Chunk* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    size = 0;
    stats.currentAllocated = 0;
}

SynapsePool::SynapsePool(size_t initialCapacity, size_t maxCapacity)
    : head(nullptr), tail(nullptr), capacity(initialCapacity), size(0),
      initialized(false) {
    if (initialCapacity > 0) {
        reserve(initialCapacity);
        initialized = true;
    }
}

SynapsePool::~SynapsePool() {
    clear();
}

void SynapsePool::reserve(size_t count) {
    std::lock_guard<std::mutex> lock(mutex);
    
    size_t additionalChunks = (count + capacity - 1) / capacity;
    for (size_t i = 0; i < additionalChunks; ++i) {
        Chunk* newChunk = new Chunk(capacity);
        if (!head) {
            head = tail = newChunk;
        } else {
            tail->next = newChunk;
            tail = newChunk;
        }
    }
    capacity = initialCapacity + additionalChunks * capacity;
    initialized = true;
}

Synapse* SynapsePool::allocate(SynapseId id, NeuronId source, NeuronId destination) {
    std::lock_guard<std::mutex> lock(mutex);
    
    stats.poolMisses++;
    
    if (!head || size >= capacity) {
        return nullptr;
    }
    
    // Find a chunk with free space
    Chunk* current = head;
    while (current) {
        // In a real implementation, we would track free slots
        // For now, just return the first synapse in the first chunk
        if (current->synapses && size < current->capacity) {
            Synapse* synapse = current->synapses + size;
            // Initialize the synapse
            new(synapse) Synapse(id, source, destination);
            size++;
            stats.poolHits++;
            stats.synapseAllocations++;
            stats.currentAllocated += sizeof(Synapse);
            if (stats.currentAllocated > stats.peakAllocated) {
                stats.peakAllocated = stats.currentAllocated;
            }
            stats.totalAllocated++;
            return synapse;
        }
        current = current->next;
    }
    
    return nullptr;
}

void SynapsePool::deallocate(Synapse* synapse) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // In a real implementation, we would track the exact location
    // and update statistics accordingly
    stats.synapseDeallocations++;
    stats.currentAllocated -= sizeof(Synapse);
    stats.totalAllocated++;
    
    // Call destructor (in real implementation)
    synapse->~Synapse();
    size--;
}

void SynapsePool::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    
    Chunk* current = head;
    while (current) {
        Chunk* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    size = 0;
    stats.currentAllocated = 0;
}

void SynapsePool::initializeGlobalPools(size_t neuronPoolSize,
                                       size_t synapsePoolSize) {
    globalNeuronPool.reserve(neuronPoolSize);
    globalSynapsePool.reserve(synapsePoolSize);
    globalInitialized = true;
}

void SynapsePool::shutdownGlobalPools() {
    globalNeuronPool.clear();
    globalSynapsePool.clear();
    globalInitialized = false;
}

NeuronPool& SynapsePool::getGlobalNeuronPool() {
    return globalNeuronPool;
}

SynapsePool& SynapsePool::getGlobalSynapsePool() {
    return globalSynapsePool;
}

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

void MemoryManager::initialize(size_t neuronPoolSize,
                               size_t synapsePoolSize) {
    neuronPool.reserve(neuronPoolSize);
    synapsePool.reserve(synapsePoolSize);
    initialized = true;
}

void MemoryManager::shutdown() {
    neuronPool.clear();
    synapsePool.clear();
    initialized = false;
}

const MemoryStats& MemoryManager::getNeuronStats() const {
    return neuronPool.getStats();
}

const MemoryStats& MemoryManager::getSynapseStats() const {
    return synapsePool.getStats();
}

uint64_t MemoryManager::getTotalAllocated() const {
    const auto& neuronStats = neuronPool.getStats();
    const auto& synapseStats = synapsePool.getStats();
    return neuronStats.totalAllocated + synapseStats.totalAllocated;
}

uint64_t MemoryManager::getPeakAllocated() const {
    const auto& neuronStats = neuronPool.getStats();
    const auto& synapseStats = synapsePool.getStats();
    return neuronStats.peakAllocated + synapseStats.peakAllocated;
}

float MemoryManager::getMemoryUtilization() const {
    uint64_t totalCapacity = neuronPool.getCapacity() + synapsePool.getCapacity();
    uint64_t currentUsed = getTotalAllocated();
    return static_cast<float>(currentUsed) / static_cast<float>(totalCapacity);
}

NeuronPool& MemoryManager::getNeuronPool() {
    return neuronPool;
}

SynapsePool& MemoryManager::getSynapsePool() {
    return synapsePool;
}

void MemoryManager::printMemoryStats() const {
    const auto& neuronStats = neuronPool.getStats();
    const auto& synapseStats = synapsePool.getStats();
    
    std::cout << "=== Memory Statistics ===" << std::endl;
    std::cout << "Neuron Pool:" << std::endl;
    std::cout << "  Allocations: " << neuronStats.neuronAllocations << std::endl;
    std::cout << "  Deallocations: " << neuronStats.neuronDeallocations << std::endl;
    std::cout << "  Pool Hits: " << neuronStats.poolHits << std::endl;
    std::cout << "  Pool Misses: " << neuronStats.poolMisses << std::endl;
    std::cout << "  Current Allocated: " << neuronStats.currentAllocated << " bytes" << std::endl;
    std::cout << "  Peak Allocated: " << neuronStats.peakAllocated << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Synapse Pool:" << std::endl;
    std::cout << "  Allocations: " << synapseStats.synapseAllocations << std::endl;
    std::cout << "  Deallocations: " << synapseStats.synapseDeallocations << std::endl;
    std::cout << "  Pool Hits: " << synapseStats.poolHits << std::endl;
    std::cout << "  Pool Misses: " << synapseStats.poolMisses << std::endl;
    std::cout << "  Current Allocated: " << synapseStats.currentAllocated << " bytes" << std::endl;
    std::cout << "  Peak Allocated: " << synapseStats.peakAllocated << " bytes" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Total Allocated: " << (neuronStats.totalAllocated + synapseStats.totalAllocated) << std::endl;
    std::cout << "Memory Utilization: " << getMemoryUtilization() * 100 << "%" << std::endl;
}

std::string MemoryManager::getMemoryReport() const {
    std::stringstream ss;
    ss << "=== Memory Report ===" << std::endl;
    ss << "Neuron Pool Utilization: " << neuronPool.getUtilization() * 100 << "%" << std::endl;
    ss << "Synapse Pool Utilization: " << synapsePool.getUtilization() * 100 << "%" << std::endl;
    ss << "Total Memory Allocated: " << getTotalAllocated() << " bytes" << std::endl;
    ss << "Peak Memory Allocated: " << getPeakAllocated() << " bytes" << std::endl;
    ss << "Overall Memory Efficiency: " << (1.0f - getMemoryUtilization()) * 100 << "%" << std::endl;
    return ss.str();
}

} // namespace nlm
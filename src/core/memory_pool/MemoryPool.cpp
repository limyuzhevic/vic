// MemoryPool.cpp
// Memory pool implementations

#include "MemoryPool.hpp"
#include <cstring>
#include <iostream>
#include <algorithm>

namespace nlm {

// MemoryPool implementation

template<typename T, size_t BLOCK_SIZE>
MemoryPool<T, BLOCK_SIZE>::MemoryPool(size_t initialBlocks) {
    reserve(initialBlocks);
}

template<typename T, size_t BLOCK_SIZE>
MemoryPool<T, BLOCK_SIZE>::~MemoryPool() {
    clear();
}

template<typename T, size_t BLOCK_SIZE>
T* MemoryPool<T, BLOCK_SIZE>::allocate() {
    if (m_usedCount >= m_totalAllocated) {
        // Need to allocate new block
        Block* newBlock = new Block();
        if (!m_firstBlock) {
            m_firstBlock = newBlock;
        } else {
            m_lastBlock->next = newBlock;
        }
        m_lastBlock = newBlock;
        m_totalAllocated += m_blockSize;
        m_stats.allocationCount++;
        m_stats.totalAllocated += m_blockSize * sizeof(T);
    }
    
    // Get pointer to next available slot
    T* ptr = reinterpret_cast<T*>(m_lastBlock->data) + m_lastBlock->used;
    m_lastBlock->used++;
    m_usedCount++;
    m_stats.totalUsed++;
    
    if (m_stats.totalUsed > m_stats.peakUsage) {
        m_stats.peakUsage = m_stats.totalUsed;
    }
    
    return ptr;
}

template<typename T, size_t BLOCK_SIZE>
void MemoryPool<T, BLOCK_SIZE>::deallocate(T* ptr) {
    if (!ptr) return;
    
    // Find block containing this pointer and position
    Block* current = m_firstBlock;
    size_t slotIndex = 0;
    
    while (current) {
        T* blockStart = reinterpret_cast<T*>(current->data);
        if (ptr >= blockStart && ptr < blockStart + m_blockSize) {
            // Found the block
            size_t indexInBlock = ptr - blockStart;
            
            // Mark slot as free (we could use a bitmap for better efficiency)
            // For now, we'll just track used count
            if (current->used > indexInBlock) {
                current->used = indexInBlock;
            }
            
            m_usedCount--;
            m_stats.deallocationCount++;
            m_stats.totalUsed--;
            break;
        }
        current = current->next;
    }
}

template<typename T, size_t BLOCK_SIZE>
void MemoryPool<T, BLOCK_SIZE>::clear() {
    Block* current = m_firstBlock;
    while (current) {
        Block* next = current->next;
        delete current;
        current = next;
    }
    
    m_firstBlock = nullptr;
    m_lastBlock = nullptr;
    m_usedCount = 0;
    m_totalAllocated = 0;
}

template<typename T, size_t BLOCK_SIZE>
void MemoryPool<T, BLOCK_SIZE>::reserve(size_t additionalBlocks) {
    size_t additionalBytes = additionalBlocks * sizeof(T);
    // Simple implementation - in practice, you'd pre-allocate more efficiently
    for (size_t i = 0; i < additionalBlocks; ++i) {
        allocate();
    }
}

template<typename T, size_t BLOCK_SIZE>
bool MemoryPool<T, BLOCK_SIZE>::owns(T* ptr) const {
    Block* current = m_firstBlock;
    while (current) {
        T* blockStart = reinterpret_cast<T*>(current->data);
        if (ptr >= blockStart && ptr < blockStart + m_blockSize) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// NeuronMemoryPool implementation

NeuronMemoryPool::NeuronMemoryPool(size_t initialCapacity) {
    reserve(initialCapacity);
}

NeuronMemoryPool::~NeuronMemoryPool() {
    clear();
}

NeuronMemoryPool::NeuronData& NeuronMemoryPool::getNeuronData(size_t index) {
    return m_neurons[index];
}

const NeuronMemoryPool::NeuronData& NeuronMemoryPool::getNeuronData(size_t index) const {
    return m_neurons[index];
}

size_t NeuronMemoryPool::allocateNeuron() {
    size_t index;
    if (!m_freeList.empty()) {
        index = m_freeList.back();
        m_freeList.pop_back();
    } else {
        if (m_size >= m_capacity) {
            reserve(m_capacity * 2);
        }
        index = m_size++;
    }
    
    m_used[index] = true;
    m_stats.allocationCount++;
    m_stats.totalUsed++;
    
    if (m_stats.totalUsed > m_stats.peakUsage) {
        m_stats.peakUsage = m_stats.totalUsed;
    }
    
    return index;
}

void NeuronMemoryPool::deallocateNeuron(size_t index) {
    if (index < m_used.size() && m_used[index]) {
        m_used[index] = false;
        m_freeList.push_back(index);
        m_stats.deallocationCount++;
        m_stats.totalUsed--;
    }
}

void NeuronMemoryPool::reserve(size_t additionalCapacity) {
    size_t newCapacity = m_capacity + additionalCapacity;
    
    // Reserve space for neuron data
    m_neurons.reserve(newCapacity);
    
    // Reserve space for used flags
    m_used.reserve(newCapacity);
    
    // Extend with default values
    m_neurons.resize(newCapacity);
    m_used.resize(newCapacity, false);
    
    m_capacity = newCapacity;
}

void NeuronMemoryPool::clear() {
    m_freeList.clear();
    std::fill(m_used.begin(), m_used.end(), false);
    m_size = 0;
    m_stats.allocationCount = 0;
    m_stats.deallocationCount = 0;
    m_stats.totalUsed = 0;
}

// SynapseMemoryPool implementation

SynapseMemoryPool::SynapseMemoryPool(size_t initialCapacity) {
    reserve(initialCapacity);
}

SynapseMemoryPool::~SynapseMemoryPool() {
    clear();
}

SynapseMemoryPool::SynapseData& SynapseMemoryPool::getSynapseData(size_t index) {
    return m_synapses[index];
}

const SynapseMemoryPool::SynapseData& SynapseMemoryPool::getSynapseData(size_t index) const {
    return m_synapses[index];
}

size_t SynapseMemoryPool::allocateSynapse() {
    size_t index;
    if (!m_freeList.empty()) {
        index = m_freeList.back();
        m_freeList.pop_back();
    } else {
        if (m_size >= m_capacity) {
            reserve(m_capacity * 2);
        }
        index = m_size++;
    }
    
    m_used[index] = true;
    m_stats.allocationCount++;
    m_stats.totalUsed++;
    
    if (m_stats.totalUsed > m_stats.peakUsage) {
        m_stats.peakUsage = m_stats.totalUsed;
    }
    
    return index;
}

void SynapseMemoryPool::deallocateSynapse(size_t index) {
    if (index < m_used.size() && m_used[index]) {
        m_used[index] = false;
        m_freeList.push_back(index);
        m_stats.deallocationCount++;
        m_stats.totalUsed--;
    }
}

void SynapseMemoryPool::reserve(size_t additionalCapacity) {
    size_t newCapacity = m_capacity + additionalCapacity;
    
    // Reserve space for synapse data
    m_synapses.reserve(newCapacity);
    
    // Reserve space for used flags
    m_used.reserve(newCapacity);
    
    // Extend with default values
    m_synapses.resize(newCapacity);
    m_used.resize(newCapacity, false);
    
    m_capacity = newCapacity;
}

void SynapseMemoryPool::clear() {
    m_freeList.clear();
    std::fill(m_used.begin(), m_used.end(), false);
    m_size = 0;
    m_stats.allocationCount = 0;
    m_stats.deallocationCount = 0;
    m_stats.totalUsed = 0;
}

// MemoryManager implementation

MemoryManager::MemoryManager() : m_initialized(false), m_trackMemory(true) {}

MemoryManager::~MemoryManager() {
    reset();
}

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

void MemoryManager::initialize(size_t neuronCapacity, 
                              size_t synapseCapacity,
                              size_t memoryPoolBlockSize) {
    if (m_initialized) return;
    
    m_neuronPool.reserve(neuronCapacity);
    m_synapsePool.reserve(synapseCapacity);
    
    m_initialized = true;
}

MemoryStats MemoryManager::getTotalStats() const {
    MemoryStats totalStats;
    totalStats.totalAllocated = 
        m_neuronPool.getStats().totalAllocated +
        m_synapsePool.getStats().totalAllocated;
    totalStats.totalUsed = 
        m_neuronPool.getStats().totalUsed +
        m_synapsePool.getStats().totalUsed;
    totalStats.peakUsage = std::max(
        m_neuronPool.getStats().peakUsage,
        m_synapsePool.getStats().peakUsage);
    totalStats.allocationCount = 
        m_neuronPool.getStats().allocationCount +
        m_synapsePool.getStats().allocationCount;
    totalStats.deallocationCount = 
        m_neuronPool.getStats().deallocationCount +
        m_synapsePool.getStats().deallocationCount;
    
    return totalStats;
}

void MemoryManager::reset() {
    m_neuronPool.clear();
    m_synapsePool.clear();
    m_initialized = false;
}

void MemoryManager::setBlockSize(size_t blockSize) {
    // In a full implementation, this would reallocate pools
    // For now, it's a placeholder
}

// Helper functions

template<typename T, size_t BLOCK_SIZE>
T* allocateFromPool(MemoryPool<T, BLOCK_SIZE>& pool) {
    return pool.allocate();
}

template<typename T, size_t BLOCK_SIZE>
void deallocateToPool(MemoryPool<T, BLOCK_SIZE>& pool, T* ptr) {
    pool.deallocate(ptr);
}

} // namespace nlm

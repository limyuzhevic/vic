#pragma once

// Memory pool implementation for NLM brain simulation
// Provides actual implementation of memory pool functionality

#include "core/MemoryPool.hpp"
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

namespace nlm {

// ==================== MEMORY POOL IMPLEMENTATION ====================

// Neuron memory pool implementation
NeuronMemoryPool::NeuronMemoryPool(size_t capacity)
    : pool_(capacity), allocationCount_(0) {}

// Synapse memory pool implementation
SynapseMemoryPool::SynapseMemoryPool(size_t capacity)
    : pool_(capacity), allocationCount_(0) {}

// Region memory pool implementation
RegionMemoryPool::RegionMemoryPool(size_t capacity)
    : pool_(capacity) {}

// Population memory pool implementation
PopulationMemoryPool::PopulationMemoryPool(size_t capacity)
    : pool_(capacity) {}

// Connection memory pool implementation
ConnectionMemoryPool::ConnectionMemoryPool(size_t capacity)
    : pool_(capacity) {}

// ==================== MEMORY MANAGER IMPLEMENTATION ====================

MemoryManager::MemoryManager() {
    initializePools();
}

MemoryManager::~MemoryManager() {
    for (auto& pair : pools_) {
        delete pair.second;
    }
}

} // namespace nlm
#pragma once

// Vector pool implementation for NLM brain simulation
// Provides actual implementation of vector memory pool functionality

#include "core/VectorPool.hpp"
#include <vector>
#include <mutex>
#include <algorithm>
#include <iostream>

namespace nlm {

// ==================== VECTOR MEMORY POOL IMPLEMENTATION ====================

// Neuron vector memory pool implementation
NeuronVectorMemoryPool::NeuronVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<Neuron*>(capacity) {}

// Synapse vector memory pool implementation  
SynapseVectorMemoryPool::SynapseVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<Synapse*>(capacity) {}

// Float vector memory pool implementation
FloatVectorMemoryPool::FloatVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<float>(capacity) {}

// Double vector memory pool implementation
DoubleVectorMemoryPool::DoubleVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<double>(capacity) {}

// String vector memory pool implementation
StringVectorMemoryPool::StringVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<std::string>(capacity) {}

// Timestamp vector memory pool implementation
TimestampVectorMemoryPool::TimestampVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<Timestamp>(capacity) {}

// Size_t vector memory pool implementation
SizeTVectorMemoryPool::SizeTVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<size_t>(capacity) {}

// Int vector memory pool implementation
IntVectorMemoryPool::IntVectorMemoryPool(size_t capacity)
    : VectorMemoryPoolBase<int>(capacity) {}

// ==================== VECTOR MEMORY MANAGER IMPLEMENTATION ====================

VectorMemoryManager::VectorMemoryManager() {
    initializePools();
}

VectorMemoryManager::~VectorMemoryManager() {
    for (auto& pair : pools_) {
        delete pair.second;
    }
}

} // namespace nlm
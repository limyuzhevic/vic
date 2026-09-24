#include "MemoryPool.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

template<typename T>
MemoryPool<T>::MemoryPool(size_t initialSize, size_t maxSize)
    : maxSize_(maxSize)
    , currentSize_(0)
    , allocationCount_(0)
    , deallocationCount_(0) {
    if (initialSize > maxSize) {
        throw std::runtime_error("Initial size cannot be larger than max size");
    }
    
    // Allocate initial blocks
    for (size_t i = 0; i < initialSize; ++i) {
        freeBlocks_.push_back(std::make_unique<Block<T>>());
    }
    currentSize_ = initialSize;
}

template<typename T>
MemoryPool<T>::~MemoryPool() = default;

template<typename T>
T* MemoryPool<T>::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (freeBlocks_.empty()) {
        if (currentSize_ < maxSize_) {
            // Expand pool
            freeBlocks_.push_back(std::make_unique<Block<T>>());
            currentSize_++;
        } else {
            throw std::runtime_error("MemoryPool is full");
        }
    }
    
    auto& block = freeBlocks_.back();
    freeBlocks_.pop_back();
    
    // Find first free slot in the block
    for (size_t i = 0; i < BlockSize; ++i) {
        if (!block->used[i]) {
            block->used[i] = true;
            allocationCount_++;
            return &block->data[i];
        }
    }
    
    // Should not reach here
    throw std::runtime_error("MemoryPool block is full");
}

template<typename T>
void MemoryPool<T>::deallocate(T* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& block : freeBlocks_) {
        for (size_t i = 0; i < BlockSize; ++i) {
            if (&block->data[i] == ptr) {
                block->used[i] = false;
                deallocationCount_++;
                return;
            }
        }
    }
    
    // Should not reach here - pointer not found in any block
    throw std::runtime_error("Pointer not found in MemoryPool");
}

template<typename T>
size_t MemoryPool<T>::getFreeBlocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return freeBlocks_.size();
}

template<typename T>
size_t MemoryPool<T>::getUsedSlots() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t used = 0;
    for (const auto& block : freeBlocks_) {
        for (size_t i = 0; i < BlockSize; ++i) {
            if (block->used[i]) {
                used++;
            }
        }
    }
    return used;
}

template<typename T>
size_t MemoryPool<T>::getMaxCapacity() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return maxSize_ * BlockSize;
}

template<typename T>
float MemoryPool<T>::getUtilization() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t totalCapacity = getMaxCapacity();
    if (totalCapacity == 0) return 0.0f;
    
    return static_cast<float>(getUsedSlots()) / static_cast<float>(totalCapacity);
}

// Explicit instantiations for common types
template class MemoryPool<Neuron>;
template class MemoryPool<Synapse>;
template class MemoryPool<NeuronalSignal>;
template class MemoryPool<PlasticityRule>;

} // namespace nlm

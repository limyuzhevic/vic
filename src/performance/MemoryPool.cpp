// Stub implementation for MemoryPool.cpp
// This is a placeholder to satisfy CMake build requirements
// Phase 2 implementations will replace this stub

#include "MemoryPool.hpp"
#include <iostream>

namespace nlm {

template<typename T>
MemoryPool<T>::MemoryPool(size_t initialCapacity, size_t maxCapacity) {
    std::cout << "MemoryPool initialized with " << initialCapacity << " items (stub)" << std::endl;
}

template<typename T>
MemoryPool<T>::~MemoryPool() = default;

template<typename T>
T* MemoryPool<T>::allocate() {
    // Stub implementation - just create a static instance for now
    static T instance;
    return &instance;
}

template<typename T>
void MemoryPool<T>::deallocate(T* ptr) {
    // Stub implementation
    std::cout << "MemoryPool::deallocate called (stub)" << std::endl;
}

template<typename T>
size_t MemoryPool<T>::getAllocatedCount() const {
    return 1;  // Stub
}

template<typename T>
void MemoryPool<T>::clear() {
    // Stub implementation
    std::cout << "MemoryPool::clear called (stub)" << std::endl;
}

} // namespace nlm
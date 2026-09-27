#include "MemoryPool.hpp"

namespace nlm {

// MemoryPool implementations

template<typename T>
MemoryPool<T>::Block::Block(size_t objectSize, size_t capacity)
    : data(objectSize * capacity)
    , allocated(0)
    , capacity(capacity)
{
    // Data is left uninitialized for performance
    // Objects will be initialized by constructors
}

// Template instantiations for common types
// Neuron and Synapse are forward-declared, so we need to provide implementations

// Helper functions for statistics

template<typename T>
size_t MemoryPool<T>::Block::memoryUsage() const {
    return allocated * sizeof(T);
}

} // namespace nlm
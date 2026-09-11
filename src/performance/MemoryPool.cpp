#include "MemoryPool.hpp"
#include <cstring>

namespace nlm {

struct MemoryPool::Impl {
    size_t poolSize;
    size_t alignment;
    size_t allocatedMemory;
    size_t peakMemory;
    size_t allocationCount;
    size_t deallocationCount;
    bool debugMode;
    std::vector<std::byte> memoryBlock;
};

MemoryPool::MemoryPool() : pImpl(std::make_unique<Impl>()) {
    pImpl->poolSize = 1024 * 1024; // 1MB default
    pImpl->alignment = alignof(std::max_align_t);
    pImpl->allocatedMemory = 0;
    pImpl->peakMemory = 0;
    pImpl->allocationCount = 0;
    pImpl->deallocationCount = 0;
    pImpl->debugMode = false;
}

MemoryPool::~MemoryPool() = default;

void MemoryPool::setPoolSize(size_t size) {
    pImpl->poolSize = size;
    pImpl->memoryBlock.resize(size);
}

void MemoryPool::setAlignment(size_t alignment) {
    pImpl->alignment = alignment;
}

size_t MemoryPool::getPoolSize() const {
    return pImpl->poolSize;
}

size_t MemoryPool::getAlignment() const {
    return pImpl->alignment;
}

void* MemoryPool::allocate(size_t size) {
    if (pImpl->allocationCount >= pImpl->poolSize) {
        return nullptr; // Pool exhausted
    }
    
    if (pImpl->debugMode) {
        std::cout << "Allocating " << size << " bytes" << std::endl;
    }
    
    // Simple bump pointer allocation
    size_t offset = (pImpl->allocatedMemory + pImpl->alignment - 1) & ~(pImpl->alignment - 1);
    if (offset + size > pImpl->poolSize) {
        return nullptr; // Not enough space
    }
    
    void* ptr = pImpl->memoryBlock.data() + offset;
    pImpl->allocatedMemory = offset + size;
    pImpl->allocationCount++;
    
    if (pImpl->allocatedMemory > pImpl->peakMemory) {
        pImpl->peakMemory = pImpl->allocatedMemory;
    }
    
    return ptr;
}

template<typename T, typename... Args>
T* MemoryPool::allocateObject(Args&&... args) {
    void* ptr = allocate(sizeof(T));
    if (!ptr) return nullptr;
    
    try {
        return new (ptr) T(std::forward<Args>(args)...);
    } catch (...) {
        deallocate(ptr, sizeof(T));
        return nullptr;
    }
}

void MemoryPool::deallocate(void* ptr, size_t size) {
    if (!ptr) return;
    
    pImpl->deallocationCount++;
    
    if (pImpl->debugMode) {
        std::cout << "Deallocating " << size << " bytes" << std::endl;
    }
}

// PoolAllocator implementations
template<typename T>
T* MemoryPool::PoolAllocator<T>::allocate(std::size_t n) {
    void* ptr = pool_.allocate(n * sizeof(T));
    return static_cast<T*>(ptr);
}

template<typename T>
void MemoryPool::PoolAllocator<T>::deallocate(T* p, std::size_t n) {
    pool_.deallocate(p, n * sizeof(T));
}

template<typename T>
template<typename U, typename... Args>
void MemoryPool::PoolAllocator<T>::construct(U* p, Args&&... args) {
    ::new ((void*)p) U(std::forward<Args>(args)...);
}

template<typename T>
template<typename U>
void MemoryPool::PoolAllocator<T>::destroy(U* p) {
    p->~U();
}

template<typename T>
bool MemoryPool::PoolAllocator<T>::operator==(const PoolAllocator& other) const {
    return &pool_ == &other.pool_;
}

template<typename T>
bool MemoryPool::PoolAllocator<T>::operator!=(const PoolAllocator& other) const {
    return !(*this == other);
}

size_t MemoryPool::getAllocatedMemory() const {
    return pImpl->allocatedMemory;
}

size_t MemoryPool::getPeakMemory() const {
    return pImpl->peakMemory;
}

size_t MemoryPool::getAllocationCount() const {
    return pImpl->allocationCount;
}

size_t MemoryPool::getDeallocationCount() const {
    return pImpl->deallocationCount;
}

void MemoryPool::clear() {
    pImpl->allocatedMemory = 0;
    pImpl->allocationCount = 0;
    pImpl->deallocationCount = 0;
}

void MemoryPool::shrinkToFit() {
    pImpl->memoryBlock.resize(pImpl->allocatedMemory);
}

void MemoryPool::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
}

bool MemoryPool::isDebugMode() const {
    return pImpl->debugMode;
}

std::string MemoryPool::getMemoryReport() const {
    std::ostringstream oss;
    oss << "Memory Pool Report:\n";
    oss << "  Pool Size: " << pImpl->poolSize << " bytes\n";
    oss << "  Alignment: " << pImpl->alignment << " bytes\n";
    oss << "  Allocated: " << pImpl->allocatedMemory << " bytes\n";
    oss << "  Peak: " << pImpl->peakMemory << " bytes\n";
    oss << "  Allocations: " << pImpl->allocationCount << "\n";
    oss << "  Deallocations: " << pImpl->deallocationCount << "\n";
    oss << "  Utilization: " << (static_cast<float>(pImpl->allocatedMemory) / pImpl->poolSize * 100) << "%\n";
    return oss.str();
}

// Explicit instantiations
namespace {
    template class MemoryPool::PoolAllocator<int>;
    template class MemoryPool::PoolAllocator<float>;
    template class MemoryPool::PoolAllocator<double>;
    template class MemoryPool::PoolAllocator<void>;
}

} // namespace nlm

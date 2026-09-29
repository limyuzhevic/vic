#include "MemoryManagement.hpp"
#include <iostream>

namespace nlm {

namespace memory {

// MemoryMonitor implementation
std::map<PoolType, MemoryPoolImpl*> MemoryMonitor::globalPools_;
std::thread MemoryMonitor::monitoringThread_;
MemoryMonitor::StatsCallback MemoryMonitor::statsCallback_ = nullptr;

// Memory allocation functions with tracking
void* operator new(size_t size) {
    void* ptr = ::operator new(size);
    trackAllocation(ptr, size, __FILE__, __LINE__, __func__);
    incrementAllocationCount();
    return ptr;
}

void* operator new[](size_t size) {
    void* ptr = ::operator new[](size);
    trackAllocation(ptr, size, __FILE__, __LINE__, __func__);
    incrementAllocationCount();
    return ptr;
}

void operator delete(void* ptr) noexcept {
    trackDeallocation(ptr, 0); // Size unknown for single object
    ::operator delete(ptr);
    incrementDeallocationCount();
}

void operator delete[](void* ptr) noexcept {
    trackDeallocation(ptr, 0); // Size unknown for array
    ::operator delete[](ptr);
    incrementDeallocationCount();
}

void operator delete(void* ptr, size_t size) noexcept {
    trackDeallocation(ptr, size);
    ::operator delete(ptr);
    incrementDeallocationCount();
}

void operator delete[](void* ptr, size_t size) noexcept {
    trackDeallocation(ptr, size);
    ::operator delete[](ptr);
    incrementDeallocationCount();
}

// Memory pool allocator specific new/delete
void* operator new(size_t size, PoolType poolType) {
    if (MemoryPoolAllocator<void, poolType>::getGlobalPool()) {
        void* ptr = MemoryPoolAllocator<void, poolType>::getGlobalPool()->allocate(size);
        if (ptr) {
            trackAllocation(ptr, size, __FILE__, __LINE__, __func__);
            incrementAllocationCount();
            return ptr;
        }
    }
    return ::operator new(size);
}

void operator delete(void* ptr, size_t size, PoolType poolType) noexcept {
    if (MemoryPoolAllocator<void, poolType>::getGlobalPool()) {
        trackDeallocation(ptr, size);
        MemoryPoolAllocator<void, poolType>::getGlobalPool()->deallocate(ptr, size);
        incrementDeallocationCount();
        return;
    }
    ::operator delete(ptr);
}

// Performance monitor implementation
PerformanceMonitor::PerformanceMetrics PerformanceMonitor::metrics_;
std::mutex PerformanceMonitor::metricsMutex_;
std::function<void(const PerformanceMonitor::PerformanceMetrics&)> PerformanceMonitor::metricsCallback_ = nullptr;

void incrementAllocationCount() {
    PerformanceMonitor::incrementAllocationCount();
    MemoryMonitor::sampleMetrics();
}

void incrementDeallocationCount() {
    PerformanceMonitor::incrementDeallocationCount();
    MemoryMonitor::sampleMetrics();
}

// Memory debug implementation
MemoryDebug::AllocationRecord MemoryDebug::allocationRecords_[0]; // Dummy initialization
std::mutex MemoryDebug::allocationMutex_;

// Global memory management function implementations
void initializeMemoryManagement() {
    MemoryMonitor::initialize();
    PerformanceMonitor::initialize();
}

void shutdownMemoryManagement() {
    PerformanceMonitor::shutdown();
    MemoryMonitor::shutdown();
}

MemoryStats getMemoryStats() {
    return MemoryMonitor::getStats();
}

void logMemoryStats() {
    MemoryMonitor::logStats();
}

void logPerformanceStats() {
    PerformanceMonitor::logMetrics();
}

void garbageCollect() {
    MemoryMonitor::garbageCollect();
}

void trackAllocation(void* ptr, size_t size, const char* file, int line, const char* function) {
    MemoryDebug::trackAllocation(ptr, size, file, line, function);
}

void trackDeallocation(void* ptr, size_t size) {
    MemoryDebug::trackDeallocation(ptr, size);
}

void logMemoryLeaks() {
    MemoryDebug::logLeaks();
}

void clearLeakRecords() {
    MemoryDebug::clearLeakRecords();
}

} // namespace memory

} // namespace nlm

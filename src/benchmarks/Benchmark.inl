// Benchmark implementation details

namespace nlm {
namespace benchmark {

// MemoryProfiler implementation details

MemoryProfiler& MemoryProfiler::instance() {
    static MemoryProfiler profiler;
    return profiler;
}

deallocate(void* address, size_t size) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = std::find_if(allocations.begin(), allocations.end(),
        [address](const AllocationInfo& info) { return info.address == address; });
    
    if (it != allocations.end()) {
        allocations.erase(it);
    }
}

deallocateMemoryUsage(size_t size) {
    peakMemoryUsage = std::max(peakMemoryUsage, totalMemoryUsage + static_cast<double>(size));
}

deallocatePeakMemoryUsage() const {
    return peakMemoryUsage;
}

// Global memory allocation tracking
void* globalAllocate(size_t size, const std::string& site) {
    void* ptr = std::malloc(size);
    if (ptr) {
        MemoryProfiler::instance().recordAllocation(size, ptr, site);
    }
    return ptr;
}

void globalDeallocate(void* ptr, size_t size) {
    if (ptr) {
        MemoryProfiler::instance().recordDeallocation(ptr, size);
        std::free(ptr);
    }
}

// Performance allocation overrides
void* operator new(size_t size) {
    return globalAllocate(size, "operator new");
}

void* operator new[](size_t size) {
    return globalAllocate(size, "operator new[]");
}

void operator delete(void* ptr) noexcept {
    globalDeallocate(ptr, 0);
}

void operator delete[](void* ptr) noexcept {
    globalDeallocate(ptr, 0);
}

void operator delete(void* ptr, size_t size) noexcept {
    globalDeallocate(ptr, size);
}

void operator delete[](void* ptr, size_t size) noexcept {
    globalDeallocate(ptr, size);
}

// Global performance monitor instance
PerformanceMonitor& globalPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

void performanceIncrementOperations() {
    globalPerformanceMonitor().incrementOperations();
}

void performanceIncrementMemoryUsage(size_t bytes) {
    globalPerformanceMonitor().incrementMemoryUsage(bytes);
}

void performanceRecordCacheHit() {
    globalPerformanceMonitor().recordCacheHit();
}

void performanceRecordCacheMiss() {
    globalPerformanceMonitor().recordCacheMiss();
}

} // namespace benchmark
} // namespace nlm

// Global benchmark macros
#define BENCHMARK_START PerformanceMonitor::getInstance().start()
#define BENCHMARK_STOP metrics PerformanceMonitor::getInstance().stop(metrics)

#define BENCHMARK_RECORD_OPERATION PerformanceMonitor::incrementOperations()
#define BENCHMARK_RECORD_MEMORY_USAGE(bytes) PerformanceMonitor::incrementMemoryUsage(bytes)
#define BENCHMARK_RECORD_CACHE_HIT PerformanceMonitor::recordCacheHit()
#define BENCHMARK_RECORD_CACHE_MISS PerformanceMonitor::recordCacheMiss()
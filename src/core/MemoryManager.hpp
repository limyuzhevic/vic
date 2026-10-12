// NLM (熙然) - Neural Learning Machine
// Phase 8: Memory Management Tools
//
// This phase enhances the memory management system with:
// - Memory leak detection and tracking
// - Real-time memory profiling
// - Allocation tracking and monitoring
// - Memory usage analysis
// - Performance optimization suggestions

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <sstream>
#include <iostream>
#include <fstream>
#include <mutex>
#include <algorithm>

namespace nlm {

// Memory allocation tracking
struct AllocationInfo {
    void* pointer;
    size_t size;
    std::string allocationSite;
    int lineNumber;
    std::string functionName;
    std::string fileName;
    std::chrono::system_clock::time_point timestamp;
    std::string callStack;
    
    AllocationInfo() : pointer(nullptr), size(0), allocationSite(""), lineNumber(0),
                     functionName(""), fileName(""), timestamp() {}
    
    AllocationInfo(void* ptr, size_t sz, const std::string& site, int line,
                   const std::string& func, const std::string& file)
        : pointer(ptr), size(sz), allocationSite(site), lineNumber(line),
          functionName(func), fileName(file), timestamp(std::chrono::system_clock::now()) {}
};

// Memory leak detection and tracking
class MemoryManager {
public:
    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }
    
    // Allocation tracking
    void trackAllocation(void* ptr, size_t size, const std::string& site,
                        int line, const std::string& func, const std::string& file);
    
    void trackDeallocation(void* ptr);
    
    // Memory statistics
    size_t getTotalAllocated() const { return totalAllocated; }
    size_t getTotalFreed() const { return totalFreed; }
    size_t getCurrentUsage() const { return totalAllocated - totalFreed; }
    size_t getAllocationCount() const { return allocationCount; }
    size_t getDeallocationCount() const { return deallocationCount; }
    
    // Leak detection
    std::vector<AllocationInfo> findLeaks() const;
    bool hasLeaks() const { return !findLeaks().empty(); }
    size_t getLeakCount() const { return getLeaks().size(); }
    
    // Memory profiling
    std::string generateMemoryReport() const;
    void exportMemoryReport(const std::string& filename) const;
    
    // Performance analysis
    void analyzeMemoryPatterns();
    std::vector<std::string> getPerformanceSuggestions() const;
    
    // Integration with NLM objects
    void trackObjectAllocation(const std::string& typeName, void* obj, size_t size);
    void trackObjectDeallocation(const std::string& typeName, void* obj);
    
    // Simulation integration
    void startMemoryProfiling();
    void stopMemoryProfiling();
    bool isMemoryProfiling() const { return memoryProfilingEnabled; }
    
    // Memory thresholds and limits
    void setMemoryLimit(size_t limitBytes) { memoryLimit = limitBytes; }
    size_t getMemoryLimit() const { return memoryLimit; }
    bool isMemoryLimitExceeded() const { return getCurrentUsage() > memoryLimit; }
    
    // Configuration
    void enableLeakDetection(bool enable) { leakDetectionEnabled = enable; }
    bool isLeakDetectionEnabled() const { return leakDetectionEnabled; }
    
    void enablePerformanceAnalysis(bool enable) { performanceAnalysisEnabled = enable; }
    bool isPerformanceAnalysisEnabled() const { return performanceAnalysisEnabled; }
    
private:
    MemoryManager() : totalAllocated(0), totalFreed(0), allocationCount(0),
                     deallocationCount(0), memoryLimit(0), memoryProfilingEnabled(false),
                     leakDetectionEnabled(true), performanceAnalysisEnabled(true) {}
    
    // Prevent copying
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    // Core tracking structures
    std::vector<AllocationInfo> allocations;
    std::unordered_map<void*, size_t> allocationMap;
    std::mutex allocationMutex;
    
    // Statistics
    size_t totalAllocated;
    size_t totalFreed;
    size_t allocationCount;
    size_t deallocationCount;
    
    // Configuration
    size_t memoryLimit;
    bool memoryProfilingEnabled;
    bool leakDetectionEnabled;
    bool performanceAnalysisEnabled;
    
    // Performance analysis data
    std::vector<std::string> performanceSuggestions;
    std::unordered_map<std::string, size_t> allocationByType;
    std::unordered_map<std::string, size_t> deallocationByType;
    
    // Helper functions
    std::string getCallStack(int maxDepth = 10) const;
    std::string formatSize(size_t size) const;
    std::string getTimestamp(const std::chrono::system_clock::time_point& tp) const;
    
    // Cleanup
    void cleanup();
    void generatePerformanceSuggestions();
};

// Smart pointer with memory tracking
template<typename T>
class TrackedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    TrackedAllocator() = default;
    
    template<typename U>
    TrackedAllocator(const TrackedAllocator<U>&) {}
    
    T* allocate(std::size_t n) {
        void* ptr = ::operator new(n * sizeof(T));
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        MemoryManager::getInstance().trackAllocation(ptr, n * sizeof(T),
                                                     "Tracked allocation", 0,
                                                     "TrackedAllocator::allocate",
                                                     "memory_manager.hpp");
        return static_cast<T*>(ptr);
    }
    
    void deallocate(T* p, std::size_t n) {
        MemoryManager::getInstance().trackDeallocation(p);
        ::operator delete(p);
    }
    
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        new(p) U(std::forward<Args>(args)...);
    }
    
    template<typename U>
    void destroy(U* p) {
        p->~U();
    }
};

// Memory allocation debugging macro
#define TRACKED_NEW new (std::nothrow)
#define TRACKED_DELETE delete

// Memory profiling macro
#ifdef NLM_MEMORY_PROFILING
    #define TRACK_ALLOCATION(ptr, size) \
        MemoryManager::getInstance().trackAllocation(ptr, size, __FILE__, __LINE__, __func__, #func)
    #define TRACK_DEALLOCATION(ptr) \
        MemoryManager::getInstance().trackDeallocation(ptr)
#else
    #define TRACK_ALLOCATION(ptr, size)
    #define TRACK_DEALLOCATION(ptr)
#endif

// Memory allocation operators
void* operator new(size_t size) throw(std::bad_alloc);
void* operator new[](size_t size) throw(std::bad_alloc);
void operator delete(void* ptr) throw();
void operator delete[](void* ptr) throw();

// Integration with NLM core objects
class MemoryProfiler {
public:
    static void profileBrainInitialization(const std::string& type, size_t neurons, size_t synapses);
    static void profileSimulationStep(size_t neurons, size_t synapses, double timeSpent);
    static void profileMemoryUsageSnapshot();
    static std::string getMemoryUsageReport();
    
private:
    static void recordMemorySnapshot(const std::string& description);
    static std::vector<std::string> generateMemoryUsageAnalysis();
};

// Memory validation and health check
class MemoryValidator {
public:
    static bool validateMemoryHealth();
    static std::vector<std::string> getMemoryIssues();
    static bool checkMemoryLeaks();
    static bool checkMemoryCorruption();
    static size_t estimateMemoryFootprint() const;
    
private:
    static bool checkAddressAlignment(void* ptr);
    static bool checkMemoryBounds(void* ptr, size_t size);
    static bool validateSmartPointerConsistency();
};

// Memory optimization suggestions
class MemoryOptimizer {
public:
    static std::vector<std::string> analyzeAllocationPatterns();
    static std::vector<std::string> suggestOptimizations();
    static void applyMemoryOptimizations();
    static size_t estimateMemorySavings() const;
    
private:
    static void analyzeVectorAllocations();
    static void analyzeSmartPointerUsage();
    static void analyzeCachePerformance();
    static void analyzeAllocationFrequency();
};

} // namespace nlm

// Global memory manager instance
inline nlm::MemoryManager& getMemoryManager() {
    return nlm::MemoryManager::getInstance();
}

// Convenience functions for easy integration
inline void enableMemoryTracking() {
    nlm::MemoryManager::getInstance().enableLeakDetection(true);
}

inline void enableMemoryProfiling() {
    nlm::MemoryManager::getInstance().enablePerformanceAnalysis(true);
    nlm::MemoryManager::getInstance().startMemoryProfiling();
}

inline bool checkForMemoryLeaks() {
    return nlm::MemoryManager::getInstance().hasLeaks();
}

inline std::string getMemoryReport() {
    return nlm::MemoryManager::getInstance().generateMemoryReport();
}

inline void exportMemoryReport(const std::string& filename) {
    nlm::MemoryManager::getInstance().exportMemoryReport(filename);
}
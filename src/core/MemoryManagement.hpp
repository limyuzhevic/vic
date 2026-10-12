// Enhanced Memory Management System
// Advanced memory tracking, leak detection, and statistics

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <spdlog/spdlog.h>

#include "core/Logger/Logger.hpp"

namespace nlm {

// Memory leak detection modes
enum class LeakDetectionMode {
    NONE,           // No leak detection
    BASIC,          // Basic allocation tracking
    DETAILED,       // Detailed allocation tracking with stack traces
    AGGRESSIVE,     // Aggressive detection with memory boundaries
    SANITIZED       // Sanitized mode with red zone/green zone
};

// Memory allocation tracking
struct AllocationInfo {
    void* address;
    size_t size;
    std::string typeName;
    std::string file;
    int line;
    std::string function;
    uint64_t timestamp;
    std::string callStack;  // For detailed tracking
    
    AllocationInfo(void* addr, size_t size, const std::string& type,
                   const std::string& file, int line, const std::string& function)
        : address(addr), size(size), typeName(type), file(file), line(line),
          function(function), timestamp(std::chrono::duration_cast<std::chrono::microseconds>(
              std::chrono::high_resolution_clock::now().time_since_epoch()).count()) {}
};

// Memory block tracking
struct MemoryBlockInfo {
    void* baseAddress;
    size_t size;
    std::string type;
    std::string sourceFile;
    int lineNumber;
    std::chrono::steady_clock::time_point allocationTime;
    std::chrono::steady_clock::time_point lastAccessTime;
    bool isFreed;
    uint32_t threadId;
    
    MemoryBlockInfo(void* base, size_t size, const std::string& type,
                   const std::string& file, int line)
        : baseAddress(base), size(size), type(type), sourceFile(file), lineNumber(line),
          allocationTime(std::chrono::steady_clock::now()), lastAccessTime(allocationTime),
          isFreed(false), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {}
};

// Memory statistics
struct MemoryStatistics {
    uint64_t totalAllocated;
    uint64_t totalFreed;
    uint64_t currentUsage;
    uint64_t peakUsage;
    size_t allocationCount;
    size_t freeCount;
    double averageAllocationSize;
    double fragmentationRatio;
    
    MemoryStatistics()
        : totalAllocated(0), totalFreed(0), currentUsage(0), peakUsage(0),
          allocationCount(0), freeCount(0), averageAllocationSize(0.0),
          fragmentationRatio(0.0) {}
    
    double getAllocationEfficiency() const {
        return totalAllocated > 0 ? (1.0 - (totalAllocated - totalFreed) / static_cast<double>(totalAllocated)) : 0.0;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Memory Statistics:\n";
        oss << "  Total Allocated: " << totalAllocated / 1024 << " KB\n";
        oss << "  Total Freed: " << totalFreed / 1024 << " KB\n";
        oss << "  Current Usage: " << currentUsage / 1024 << " KB\n";
        oss << "  Peak Usage: " << peakUsage / 1024 << " KB\n";
        oss << "  Allocations: " << allocationCount << "\n";
        oss << "  Frees: " << freeCount << "\n";
        oss << "  Average Size: " << averageAllocationSize << " bytes\n";
        oss << "  Allocation Efficiency: " << (getAllocationEfficiency() * 100) << "%\n";
        oss << "  Fragmentation Ratio: " << (fragmentationRatio * 100) << "%\n";
        return oss.str();
    }
};

// Memory tracking system
class MemoryTracker {
public:
    MemoryTracker(LeakDetectionMode mode = LeakDetectionMode::BASIC,
                  bool enableStackTrace = false,
                  size_t maxTrackedAllocations = 100000,
                  bool enableMemoryBounds = false);
    
    ~MemoryTracker();
    
    // Disable copying
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    // Allocation tracking
    void* trackAllocation(size_t size, const std::string& typeName,
                         const std::string& file, int line, const std::string& function);
    
    void trackDeallocation(void* ptr, const std::string& typeName);
    
    // Memory usage tracking
    void trackMemoryAccess(void* ptr);
    
    // Statistics
    MemoryStatistics getStatistics() const;
    
    // Leak detection
    std::vector<MemoryBlockInfo> detectLeaks() const;
    std::vector<MemoryBlockInfo> detectLeaks(const std::string& typeFilter) const;
    
    // Reporting
    std::string generateReport() const;
    
    void exportReport(const std::string& filename) const;
    
    // Control
    void setMode(LeakDetectionMode mode);
    void enableStackTrace(bool enable);
    void enableMemoryBounds(bool enable);
    
    // Query
    bool isAllocated(void* ptr) const;
    size_t getAllocationCount() const { return allocationCount_.load(); }
    size_t getDeallocationCount() const { return freeCount_.load(); }
    uint64_t getCurrentUsage() const { return currentUsage_.load(); }
    
    // Reset
    void reset();
    
    // Thread-safe operations
    void lock() { mutex_.lock(); }
    void unlock() { mutex_.unlock(); }
    
private:
    // Helper functions
    void recordAllocation(void* ptr, size_t size, const std::string& typeName,
                         const std::string& file, int line, const std::string& function);
    
    void recordDeallocation(void* ptr, const std::string& typeName);
    
    void updateMemoryUsage() const;
    
    std::string getCallStack() const;
    
    // Data structures
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::unordered_map<void*, MemoryBlockInfo> blocks_;
    mutable std::mutex mutex_;
    
    // Statistics
    mutable std::atomic<uint64_t> totalAllocated_;
    mutable std::atomic<uint64_t> totalFreed_;
    mutable std::atomic<uint64_t> currentUsage_;
    mutable std::atomic<uint64_t> peakUsage_;
    mutable std::atomic<size_t> allocationCount_;
    mutable std::atomic<size_t> freeCount_;
    
    // Configuration
    LeakDetectionMode mode_;
    bool enableStackTrace_;
    size_t maxTrackedAllocations_;
    bool enableMemoryBounds_;
    
    // Logger
    std::shared_ptr<spdlog::logger> logger_;
};

// Global memory tracker instance
extern MemoryTracker* globalMemoryTracker;

// Memory management macros for easy integration
#define NLM_TRACK_ALLOCATION(size) \
    nlm::globalMemoryTracker ? nlm::globalMemoryTracker->trackAllocation(size, #type, __FILE__, __LINE__, __func__) : nullptr

#define NLM_TRACK_DEALLOCATION(ptr) \
    if (ptr && nlm::globalMemoryTracker) \
        nlm::globalMemoryTracker->trackDeallocation(ptr, #type)

#define NLM_TRACK_MEMORY_ACCESS(ptr) \
    if (ptr && nlm::globalMemoryTracker) \
        nlm::globalMemoryTracker->trackMemoryAccess(ptr)

// Memory-safe deallocation wrapper
void safeDeallocate(void* ptr, const std::string& typeName = "unknown");

// Memory validation functions
bool validateMemoryBounds(void* ptr, size_t size);
void* validateAndGetAllocation(void* ptr);

// Memory profiling
struct AllocationProfile {
    std::string typeName;
    size_t allocationCount;
    uint64_t totalSize;
    uint64_t peakSize;
    double averageSize;
    std::string topFile;  // File with most allocations
    int topLine;          // Line number with most allocations
    
    AllocationProfile() : allocationCount(0), totalSize(0), peakSize(0),
                         averageSize(0.0), topLine(0) {}
};

class MemoryProfiler {
public:
    MemoryProfiler(std::shared_ptr<MemoryTracker> tracker);
    
    std::unordered_map<std::string, AllocationProfile> profileAllocations() const;
    
    std::vector<MemoryBlockInfo> getLargestAllocations(size_t limit) const;
    
    std::string generateProfileReport() const;
    
    void exportProfile(const std::string& filename) const;
    
private:
    std::shared_ptr<MemoryTracker> tracker_;
};

// Memory monitoring thread
class MemoryMonitor {
public:
    MemoryMonitor(std::shared_ptr<MemoryTracker> tracker, 
                  std::chrono::milliseconds interval = std::chrono::seconds(1));
    
    ~MemoryMonitor();
    
    void start();
    void stop();
    
    void setThreshold(double threshold);
    double getMemoryUsagePercentage() const;
    
    std::string getStatusReport() const;
    
private:
    void monitoringThread();
    
    std::shared_ptr<MemoryTracker> tracker_;
    std::thread monitorThread_;
    std::atomic<bool> running_;
    std::chrono::milliseconds interval_;
    double memoryThreshold_;
    std::mutex statusMutex_;
    std::string lastStatus_;
};

// Integration with existing memory pools
void integrateMemoryTrackerWithPool(MemoryPool<Neuron>* neuronPool);
void integrateMemoryTrackerWithPool(MemoryPool<Synapse>* synapsePool);

// Configuration functions
void initializeMemoryTracker(LeakDetectionMode mode = LeakDetectionMode::BASIC,
                              bool enableStackTrace = false,
                              bool enableMemoryBounds = false);

MemoryTracker& getMemoryTracker();

void resetMemoryTracker();

// Utility functions
std::string formatMemorySize(uint64_t bytes);

// Global helper functions
namespace MemoryUtils {
    template<typename T>
    void safeDelete(T* ptr) {
        if (ptr) {
            delete ptr;
            if (globalMemoryTracker) {
                globalMemoryTracker->trackDeallocation(ptr, typeid(T).name());
            }
        }
    }
    
    template<typename T>
    void safeDeleteArray(T* ptr) {
        if (ptr) {
            delete[] ptr;
            if (globalMemoryTracker) {
                globalMemoryTracker->trackDeallocation(ptr, typeid(T).name());
            }
        }
    }
    
    template<typename T, typename... Args>
    T* safeMake(Args&&... args) {
        void* ptr = globalMemoryTracker ? 
            globalMemoryTracker->trackAllocation(sizeof(T), typeid(T).name(), "", 0, "") : nullptr;
        if (ptr) {
            return new(ptr) T(std::forward<Args>(args)...);
        }
        return new T(std::forward<Args>(args)...);
    }
    
    template<typename T>
    void safeDestroy(T* ptr) {
        if (ptr) {
            ptr->~T();
            if (globalMemoryTracker) {
                globalMemoryTracker->trackDeallocation(ptr, typeid(T).name());
            }
        }
    }
}

} // namespace nlm

// Global instance declaration
extern nlm::MemoryTracker* nlm::globalMemoryTracker;

// Macro for easy integration
#define TRACK_MEMORY() \
    static bool memoryTrackerInitialized = [](){ \
        nlm::initializeMemoryTracker(nlm::LeakDetectionMode::DETAILED, true, true); \
        return true; \
    }()

// Custom new/delete operators with tracking
void* operator new(size_t size) throw(std::bad_alloc);
void* operator new[](size_t size) throw(std::bad_alloc);
void operator delete(void* ptr) throw();
void operator delete[](void* ptr) throw();
void operator delete(void* ptr, size_t size) throw();
void operator delete[](void* ptr, size_t size) throw();
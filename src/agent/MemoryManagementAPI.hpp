#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <chrono>
#include <optional>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <set>
#include <deque>
#include <array>
#include <fstream>
#include <iostream>
#include "../core/Config/Config.hpp"
#include "../performance/MemoryPool.hpp"

namespace nlm {

// Memory management types
enum class MemoryPoolType {
    Standard,
    Custom,
    ThreadLocal,
    Shared,
    Hierarchical
};

enum class MemoryAllocationType {
    Neuron,
    Synapse,
    Population,
    Region,
    Brain,
    Memory,
    WorkingMemory,
    Custom
};

enum class MemoryLeakSeverity {
    Low,
    Medium,
    High,
    Critical
};

enum class MemoryOptimizationType {
    PoolSizing,
    Alignment,
    FragmentationReduction,
    AllocationBatching,
    Reuse,
    Compression
};

// Custom memory pool configuration
struct CustomMemoryPoolConfig {
    std::string poolName;
    MemoryPoolType poolType;
    size_t initialSize;
    size_t maxSize;
    size_t chunkSize;
    size_t alignment;
    std::string allocatorType; // "std::allocator", "custom", "pool"
    std::map<std::string, std::string> customParameters;
    bool enableThreadLocal;
    bool enableStatsTracking;
    bool enableLeakDetection;
    double growthFactor;
    double shrinkFactor;
    
    CustomMemoryPoolConfig() : poolType(MemoryPoolType::Standard), initialSize(1024),
                               maxSize(1024 * 1024), chunkSize(64), alignment(8),
                               enableThreadLocal(false), enableStatsTracking(true),
                               enableLeakDetection(true), growthFactor(1.5), shrinkFactor(0.8) {}
};

// Memory allocation tracking
struct MemoryAllocationInfo {
    void* address;
    size_t size;
    std::chrono::high_resolution_clock::time_point allocationTime;
    std::string allocationType;
    std::string file;
    size_t line;
    std::string stackTrace;
    size_t threadId;
    std::string allocationContext;
    
    MemoryAllocationInfo() : address(nullptr), size(0), line(0), threadId(0) {}
    
    MemoryAllocationInfo(void* addr, size_t sz, const std::string& type,
                        const std::string& file, size_t line, const std::string& context)
        : address(addr), size(sz), allocationTime(), allocationType(type),
          file(file), line(line), stackTrace(), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
          allocationContext(context) {}
};

// Memory leak report
struct MemoryLeakReport {
    std::vector<void*> leakedAddresses;
    size_t totalLeakedBytes;
    std::map<std::string, size_t> leaksByType;
    std::map<std::string, size_t> leaksByFile;
    MemoryLeakSeverity overallSeverity;
    std::vector<std::string> affectedContexts;
    std::chrono::high_resolution_clock::time_point detectionTime;
    std::string rootCause;
    
    MemoryLeakReport() : totalLeakedBytes(0), overallSeverity(MemoryLeakSeverity::Low) {}
    
    bool hasLeaks() const { return !leakedAddresses.empty(); }
    std::string getSeverityString() const {
        switch (overallSeverity) {
            case MemoryLeakSeverity::Low: return "Low";
            case MemoryLeakSeverity::Medium: return "Medium";
            case MemoryLeakSeverity::High: return "High";
            case MemoryLeakSeverity::Critical: return "Critical";
            default: return "Unknown";
        }
    }
};

// Memory usage optimization recommendation
struct MemoryOptimization {
    std::string optimizationId;
    std::string description;
    MemoryOptimizationType type;
    double estimatedMemorySavings;
    double estimatedPerformanceGain;
    double implementationComplexity; // 1-5 scale
    double riskLevel; // 1-5 scale
    std::vector<std::string> requiredChanges;
    std::string implementationGuide;
    std::chrono::high_resolution_clock::time_point suggestedTime;
    bool isImplemented;
    bool isVerified;
    
    MemoryOptimization() : estimatedMemorySavings(0.0), estimatedPerformanceGain(0.0),
                           implementationComplexity(3.0), riskLevel(2.0),
                           isImplemented(false), isVerified(false) {}
    
    MemoryOptimization(const std::string& id, const std::string& desc,
                      MemoryOptimizationType type, double memorySavings,
                      double performanceGain, double complexity, double risk)
        : optimizationId(id), description(desc), type(type),
          estimatedMemorySavings(memorySavings), estimatedPerformanceGain(performanceGain),
          implementationComplexity(complexity), riskLevel(risk),
          isImplemented(false), isVerified(false) {
        suggestedTime = std::chrono::high_resolution_clock::now();
    }
    
    std::string getPriorityLabel() const {
        if (estimatedMemorySavings > 100.0 * 1024 * 1024) return "Critical"; // 100MB
        if (estimatedMemorySavings > 10.0 * 1024 * 1024) return "High"; // 10MB
        if (estimatedMemorySavings > 1.0 * 1024 * 1024) return "Medium"; // 1MB
        return "Low";
    }
};

// Memory pool statistics
struct MemoryPoolStats {
    std::string poolName;
    size_t totalAllocations{0};
    size_t totalDeallocations{0};
    size_t currentAllocations{0};
    size_t peakAllocations{0};
    size_t totalAllocatedBytes{0};
    size_t peakAllocatedBytes{0};
    size_t fragmentationRatio{0};
    double allocationFailureRate{0.0};
    std::chrono::high_resolution_clock::time_point lastAllocationTime;
    std::chrono::high_resolution_clock::time_point lastDeallocationTime;
    
    void reset() {
        totalAllocations = totalDeallocations = currentAllocations = peakAllocations = 0;
        totalAllocatedBytes = peakAllocatedBytes = 0;
        fragmentationRatio = 0;
        allocationFailureRate = 0.0;
    }
};

// Memory allocation profiler
class MemoryAllocationProfiler {
public:
    MemoryAllocationProfiler();
    ~MemoryAllocationProfiler();
    
    // Disable copying
    MemoryAllocationProfiler(const MemoryAllocationProfiler&) = delete;
    MemoryAllocationProfiler& operator=(const MemoryAllocationProfiler&) = delete;
    
    // Singleton access
    static MemoryAllocationProfiler& getInstance() {
        static MemoryAllocationProfiler instance;
        return instance;
    }
    
    // Memory tracking
    void* trackAllocation(size_t size, const std::string& type,
                         const std::string& file, size_t line,
                         const std::string& context = "");
    void trackDeallocation(void* address, const std::string& type,
                         const std::string& file, size_t line,
                         const std::string& context = "");
    
    // Memory leak detection
    MemoryLeakReport detectLeaks() const;
    std::vector<MemoryAllocationInfo> getActiveAllocations() const;
    std::vector<MemoryAllocationInfo> getAllocationsByType(const std::string& type) const;
    std::vector<MemoryAllocationInfo> getAllocationsByFile(const std::string& file) const;
    
    // Memory statistics
    struct AllocationStats {
        size_t totalAllocations{0};
        size_t totalDeallocations{0};
        size_t currentAllocations{0};
        size_t totalAllocatedBytes{0};
        size_t peakAllocatedBytes{0};
        double allocationFailureRate() const {
            size_t total = totalAllocations + totalDeallocations;
            return total > 0 ? static_cast<double>(totalAllocations) / total : 0.0;
        }
        
        void reset() {
            totalAllocations = totalDeallocations = currentAllocations = 0;
            totalAllocatedBytes = peakAllocatedBytes = 0;
        }
    };
    
    AllocationStats getAllocationStats() const;
    
    // Memory leak analysis
    std::map<std::string, size_t> analyzeLeaksByType() const;
    std::map<std::string, size_t> analyzeLeaksByFile() const;
    std::vector<std::string> getStackTracesForAddress(void* address) const;
    
    // Configuration integration
    void setProfilerConfig(const Config& config);
    Config getProfilerConfig() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
private:
    // Thread-local allocation tracking
    struct ThreadLocalAllocations {
        std::vector<MemoryAllocationInfo> allocations;
        mutable std::mutex mutex;
    };
    
    // Internal allocation tracking
    ThreadLocalAllocations& getThreadLocalAllocations();
    
    // Memory tracking
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
    std::unordered_map<size_t, ThreadLocalAllocations> threadLocalAllocations_;
    
    // Statistics
    AllocationStats stats_;
    mutable std::mutex statsMutex_;
    
    // Configuration
    Config profilerConfig_;
    mutable std::mutex configMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
};

// Custom memory pool manager
class CustomMemoryPoolManager {
public:
    CustomMemoryPoolManager();
    ~CustomMemoryPoolManager();
    
    // Disable copying
    CustomMemoryPoolManager(const CustomMemoryPoolManager&) = delete;
    CustomMemoryPoolManager& operator=(const CustomMemoryPoolManager&) = delete;
    
    // Singleton access
    static CustomMemoryPoolManager& getInstance() {
        static CustomMemoryPoolManager instance;
        return instance;
    }
    
    // Pool management
    bool createPool(const CustomMemoryPoolConfig& config);
    bool deletePool(const std::string& poolName);
    bool updatePool(const std::string& poolName, const CustomMemoryPoolConfig& config);
    MemoryPoolStats getPoolStats(const std::string& poolName) const;
    std::vector<MemoryPoolStats> getAllPoolStats() const;
    
    // Allocation/deallocation
    void* allocateFromPool(const std::string& poolName, size_t size,
                         const std::string& allocationType = "custom");
    bool deallocateFromPool(const std::string& poolName, void* pointer,
                          size_t size);
    
    // Memory optimization
    std::vector<MemoryOptimization> analyzeForOptimizations() const;
    bool applyOptimization(const MemoryOptimization& optimization);
    bool applyAllOptimizations();
    
    // Memory monitoring
    bool startMemoryMonitoring();
    void stopMemoryMonitoring();
    bool isMemoryMonitoringActive() const;
    
    // Configuration integration
    void setPoolManagerConfig(const Config& config);
    Config getPoolManagerConfig() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
private:
    // Pool implementation
    class MemoryPoolImpl {
    public:
        MemoryPoolImpl(const CustomMemoryPoolConfig& config);
        ~MemoryPoolImpl();
        
        // Disable copying
        MemoryPoolImpl(const MemoryPoolImpl&) = delete;
        MemoryPoolImpl& operator=(const MemoryPoolImpl&) = delete;
        
        void* allocate(size_t size, const std::string& type);
        bool deallocate(void* pointer, size_t size);
        bool resizePool(size_t newSize);
        MemoryPoolStats getStats() const;
        
    private:
        CustomMemoryPoolConfig config_;
        mutable std::mutex mutex_;
        std::vector<void*> freeBlocks_;
        MemoryPoolStats stats_;
    };
    
    // Internal pool management
    std::unordered_map<std::string, std::unique_ptr<MemoryPoolImpl>> pools_;
    mutable std::mutex poolsMutex_;
    
    // Memory monitoring
    bool memoryMonitoringActive_;
    mutable std::mutex monitoringMutex_;
    
    // Configuration
    Config poolManagerConfig_;
    mutable std::mutex configMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
    
    // Thread-local pools
    std::unordered_map<size_t, std::string> threadLocalPoolNames_;
    mutable std::mutex threadLocalMutex_;
};

// Advanced memory management API
class MemoryManagementAPI {
public:
    MemoryManagementAPI();
    ~MemoryManagementAPI();
    
    // Disable copying
    MemoryManagementAPI(const MemoryManagementAPI&) = delete;
    MemoryManagementAPI& operator=(const MemoryManagementAPI&) = delete;
    
    // Singleton access
    static MemoryManagementAPI& getInstance() {
        static MemoryManagementAPI instance;
        return instance;
    }
    
    // Memory pool operations
    bool createMemoryPool(const CustomMemoryPoolConfig& config);
    bool deleteMemoryPool(const std::string& poolName);
    bool allocateFromPool(const std::string& poolName, size_t size,
                        void*& outPointer, const std::string& allocationType = "custom");
    bool deallocateFromPool(const std::string& poolName, void* pointer, size_t size);
    
    // Memory allocation tracking
    void* trackAllocation(size_t size, const std::string& type,
                         const std::string& file, size_t line,
                         const std::string& context = "");
    bool trackDeallocation(void* address, const std::string& type,
                         const std::string& file, size_t line,
                         const std::string& context = "");
    
    // Memory leak detection
    MemoryLeakReport detectMemoryLeaks();
    std::vector<MemoryAllocationInfo> getActiveAllocations();
    std::vector<MemoryAllocationInfo> getAllocationsByType(const std::string& type);
    std::vector<MemoryAllocationInfo> getAllocationsByFile(const std::string& file);
    
    // Memory optimization
    std::vector<MemoryOptimization> analyzeMemoryOptimizations();
    bool applyMemoryOptimization(const MemoryOptimization& optimization);
    bool applyAllMemoryOptimizations();
    
    // Memory monitoring
    bool startMemoryMonitoring();
    void stopMemoryMonitoring();
    bool isMemoryMonitoringActive() const;
    
    // Memory statistics
    struct SystemMemoryStats {
        size_t totalAllocatedBytes{0};
        size_t peakAllocatedBytes{0};
        size_t totalAllocationCount{0};
        size_t totalDeallocationCount{0};
        double memoryUtilizationRatio{0.0};
        size_t fragmentationLevel{0};
        std::map<std::string, MemoryPoolStats> poolStats;
        std::map<std::string, size_t> allocationCountsByType;
        
        void reset() {
            totalAllocatedBytes = peakAllocatedBytes = 0;
            totalAllocationCount = totalDeallocationCount = 0;
            memoryUtilizationRatio = 0.0;
            fragmentationLevel = 0;
            poolStats.clear();
            allocationCountsByType.clear();
        }
    };
    
    SystemMemoryStats getSystemMemoryStats() const;
    MemoryPoolStats getPoolStats(const std::string& poolName) const;
    
    // Configuration integration
    void setMemoryManagementConfig(const Config& config);
    Config getMemoryManagementConfig() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Error handling
    std::string getLastError() const { return lastError_; }
    void setLastError(const std::string& error) { lastError_ = error; }
    
    // Integration with existing MemoryPool
    bool integrateWithExistingMemoryPool(const std::string& poolName);
    bool optimizeExistingMemoryPool(const std::string& poolName);
    
private:
    // Internal memory management
    void* performAllocation(size_t size, const std::string& type,
                          const std::string& file, size_t line,
                          const std::string& context);
    bool performDeallocation(void* pointer, size_t size,
                          const std::string& type,
                          const std::string& file, size_t line,
                          const std::string& context);
    
    // Configuration integration
    void applyMemoryPoolConfig();
    
    // Thread synchronization
    mutable std::mutex systemMutex_;
    std::atomic<bool> isLocked_;
    
    // Memory profiling
    MemoryAllocationProfiler profiler_;
    
    // Memory pool management
    CustomMemoryPoolManager poolManager_;
    
    // Configuration
    Config memoryManagementConfig_;
    mutable std::mutex configMutex_;
    
    // Error handling
    std::string lastError_;
    mutable std::mutex errorMutex_;
};

// Memory management helper functions
inline MemoryLeakSeverity calculateLeakSeverity(size_t totalLeakedBytes,
                                                const std::map<std::string, size_t>& leaksByType) {
    if (totalLeakedBytes > 100 * 1024 * 1024) return MemoryLeakSeverity::Critical; // 100MB
    if (totalLeakedBytes > 10 * 1024 * 1024) return MemoryLeakSeverity::High; // 10MB
    if (totalLeakedBytes > 1 * 1024 * 1024) return MemoryLeakSeverity::Medium; // 1MB
    return MemoryLeakSeverity::Low;
}

inline std::string leakSeverityToString(MemoryLeakSeverity severity) {
    switch (severity) {
        case MemoryLeakSeverity::Low: return "Low";
        case MemoryLeakSeverity::Medium: return "Medium";
        case MemoryLeakSeverity::High: return "High";
        case MemoryLeakSeverity::Critical: return "Critical";
        default: return "Unknown";
    }
}

} // namespace nlm
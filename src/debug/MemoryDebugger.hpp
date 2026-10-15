// NLM Advanced Debugging and Profiling Tools - Memory Debugger

#pragma once

#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <chrono>
#include <iomanip>

namespace nlm {

// Memory allocation record
struct AllocationRecord {
    void* address;
    size_t size;
    std::string sourceFile;
    int sourceLine = 0;
    std::string sourceFunction;
    std::chrono::system_clock::time_point allocationTime;
    std::map<std::string, std::string> tags;
    bool isLeaked = false;
    
    AllocationRecord(void* addr, size_t sz, const std::string& file, int line, const std::string& func)
        : address(addr), size(sz), sourceFile(file), sourceLine(line), sourceFunction(func),
          allocationTime(std::chrono::system_clock::now()), isLeaked(false) {}
};

// Memory debugging configuration
struct MemoryDebugConfig {
    bool enableAllocationTracking = true;
    bool enableDeallocationTracking = true;
    bool enableLeakDetection = true;
    bool enableBufferOverflowDetection = true;
    bool enableUseAfterFreeDetection = true;
    size_t maxAllocationHistory = 10000;
    size_t maxStackTraceDepth = 50;
    double leakDetectionThreshold = 0.01; // 1% threshold
    std::string allocationLogFile = "allocation_log.txt";
    std::string leakReportFile = "memory_leak_report.txt";
    bool generateDetailedReports = true;
};

// Memory debugger class
class MemoryDebugger {
public:
    MemoryDebugger(const MemoryDebugConfig& config = MemoryDebugConfig());
    ~MemoryDebugger();
    
    // Allocation tracking
    void* trackAllocation(size_t size, const std::string& sourceFile, int sourceLine, const std::string& sourceFunction);
    void trackDeallocation(void* address);
    
    // Leak detection
    std::vector<AllocationRecord> detectMemoryLeaks() const;
    size_t getTotalMemoryAllocated() const;
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    // Buffer overflow detection
    bool checkBufferOverflow(void* address, size_t accessSize) const;
    void markAllocationForBoundsChecking(void* address, size_t size);
    
    // Use-after-free detection
    bool isUseAfterFree(void* address) const;
    void markAllocationForUseAfterFreeTracking(void* address);
    
    // Memory allocation overrides (for testing)
    void* mallocOverride(size_t size, const std::string& sourceFile, int sourceLine, const std::string& sourceFunction);
    void freeOverride(void* ptr, const std::string& sourceFile, int sourceLine, const std::string& sourceFunction);
    void* callocOverride(size_t num, size_t size, const std::string& sourceFile, int sourceLine, const std::string& sourceFunction);
    void* reallocOverride(void* ptr, size_t size, const std::string& sourceFile, int sourceLine, const std::string& sourceFunction);
    
    // Control methods
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const { return running_; }
    
    // Configuration
    void updateConfig(const MemoryDebugConfig& config);
    const MemoryDebugConfig& getConfig() const { return config_; }
    
    // Statistics and reporting
    std::string getMemoryUsageReport() const;
    std::string getLeakReport() const;
    std::string getAllocationHistory() const;
    size_t getAllocationCount() const;
    size_t getDeallocationCount() const;
    
    // Integration
    void attachToBrain(const std::string& brainId);
    void attachToExperiment(const std::string& experimentName);
    
    // Event handling
    void setMemoryAllocationCallback(std::function<void(const AllocationRecord&)> callback);
    void setMemoryDeallocationCallback(std::function<void(void* address)> callback);
    void setMemoryLeakCallback(std::function<void(const AllocationRecord&)> callback);
    
    // Export functionality
    bool exportAllocationHistory(const std::string& filename, const std::string& format = "csv");
    bool exportLeakReport(const std::string& filename, const std::string& format = "html");
    
private:
    std::atomic<bool> running_;
    mutable std::mutex mutex_;
    
    MemoryDebugConfig config_;
    
    // Allocation tracking
    std::unordered_map<void*, AllocationRecord> allocations_;
    std::vector<AllocationRecord> allocationHistory_;
    std::unordered_map<void*, size_t> boundsCheckingMap_;
    std::unordered_set<void*> useAfterFreeTrackingMap_;
    
    // Statistics
    std::atomic<size_t> allocationCount_;
    std::atomic<size_t> deallocationCount_;
    std::atomic<size_t> peakMemoryUsage_;
    std::atomic<size_t> totalMemoryAllocated_;
    
    // Callbacks
    std::function<void(const AllocationRecord&)> allocationCallback_;
    std::function<void(void* address)> deallocationCallback_;
    std::function<void(const AllocationRecord&)> leakCallback_;
    
    // Thread-local storage for stack traces
    struct ThreadLocalData {
        std::vector<std::string> stackTrace;
    };
    
    // Internal methods
    AllocationRecord createAllocationRecord(void* address, size_t size, 
                                            const std::string& sourceFile, int sourceLine, 
                                            const std::string& sourceFunction);
    std::vector<std::string> captureStackTrace(int maxDepth = 50) const;
    void cleanupDeallocatedAllocations();
    void checkForLeaks() const;
    bool isAddressAllocated(void* address) const;
    void logAllocation(const AllocationRecord& record);
    void logDeallocation(void* address);
    std::string getTimestamp() const;
    size_t calculateMemoryFootprint() const;
};

} // namespace nlm

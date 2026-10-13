// NLM Memory Analyzer - Memory analysis and debugging tools
// Provides comprehensive memory usage tracking and leak detection

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Memory allocation tracking
struct MemoryAllocation {
    void* address;
    size_t size;
    std::chrono::system_clock::time_point timestamp;
    std::string allocationSite;
    std::string backtrace;  // For leak detection
    
    MemoryAllocation(void* addr, size_t sz, const std::string& site)
        : address(addr), size(sz), timestamp(std::chrono::system_clock::now()),
          allocationSite(site) {}
};

// Memory block state
enum class MemoryBlockState {
    Allocated,
    Freed,
    Leaked,
    Unknown
};

// Memory analyzer
class MemoryAnalyzer {
public:
    MemoryAnalyzer();
    ~MemoryAnalyzer();

    // Memory tracking control
    void startTracking();
    void stopTracking();
    bool isTracking() const { return tracking_; }
    
    // Allocation tracking
    void trackAllocation(void* address, size_t size, const std::string& allocationSite = "");
    void trackDeallocation(void* address);
    
    // Memory queries
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    size_t getTotalAllocated() const;
    size_t getTotalDeallocated() const;
    size_t getAllocatedBlockCount() const;
    
    // Block inspection
    bool isBlockAllocated(void* address) const;
    size_t getBlockSize(void* address) const;
    std::string getAllocationSite(void* address) const;
    
    // Leak detection
    void detectMemoryLeaks();
    std::vector<void*> findLeakedBlocks() const;
    
    // Statistics and reporting
    std::string generateReport() const;
    void printReport() const;
    
    // Comparison
    double compareToBaseline(const std::map<std::string, size_t>& baseline) const;
    
    // Integration with profiler
    void integrateWithProfiler(class Profiler& profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool tracking_;
    mutable std::mutex memoryMutex_;
    
    // Memory tracking helpers
    void* allocateWrapper(size_t size, const std::string& site);
    void deallocateWrapper(void* ptr);
    
    // Block state management
    void updateBlockState(void* address, MemoryBlockState state, size_t size, const std::string& site);
};

// Memory snapshot for comparison
struct MemorySnapshot {
    std::chrono::system_clock::time_point timestamp;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    std::map<std::string, size_t> allocationSites;
    
    MemorySnapshot() : currentUsage(0), peakUsage(0), allocationCount(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Memory leak detector
class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Detection control
    void startDetection(size_t detectionInterval = 1000);
    void stopDetection();
    bool isDetecting() const { return detecting_; }
    
    // Block registration
    void registerAllocation(void* address, size_t size, const std::string& context = "");
    void registerDeallocation(void* address);
    
    // Leak analysis
    bool hasLeaks() const;
    size_t getLeakCount() const;
    std::vector<void*> getLeakedBlocks() const;
    std::map<std::string, size_t> getLeaksBySite() const;
    double getEstimatedLeakSize() const;
    
    // Reporting
    std::string generateLeakReport() const;
    void printLeakReport() const;
    
    // Comparison with baseline
    void setBaseline(const MemorySnapshot& baseline);
    double calculateLeakGrowth(const MemorySnapshot& current) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool detecting_;
    size_t detectionInterval_;
    std::chrono::system_clock::time_point lastDetectionTime_;
    mutable std::mutex leakMutex_;
    
    // Leak tracking
    std::map<void*, MemoryAllocation> allocatedBlocks_;
    std::vector<void*> currentlyLeaked_;
    std::chrono::system_clock::time_point detectionStartTime_;
    
    // Baseline tracking
    std::vector<MemorySnapshot> history_;
    std::optional<MemorySnapshot> baseline_;
};

// Convenience functions
MemoryAnalyzer& getMemoryAnalyzer();
void initializeMemoryAnalyzer();
void shutdownMemoryAnalyzer();

MemoryLeakDetector& getMemoryLeakDetector();
void initializeMemoryLeakDetector();
void shutdownMemoryLeakDetector();

} // namespace nlm

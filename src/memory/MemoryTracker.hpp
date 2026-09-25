#pragma once

/**
 * NLM Memory Tracking and Monitoring Infrastructure
 * 
 * Comprehensive real-time memory tracking system for the NLM (Neural Learning Machine) framework.
 * Provides detailed memory usage analysis, allocation/deallocation tracking, and performance
 * optimization recommendations for memory management.
 * 
 * Features:
 * - Real-time memory usage monitoring with millisecond precision
 * - Detailed allocation/deallocation tracking with stack traces
 * - Memory pool efficiency analysis and fragmentation detection
 * - Memory leak detection and reporting
 * - Memory bandwidth and cache efficiency measurements
 * - Cross-platform memory debugging support
 * - Zero-overhead operation when disabled
 * 
 * Integration:
 * - Works with existing MemoryPool infrastructure
 * - Integrates with EventQueue for event memory tracking
 * - Supports thread-local memory tracking for parallel processing
 * - Compatible with Brain.cpp simulation memory management
 * - Provides Python bindings for user-friendly memory APIs
 */

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <mutex>
#include <shared_mutex>
#include <deque>
#include <set>
#include <functional>

// Configuration
#define NLM_MEMORY_TRACKING_ENABLED 1

// Forward declarations
namespace nlm {
    class MemoryPool;
    class Brain;
    class EventQueue;
    class ParallelProcessing;
    class SIMDVectorization;
}

// Memory tracking types
enum class AllocationType {
    Standard,
    Pool,
    ThreadLocal,
    SIMD,
    Event,
    Simulation
};

enum class MemoryLeakType {
    UnfreedAllocation,
    PoolDepletion,
    Fragmentation,
    CrossThreadReference,
    SimulationError
};

struct AllocationRecord {
    size_t timestamp;           // Simulation time when allocated
    size_t address;              // Memory address
    size_t size;                 // Size in bytes
    std::string allocationSite;  // File and line where allocated
    std::string stackTrace;     // Stack trace for debugging
    AllocationType type;         // Type of allocation
    uint32_t threadId;           // Thread ID
    bool tracked;                // Whether this allocation is currently tracked
    
    AllocationRecord(size_t ts, size_t addr, size_t sz, 
                    const std::string& site, const std::string& trace,
                    AllocationType allocType, uint32_t tid)
        : timestamp(ts), address(addr), size(sz), allocationSite(site),
          stackTrace(trace), type(allocType), threadId(tid), tracked(true) {}
};

struct DeallocationRecord {
    size_t timestamp;           // Simulation time when deallocated
    size_t address;              // Memory address
    std::string deallocationSite; // File and line where deallocated
    uint32_t threadId;           // Thread ID
    bool wasTracked;             // Whether this address was being tracked
    
    DeallocationRecord(size_t ts, size_t addr, const std::string& site, uint32_t tid, bool tracked)
        : timestamp(ts), address(addr), deallocationSite(site), threadId(tid), wasTracked(tracked) {}
};

struct MemoryBlockInfo {
    size_t startAddress;         // Start address of block
    size_t size;                 // Size in bytes
    AllocationType type;         // Block type
    std::string description;     // Description of block
    uint32_t threadId;           // Owning thread
    std::vector<size_t> associatedAllocations; // Related allocation records
    double lastAccessTime;       // Time of last access (for LRU)
};

struct MemoryEfficiencyMetrics {
    double allocationEfficiency;  // Ratio of allocated to available memory
    double fragmentationRatio;   // Degree of memory fragmentation
    size_t totalAllocated;       // Total allocated bytes
    size_t totalDeallocated;     // Total deallocated bytes
    size_t peakMemoryUsage;      // Peak memory usage during simulation
    size_t currentMemoryUsage;   // Current memory usage
    size_t activeAllocations;   // Number of currently active allocations
    size_t averageAllocationSize; // Average allocation size
    double bandwidthUtilization; // Memory bandwidth utilization
    size_t cacheMisses;          // Number of cache misses
    size_t cacheHits;            // Number of cache hits
};

struct MemoryLeakDetection {
    MemoryLeakType type;         // Type of leak detected
    size_t address;              // Address of leaked memory
    size_t size;                 // Size of leaked block
    std::string allocationSite;  // Where allocated
    std::string stackTrace;     // Stack trace at allocation
    uint32_t threadId;           // Thread that allocated
    size_t timeSinceAllocation;  // Time since allocation (sim steps)
    double confidenceScore;      // Confidence level (0-1) for leak detection
};

class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Singleton pattern
    static MemoryTracker& getInstance();
    
    // Configuration and control
    void enable(bool enable = true);
    bool isEnabled() const { return enabled_; }
    void setMaxHistorySize(size_t maxSize) { maxHistorySize_ = maxSize; }
    void setSamplingRate(double rate) { samplingRate_ = std::max(0.0, std::min(1.0, rate)); }
    
    // Allocation tracking
    void trackAllocation(size_t address, size_t size, const std::string& site,
                         AllocationType type, const std::string& stackTrace);
    void trackDeallocation(size_t address, const std::string& site);
    void trackMemoryPoolUsage(size_t allocated, size_t available, const std::string& poolName);
    
    // Integration with NLM systems
    void integrateWithMemoryPool(nlm::MemoryPool* memoryPool);
    void integrateWithBrain(nlm::Brain* brain);
    void integrateWithEventQueue(nlm::EventQueue* eventQueue);
    void integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc);
    
    // Analysis and reporting
    MemoryEfficiencyMetrics getEfficiencyMetrics() const;
    std::vector<MemoryLeakDetection> detectMemoryLeaks() const;
    std::vector<MemoryBlockInfo> getMemoryBlocks() const;
    std::vector<AllocationRecord> getAllocationHistory(size_t address) const;
    std::vector<DeallocationRecord> getDeallocationHistory(size_t address) const;
    
    // Real-time monitoring
    void recordAccess(size_t address, uint32_t threadId);
    double getCurrentMemoryUsage() const;
    size_t getTotalMemoryAllocated() const;
    size_t getTotalMemoryDeallocated() const;
    
    // Bandwidth and performance
    void recordMemoryAccess(size_t bytes, bool cacheHit);
    double getMemoryBandwidth() const;
    double getCacheEfficiency() const;
    
    // Simulation integration
    void startStep(size_t step, double time);
    void endStep(size_t step, double time);
    
    // Reporting and analysis
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Cleanup
    void flush();
    void reset();
    
private:
    struct AllocationTracker {
        size_t address;
        size_t size;
        std::string allocationSite;
        std::string stackTrace;
        AllocationType type;
        uint32_t threadId;
        size_t allocationTime;
        bool tracked;
        
        AllocationTracker(size_t addr, size_t sz, const std::string& site,
                         const std::string& trace, AllocationType type, uint32_t tid)
            : address(addr), size(sz), allocationSite(site), stackTrace(trace),
              type(type), threadId(tid), allocationTime(0), tracked(true) {}
    };
    
    // Core tracking state
    bool enabled_;
    bool initialized_;
    size_t maxHistorySize_;
    double samplingRate_;
    
    // Integration pointers
    nlm::MemoryPool* memoryPool_;
    nlm::Brain* brain_;
    nlm::EventQueue* eventQueue_;
    nlm::ParallelProcessing* parallelProc_;
    
    // Tracking data structures
    std::vector<AllocationTracker> activeAllocations_;
    std::vector<AllocationRecord> allocationHistory_;
    std::vector<DeallocationRecord> deallocationHistory_;
    std::vector<MemoryBlockInfo> memoryBlocks_;
    std::vector<std::pair<size_t, bool>> memoryAccesses_; // (bytes, cacheHit)
    
    // Memory leak detection
    std::vector<MemoryLeakDetection> detectedLeaks_;
    std::unordered_set<size_t> leakedAddresses_;
    
    // Statistics
    std::atomic<size_t> totalMemoryAllocated_;
    std::atomic<size_t> totalMemoryDeallocated_;
    std::atomic<size_t> currentMemoryUsage_;
    std::atomic<size_t> peakMemoryUsage_;
    std::atomic<size_t> totalMemoryAccesses_;
    std::atomic<size_t> cacheHits_;
    std::atomic<double> memoryBandwidth_;
    
    // Simulation tracking
    size_t currentStep_;
    double currentTime_;
    
    // Thread safety
    mutable std::shared_mutex dataMutex_;
    std::mutex leakDetectionMutex_;
    
    // Utility functions
    std::string getCurrentStackTrace() const;
    std::string getAllocationSite() const;
    size_t getCurrentTime() const;
    void updateMemoryBlocks();
    void analyzeMemoryLeaks();
    void generateLeakReport(std::stringstream& report) const;
    void generateEfficiencyReport(std::stringstream& report) const;
    void generateIntegrationReport(std::stringstream& report) const;
    
    // Leak detection algorithms
    void detectUnfreedAllocations();
    void detectPoolDepletion();
    void detectFragmentation();
};
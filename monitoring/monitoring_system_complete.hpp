# Performance Monitoring Utilities for NLM

This document provides comprehensive performance monitoring utilities for the NLM neural simulation framework.

## Overview

The NLM Performance Monitoring System provides real-time and offline performance analysis capabilities:

1. **Real-time Monitoring**: Live performance metrics during simulation
2. **Post-Simulation Analysis**: Detailed profiling of completed runs
3. **Bottleneck Identification**: Automatic detection of performance constraints
4. **Optimization Guidance**: Recommendations for performance improvements
5. **Integration**: Seamless integration with existing NLM architecture

## Core Components

### 1. Performance Event System

```cpp
// nlm/performance/MonitoringEvent.hpp

#pragma once

#include <chrono>
#include <string>
#include <variant>
#include <unordered_map>

namespace nlm {

enum class EventType {
    SimulationStep,
    MemoryAllocation,
    MemoryDeallocation,
    CacheMiss,
    LockContention,
    ThreadStart,
    ThreadEnd,
    SIMDInstruction,
    BranchPrediction,
    PageFault,
    Custom
};

struct PerformanceEvent {
    uint64_t timestamp;                    // Nanoseconds since epoch
    EventType type;                        // Event type
    uint64_t threadId;                     // Thread that generated event
    uint64_t step;                         // Current simulation step
    double simulationTime;                 // Current simulation time
    
    // Event-specific data (use variant for flexibility)
    std::variant<
        size_t,                          // count (for allocation events)
        std::pair<size_t, size_t>,       // address range (for memory events)
        bool,                            // hit/miss (for cache events)
        std::string,                     // message (for custom events)
        double                           // value (for metric events)
    > data;
    
    PerformanceEvent(EventType type, uint64_t threadId, uint64_t step,
                     double simTime) : type(type), threadId(threadId), step(step),
                                       simulationTime(simTime), timestamp(getNanoseconds()) {}
    
    static uint64_t getNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

class PerformanceEventBus {
public:
    PerformanceEventBus(size_t capacity = 1000000);
    ~PerformanceEventBus();
    
    // Event publishing
    void publish(const PerformanceEvent& event);
    void publish(EventType type, uint64_t threadId, uint64_t step,
                 double simTime, const std::variant<size_t, std::pair<size_t, size_t>, 
                   bool, std::string, double>& data);
    
    // Event subscription
    using EventCallback = std::function<void(const PerformanceEvent&)>
    class EventSubscriber {
    public:
        EventSubscriber(std::function<void(const PerformanceEvent&)> callback);
        void onEvent(const PerformanceEvent& event);
    };
    
    EventSubscriber subscribe(EventType type, std::function<void(const PerformanceEvent&)> callback);
    
    // Event retrieval
    std::vector<PerformanceEvent> getEvents(EventType type, uint64_t startStep = 0,
                                           uint64_t endStep = UINT64_MAX) const;
    size_t getEventCount(EventType type) const;
    
    // Memory management
    void clear();
    size_t memoryUsage() const;
    
private:
    struct EventBuffer {
        std::vector<PerformanceEvent> events;
        std::mutex mutex;
        std::condition_variable cv;
    };
    
    std::vector<EventBuffer> eventBuffers_;
    std::vector<std::vector<EventSubscriber>> subscribers_;
    std::atomic<uint64_t> eventCounter_;
};

} // namespace nlm
```

### 2. Real-Time Performance Monitor

```cpp
// nlm/performance/RealTimePerformanceMonitor.hpp

#pragma once

#include "MonitoringEvent.hpp"
#include "../brain/Brain.hpp"
#include <atomic>
#include <thread>
#include <unordered_map>
#include <functional>

namespace nlm {

struct RealTimeMetrics {
    // Core performance
    uint64_t currentStep;                  // Current simulation step
    double currentSimulationTime;          // Current simulation time
    uint64_t totalSpikes;                  // Total spikes processed
    uint64_t totalNeurons;                 // Total neurons in simulation
    uint64_t activeNeurons;                // Currently active neurons
    
    // Performance indicators
    double stepsPerSecond;                 // Current simulation speed
    double neuronsPerSecond;               // Processing throughput
    double memoryUsageMB;                  // Current memory usage
    float cacheHitRate;                    // Cache efficiency (0-1)
    uint32_t lockContentionCount;         // Lock contention events
    
    // System resources
    uint32_t activeThreadCount;            // Number of active threads
    double cpuUsagePercent;               // CPU utilization
    uint64_t pageFaultCount;               // Page fault events
    
    // Neural system metrics
    float averageFiringRate;               // Average firing rate (Hz)
    float excitationInhibitionRatio;       // E/I balance
    uint32_t plasticityEvents;             // Plasticity update events
    
    // Memory system metrics
    uint64_t allocationCount;              // Number of memory allocations
    uint64_t deallocationCount;            // Number of memory deallocations
    uint64_t cacheLinesAllocated;          // Cache lines allocated
    
    RealTimeMetrics() : currentStep(0), currentSimulationTime(0), totalSpikes(0),
                       totalNeurons(0), activeNeurons(0), stepsPerSecond(0),
                       neuronsPerSecond(0), memoryUsageMB(0), cacheHitRate(0),
                       lockContentionCount(0), activeThreadCount(0), cpuUsagePercent(0),
                       pageFaultCount(0), averageFiringRate(0), excitationInhibitionRatio(0),
                       plasticityEvents(0), allocationCount(0), deallocationCount(0),
                       cacheLinesAllocated(0) {}
};

class RealTimePerformanceMonitor {
public:
    RealTimePerformanceMonitor(Brain& brain, PerformanceEventBus& eventBus);
    ~RealTimePerformanceMonitor();
    
    // Monitor lifecycle
    void start(double updateInterval = 0.1);  // Update every 100ms
    void stop();
    bool isRunning() const { return monitoringThread_.joinable(); }
    
    // Metrics access
    RealTimeMetrics getCurrentMetrics() const;
    std::vector<RealTimeMetrics> getMetricsHistory(size_t count) const;
    
    // Monitoring configuration
    void setUpdateInterval(double interval) { updateInterval_ = interval; }
    void setThresholds(const std::unordered_map<std::string, double>& thresholds);
    void setAlertCallback(std::function<void(const std::string&, double)> callback);
    
    // Integration with Brain
    void setBrain(Brain& brain) { brain_ = &brain; }
    void attachToBrainStep(std::function<void()> stepCallback);
    
    // Event handling
    void onEvent(const PerformanceEvent& event);
    void setEventCallback(std::function<void(const PerformanceEvent&)> callback);
    
    // Export and analysis
    void exportMetrics(const std::string& filename) const;
    void generateReport(const std::string& filename) const;
    
    // Performance analysis
    double calculateStepsPerSecond(double timeWindow) const;
    double calculateEfficiency(double idealStepsPerSecond) const;
    std::string identifyBottlenecks() const;
    
private:
    void monitoringLoop();
    void updateMetrics();
    void checkThresholds();
    void publishMetrics();
    
    Brain* brain_;
    PerformanceEventBus& eventBus_;
    std::thread monitoringThread_;
    std::atomic<bool> running_;
    
    RealTimeMetrics currentMetrics_;
    std::vector<RealTimeMetrics> metricsHistory_;
    double updateInterval_;
    
    // Thresholds for alerts
    std::unordered_map<std::string, double> thresholds_;
    std::function<void(const std::string&, double)> alertCallback_;
    
    // Event processing
    std::function<void(const PerformanceEvent&)> eventCallback_;
    mutable std::mutex metricsMutex_;
    
    // Performance tracking
    uint64_t stepCount_;                   // Steps processed in current window
    uint64_t spikeCount_;                  // Spikes processed in current window
    std::chrono::high_resolution_clock::time_point windowStart_;
};

} // namespace nlm
```

### 3. Memory Profiler

```cpp
// nlm/performance/MemoryProfiler.hpp

#pragma once

#include "MonitoringEvent.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

struct AllocationInfo {
    uint64_t timestamp;                    // Allocation time
    size_t size;                           // Size in bytes
    const char* file;                      // File where allocated
    int line;                              // Line number
    std::string allocationStack;           // Call stack at allocation
    
    AllocationInfo(size_t size, const char* file, int line) 
        : size(size), file(file), line(line), timestamp(getNanoseconds()) {}
    
    static uint64_t getNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

struct MemoryProfile {
    uint64_t totalAllocated;               // Total bytes allocated
    uint64_t peakAllocated;                // Peak allocation
    uint64_t allocationCount;              // Number of allocations
    size_t uniqueAllocations;              // Unique allocation sites
    std::vector<AllocationInfo> allocations; // Allocation records
    
    // Memory usage by source file
    std::unordered_map<std::string, size_t> allocationsByFile;
    
    // Memory usage by size
    std::vector<size_t> allocationSizes;   // Distribution of allocation sizes
    
    // Memory efficiency metrics
    double fragmentationRatio;             // 0 = no fragmentation, 1 = complete fragmentation
    double averageAllocationSize;          // Mean allocation size
    
    MemoryProfile() : totalAllocated(0), peakAllocated(0), allocationCount(0),
                     uniqueAllocations(0), fragmentationRatio(0), averageAllocationSize(0) {}
};

class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Memory tracking
    void onAllocation(size_t size, const char* file, int line);
    void onDeallocation(size_t size, const char* file, int line);
    void onEvent(const PerformanceEvent& event);
    
    // Profiling control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return profilingEnabled_; }
    
    // Profile access
    MemoryProfile getCurrentProfile() const;
    MemoryProfile getProfileForTimeRange(uint64_t startTime, uint64_t endTime) const;
    
    // Analysis methods
    std::vector<std::string> findMemoryLeaks() const;
    std::vector<std::pair<std::string, size_t>> getTopAllocationSites(size_t limit = 10) const;
    size_t getAllocationCountForFile(const std::string& file) const;
    size_t getTotalAllocationForFile(const std::string& file) const;
    
    // Memory efficiency analysis
    double calculateFragmentationRatio() const;
    size_t findLargestAllocation() const;
    size_t calculateAllocationHistogram(size_t binCount = 10) const;
    
    // Integration with memory pools
    void onMemoryPoolAllocation(size_t poolId, size_t size);
    void onMemoryPoolDeallocation(size_t poolId, size_t size);
    
    // Export and reporting
    void exportProfile(const std::string& filename) const;
    void generateMemoryReport(const std::string& filename) const;
    void printProfileSummary() const;
    
private:
    bool profilingEnabled_;
    MemoryProfile currentProfile_;
    
    // Memory pool tracking
    std::unordered_map<size_t, size_t> poolAllocations_;
    std::unordered_map<size_t, size_t> poolDeallocations_;
    
    // Allocation tracking
    std::vector<AllocationInfo> allocationHistory_;
    std::mutex profileMutex_;
    
    // Call stack capture
    std::string captureCallStack() const;
    std::vector<std::string> getCallStackStrings() const;
};

} // namespace nlm
```

### 4. Cache Performance Monitor

```cpp
// nlm/performance/CacheMonitor.hpp

#pragma once

#include <chrono>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct CacheStats {
    uint64_t accesses;                     // Total cache accesses
    uint64_t hits;                         // Cache hits
    uint64_t misses;                       // Cache misses
    uint64_t evictions;                     // Cache evictions
    uint64_t prefetches;                   // Prefetch operations
    
    // Level-specific stats
    uint64_t l1Accesses;                  // L1 cache accesses
    uint64_t l1Hits;                        // L1 cache hits
    uint64_t l2Accesses;                    // L2 cache accesses
    uint64_t l2Hits;                        // L2 cache hits
    uint64_t l3Accesses;                    // L3 cache accesses
    uint64_t l3Hits;                        // L3 cache hits
    
    // Performance metrics
    double hitRate() const { return hits > 0 ? static_cast<double>(hits) / accesses : 0.0; }
    double missRate() const { return misses > 0 ? static_cast<double>(misses) / accesses : 0.0; }
    double averageMissLatency() const;      // To be implemented with hardware counters
    
    CacheStats() : accesses(0), hits(0), misses(0), evictions(0), prefetches(0),
                  l1Accesses(0), l1Hits(0), l2Accesses(0), l2Hits(0),
                  l3Accesses(0), l3Hits(0) {}
};

struct CacheLineInfo {
    uint64_t address;                      // Cache line address
    uint32_t cpuId;                        // CPU core that accessed
    uint64_t timestamp;                    // Access time
    uint8_t cacheLevel;                    // 1=L1, 2=L2, 3=L3
    bool write;                            // Read/Write operation
    
    CacheLineInfo(uint64_t addr, uint32_t cpu, uint8_t level, bool isWrite)
        : address(addr), cpuId(cpu), timestamp(getNanoseconds()),
          cacheLevel(level), write(isWrite) {}
    
    static uint64_t getNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

class CacheMonitor {
public:
    CacheMonitor();
    ~CacheMonitor();
    
    // Cache event tracking
    void onCacheAccess(uint64_t address, uint8_t cacheLevel, bool hit, uint32_t cpuId = 0);
    void onCacheMiss(uint64_t address, uint8_t cacheLevel, uint32_t cpuId = 0);
    void onCacheEviction(uint64_t address, uint8_t cacheLevel, uint32_t cpuId = 0);
    void onPrefetch(uint64_t address, uint8_t cacheLevel);
    
    // Hardware performance counters (if available)
    bool initializeHardwareCounters();
    void updateHardwareCounters();
    
    // Cache analysis
    CacheStats getCacheStats(uint8_t level = 0) const;
    std::vector<CacheLineInfo> getRecentAccesses(uint64_t limit = 1000) const;
    std::vector<uint64_t> getHotCacheLines(size_t count = 10) const;
    
    // Performance analysis
    double calculateCacheMissRate(uint8_t level = 0) const;
    uint64_t getLastMissLatency(uint8_t level = 0) const;
    std::string analyzeAccessPattern(const std::vector<CacheLineInfo>& accesses) const;
    
    // Integration with simulation
    void onSimulationStep(uint64_t step, uint32_t cpuId = 0);
    void onMemoryAccess(size_t address, size_t size, uint32_t cpuId = 0);
    
    // Export and reporting
    void exportCacheStats(const std::string& filename) const;
    void generateCacheReport(const std::string& filename) const;
    void printCacheSummary() const;
    
    // Cache optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
private:
    // Cache statistics
    CacheStats globalStats_;
    std::unordered_map<uint8_t, CacheStats> levelStats_;
    
    // Access tracking
    std::vector<CacheLineInfo> accessHistory_;
    mutable std::mutex cacheMutex_;
    
    // Hardware counters
    bool hardwareCountersEnabled_;
    std::vector<uint64_t> hardwareCounterData_;
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point startTime_;
    uint64_t lastMissLatency_[4];          // Store last few miss latencies
    
    // Analysis
    std::unordered_map<uint64_t, uint64_t> accessCount_;  // Address -> access count
    
    // Utility functions
    double calculateReuseDistance(const std::vector<CacheLineInfo>& accesses) const;
    std::string analyzeStridePattern(const std::vector<CacheLineInfo>& accesses) const;
    double estimateCachePressure() const;
};

} // namespace nlm
```

### 5. SIMD Performance Monitor

```cpp
// nlm/performance/SIMDMonitor.hpp

#pragma once

#include <chrono>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>

namespace nlm {

enum class SIMDInstruction {
    SSE2, SSE3, SSE4_1, SSE4_2, SSE4A,
    AVX, AVX2, AVX512F, AVX512DQ,
    FMA, PCLMULQDQ, SSE41, AVX41,
    Unknown
};

struct SIMDStats {
    uint64_t instructions;                 // Total SIMD instructions executed
    uint64_t scalarFallbacks;              // Scalar fallback operations
    double utilization;                    // SIMD utilization rate (0-1)
    double throughput;                     // Instructions per cycle
    
    // Instruction breakdown
    std::unordered_map<SIMDInstruction, uint64_t> instructionCounts;
    
    // Performance metrics
    double efficiency() const { return scalarFallbacks > 0 ? 
        static_cast<double>(instructions) / (instructions + scalarFallbacks) : 1.0; }
    uint64_t totalFallbacks() const { return scalarFallbacks; }
    
    SIMDStats() : instructions(0), scalarFallbacks(0), utilization(0), throughput(0) {}
};

struct SIMDContext {
    SIMDInstruction instruction;           // SIMD instruction type
    uint64_t timestamp;                    // Execution time
    uint32_t cpuId;                        // CPU core
    uint64_t operationCount;               // Operations in this SIMD instruction
    uint64_t memoryAccesses;               // Memory accesses for this operation
    
    SIMDContext(SIMDInstruction instr, uint32_t cpu, uint64_t ops, uint64_t mem)
        : instruction(instr), timestamp(getNanoseconds()), cpuId(cpu),
          operationCount(ops), memoryAccesses(mem) {}
    
    static uint64_t getNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

class SIMDMonitor {
public:
    SIMDMonitor();
    ~SIMDMonitor();
    
    // SIMD instruction tracking
    void onSIMDInstruction(SIMDInstruction instr, uint32_t cpuId, uint64_t operationCount,
                          uint64_t memoryAccesses);
    void onScalarFallback(SIMDInstruction fallbackFor, uint32_t cpuId, uint64_t operationCount);
    
    // Hardware detection
    SIMDInstruction detectAvailableSIMD() const;
    SIMDInstruction getCurrentSIMD() const { return currentSIMD_; }
    bool isSIMDAvailable(SIMDInstruction instr) const;
    
    // Performance analysis
    SIMDStats getSIMDStats() const;
    std::vector<SIMDContext> getRecentInstructions(size_t limit = 1000) const;
    std::string analyzeSIMDUtilization() const;
    
    // Integration with vectorization
    void onVectorizedOperation(size_t elementCount, SIMDInstruction instr);
    void onScalarOperation(size_t elementCount);
    
    // Optimization recommendations
    std::vector<std::string> getSIMDOptimizationRecommendations() const;
    
    // Export and reporting
    void exportSIMDStats(const std::string& filename) const;
    void generateSIMDReport(const std::string& filename) const;
    void printSIMDSummary() const;
    
private:
    // SIMD statistics
    SIMDStats currentStats_;
    SIMDInstruction currentSIMD_;
    
    // Instruction tracking
    std::vector<SIMDContext> instructionHistory_;
    mutable std::mutex simdMutex_;
    
    // Hardware detection
    bool detectSIMDSupport() const;
    SIMDInstruction detectBestAvailableSIMD() const;
    
    // Performance analysis
    double calculateUtilization() const;
    double calculateThroughput() const;
    std::string analyzeInstructionMix() const;
    
    // Utility functions
    SIMDInstruction getSIMDFromCPUInfo() const;
    uint64_t estimateInstructionLatency(SIMDInstruction instr) const;
};

} // namespace nlm
```

### 6. Parallel Processing Monitor

```cpp
// nlm/performance/ParallelMonitor.hpp

#pragma once

#include <chrono>
#include <atomic>
#include <unordered_map>
#include <string>
#include <vector>
#include <thread>

namespace nlm {

struct ThreadStats {
    uint32_t threadId;                     // Thread identifier
    uint64_t activeTime;                   // Time thread was active
    uint64_t idleTime;                     // Time thread was idle
    uint64_t workItems;                    // Number of work items processed
    uint64_t stepCompletions;              // Simulation steps completed
    
    // Performance metrics
    double utilization() const {
        uint64_t total = activeTime + idleTime;
        return total > 0 ? static_cast<double>(activeTime) / total : 0.0;
    }
    double workItemsPerSecond(uint64_t totalTime) const {
        return totalTime > 0 ? static_cast<double>(workItems) / (totalTime / 1e9) : 0.0;
    }
    
    ThreadStats() : threadId(0), activeTime(0), idleTime(0), workItems(0), stepCompletions(0) {}
};

struct ParallelStats {
    // Thread pool statistics
    uint32_t threadCount;                  // Number of worker threads
    uint32_t activeThreadCount;            // Currently active threads
    uint32_t idleThreadCount;               // Currently idle threads
    
    // Workload distribution
    double loadBalance;                    // 1.0 = perfect balance, <1.0 = imbalance
    uint64_t totalWorkItems;               // Total work items processed
    uint64_t totalStepCompletions;         // Total simulation steps completed
    
    // Synchronization metrics
    uint64_t lockAcquisitions;             // Number of lock acquisitions
    uint64_t lockContentionEvents;         // Number of contention events
    double lockWaitTime;                   // Average lock wait time
    
    // Performance metrics
    double parallelEfficiency;            // 0-1, efficiency relative to ideal
    double speedup;                        // Speedup relative to single thread
    uint64_t totalWallTime;                // Total wall clock time
    uint64_t totalCpuTime;                 // Total CPU time
    
    // Throughput
    uint64_t stepsPerSecond() const {
        return totalWallTime > 0 ? (totalStepCompletions * 1e9) / totalWallTime : 0;
    }
    double cpuUtilization() const {
        return totalWallTime > 0 ? static_cast<double>(totalCpuTime) / totalWallTime : 0.0;
    }
    
    ParallelStats() : threadCount(0), activeThreadCount(0), idleThreadCount(0),
                     loadBalance(0), totalWorkItems(0), totalStepCompletions(0),
                     lockAcquisitions(0), lockContentionEvents(0), lockWaitTime(0),
                     parallelEfficiency(0), speedup(0), totalWallTime(0), totalCpuTime(0) {}
};

class ParallelMonitor {
public:
    ParallelMonitor();
    ~ParallelMonitor();
    
    // Thread lifecycle tracking
    void onThreadStart(uint32_t threadId);
    void onThreadEnd(uint32_t threadId);
    void onThreadActive(uint32_t threadId);
    void onThreadIdle(uint32_t threadId);
    
    // Work item tracking
    void onWorkItemStart(uint32_t threadId, const std::string& workType);
    void onWorkItemComplete(uint32_t threadId, const std::string& workType);
    void onStepCompletion(uint32_t threadId, uint64_t step);
    
    // Synchronization tracking
    void onLockAcquire(uint32_t threadId);
    void onLockWait(uint32_t threadId, uint64_t waitTime);
    void onLockRelease(uint32_t threadId);
    
    // Performance analysis
    ThreadStats getThreadStats(uint32_t threadId) const;
    ParallelStats getParallelStats() const;
    double calculateLoadBalance() const;
    double calculateParallelEfficiency(uint32_t threadCount) const;
    
    // Bottleneck detection
    std::string identifyParallelBottlenecks() const;
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Integration with ParallelProcessing
    void onParallelForExecution(uint32_t threadCount, uint64_t workRange);
    void onThreadPoolTaskSubmission(uint32_t threadId, const std::string& taskType);
    
    // Export and reporting
    void exportParallelStats(const std::string& filename) const;
    void generateParallelReport(const std::string& filename) const;
    void printParallelSummary() const;
    
private:
    // Thread statistics
    std::unordered_map<uint32_t, ThreadStats> threadStats_;
    std::mutex threadMutex_;
    
    // Parallel statistics
    ParallelStats globalStats_;
    mutable std::mutex parallelMutex_;
    
    // Work tracking
    std::unordered_map<uint32_t, std::vector<std::pair<uint64_t, std::string>>> threadWork_;
    
    // Time tracking
    std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> threadTimes_;
    
    // Synchronization tracking
    std::unordered_map<uint32_t, uint64_t> lockStats_;
    
    // Performance analysis utilities
    double calculateIdealParallelTime(uint32_t threadCount, uint64_t workSize) const;
    double calculateAmdahlEfficiency(double serialFraction, uint32_t threadCount) const;
    std::string analyzeLoadImbalance() const;
    
    // Thread utility functions
    uint32_t getCurrentThreadId() const;
    std::chrono::nanoseconds getThreadActiveTime(uint32_t threadId) const;
};

} // namespace nlm
```

### 7. Performance Monitor Controller

```cpp
// nlm/performance/PerformanceMonitorController.hpp

#pragma once

#include "MonitoringEvent.hpp"
#include "RealTimePerformanceMonitor.hpp"
#include "MemoryProfiler.hpp"
#include "CacheMonitor.hpp"
#include "SIMDMonitor.hpp"
#include "ParallelMonitor.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class PerformanceMonitorController {
public:
    PerformanceMonitorController(Brain& brain);
    ~PerformanceMonitorController();
    
    // Monitor lifecycle
    void startAllMonitors();
    void stopAllMonitors();
    bool areAllMonitorsRunning() const;
    
    // Individual monitor control
    void startMonitor(const std::string& monitorName);
    void stopMonitor(const std::string& monitorName);
    bool isMonitorRunning(const std::string& monitorName) const;
    
    // Integration with Brain
    void attachToBrain(Brain& brain);
    void integrateWithEventBus(PerformanceEventBus& eventBus);
    
    // Configuration
    void configureRealTimeMonitor(double updateInterval);
    void configureMemoryProfiler(bool trackAllocations, bool trackPools);
    void configureCacheMonitor(bool trackHardwareCounters);
    void configureSIMDMonitor(bool trackInstructionMix);
    void configureParallelMonitor(bool trackThreadUtilization);
    
    // Event handling
    void onEvent(const PerformanceEvent& event);
    void setEventHandler(std::function<void(const PerformanceEvent&)> handler);
    
    // Performance analysis
    RealTimeMetrics getRealTimeMetrics() const;
    MemoryProfile getMemoryProfile() const;
    CacheStats getCacheStats(uint8_t level = 0) const;
    SIMDStats getSIMDStats() const;
    ParallelStats getParallelStats() const;
    
    // System-wide analysis
    std::string identifySystemBottlenecks() const;
    std::vector<std::string> getPerformanceRecommendations() const;
    double calculateOverallPerformanceScore() const;
    
    // Export and reporting
    void exportAllMetrics(const std::string& directory) const;
    void generatePerformanceReport(const std::string& filename) const;
    void generateBottleneckReport(const std::string& filename) const;
    void generateOptimizationReport(const std::string& filename) const;
    
    // Monitoring configuration
    static void initializeDefaultMonitors();
    static void loadMonitorConfiguration(const std::string& filename);
    static void saveMonitorConfiguration(const std::string& filename);
    
private:
    // Individual monitors
    std::unique_ptr<RealTimePerformanceMonitor> realTimeMonitor_;
    std::unique_ptr<MemoryProfiler> memoryProfiler_;
    std::unique_ptr<CacheMonitor> cacheMonitor_;
    std::unique_ptr<SIMDMonitor> simdMonitor_;
    std::unique_ptr<ParallelMonitor> parallelMonitor_;
    
    // Core components
    Brain* brain_;
    PerformanceEventBus* eventBus_;
    
    // Control
    std::unordered_map<std::string, std::unique_ptr<class MonitorBase>> monitors_;
    std::function<void(const PerformanceEvent&)> eventHandler_;
    
    // Performance analysis
    std::vector<std::pair<std::string, double>> calculatePerformanceScores() const;
    std::string analyzeCrossSubsystemBottlenecks() const;
    std::vector<std::string> generateOptimizationPlan() const;
    
    // Export utilities
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    void exportToExcel(const std::string& filename) const;
    void generateHTMLReport(const std::string& filename) const;
};

} // namespace nlm
```

### 8. Integration with NLM

```cpp
// src/monitoring/MonitoringIntegration.cpp

#include "PerformanceMonitorController.hpp"
#include "../performance/PerformanceEventBus.hpp"
#include <memory>

namespace nlm {

class MonitoringIntegration {
public:
    static MonitoringIntegration& getInstance();
    
    // Integration with Brain initialization
    static void setupMonitoringForBrain(Brain& brain);
    static void setupMonitoringForEventBus(PerformanceEventBus& eventBus);
    
    // Global monitoring control
    static void startMonitoring();
    static void stopMonitoring();
    static bool isMonitoringActive();
    
    // Configuration
    static void configureMonitoring(const std::string& configFile);
    static void saveMonitoringConfig(const std::string& configFile);
    
    // Monitoring setup helpers
    static void setupRealTimeMonitoring(double updateInterval = 0.1);
    static void setupMemoryTracking(bool trackAllocations = true);
    static void setupCacheMonitoring(bool hardwareCounters = true);
    static void setupSIMDMonitoring(bool instructionTracking = true);
    static void setupParallelMonitoring(bool threadAnalysis = true);
    
    // Integration hooks
    static void onBrainStep(Brain& brain, uint64_t step);
    static void onMemoryAllocation(size_t size, const char* file, int line);
    static void onMemoryDeallocation(size_t size, const char* file, int line);
    static void onSimulationEvent(const PerformanceEvent& event);
    
    // Results access
    static PerformanceMonitorController& getMonitorController();
    static PerformanceEventBus& getEventBus();
    
private:
    MonitoringIntegration();
    ~MonitoringIntegration();
    
    // Singleton instance
    static MonitoringIntegration* instance_;
    
    // Core components
    std::unique_ptr<PerformanceMonitorController> monitorController_;
    std::unique_ptr<PerformanceEventBus> eventBus_;
    
    // Brain integration
    std::vector<Brain*> monitoredBrains_;
    
    // Configuration
    std::string configFile_;
    bool monitoringEnabled_;
};

} // namespace nlm
```

### 9. Python Bindings

```cpp
// python/monitoring_module.cpp

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include "PerformanceMonitorController.hpp"
#include "PerformanceEventBus.hpp"

namespace py = pybind11;

PYBIND11_MODULE(monitoring, m) {
    py::class_<nlm::PerformanceEvent>(m, "PerformanceEvent")
        .def(py::init<>())
        .def_readwrite("timestamp", &nlm::PerformanceEvent::timestamp)
        .def_readwrite("type", &nlm::PerformanceEvent::type)
        .def_readwrite("threadId", &nlm::PerformanceEvent::threadId)
        .def_readwrite("step", &nlm::PerformanceEvent::step)
        .def_readwrite("simulationTime", &nlm::PerformanceEvent::simulationTime)
        .def_readwrite("data", &nlm::PerformanceEvent::data);
    
    py::class_<nlm::PerformanceEventBus>(m, "PerformanceEventBus")
        .def(py::init<>(), "Create event bus")
        .def("publish", &nlm::PerformanceEventBus::publish)
        .def("subscribe", &nlm::PerformanceEventBus::subscribe)
        .def("get_events", &nlm::PerformanceEventBus::getEvents)
        .def("clear", &nlm::PerformanceEventBus::clear);
    
    py::class_<nlm::RealTimeMetrics>(m, "RealTimeMetrics")
        .def_readonly("currentStep", &nlm::RealTimeMetrics::currentStep)
        .def_readonly("stepsPerSecond", &nlm::RealTimeMetrics::stepsPerSecond)
        .def_readonly("neuronsPerSecond", &nlm::RealTimeMetrics::neuronsPerSecond)
        .def_readonly("memoryUsageMB", &nlm::RealTimeMetrics::memoryUsageMB)
        .def_readonly("cacheHitRate", &nlm::RealTimeMetrics::cacheHitRate)
        .def_readonly("activeThreadCount", &nlm::RealTimeMetrics::activeThreadCount);
    
    py::class_<nlm::PerformanceMonitorController>(m, "PerformanceMonitorController")
        .def(py::init<nlm::Brain&>(), py::arg("brain"))
        .def("start_all_monitors", &nlm::PerformanceMonitorController::startAllMonitors)
        .def("stop_all_monitors", &nlm::PerformanceMonitorController::stopAllMonitors)
        .def("is_monitor_running", &nlm::PerformanceMonitorController::isMonitorRunning)
        .def("get_real_time_metrics", &nlm::PerformanceMonitorController::getRealTimeMetrics)
        .def("get_memory_profile", &nlm::PerformanceMonitorController::getMemoryProfile)
        .def("get_cache_stats", &nlm::PerformanceMonitorController::getCacheStats, py::arg("level") = 0)
        .def("get_simd_stats", &nlm::PerformanceMonitorController::getSIMDStats)
        .def("get_parallel_stats", &nlm::PerformanceMonitorController::getParallelStats)
        .def("identify_bottlenecks", &nlm::PerformanceMonitorController::identifySystemBottlenecks)
        .def("generate_report", &nlm::PerformanceMonitorController::generatePerformanceReport, py::arg("filename"));
    
    // Event type enumeration
    py::enum_<nlm::EventType>(m, "EventType")
        .value("SimulationStep", nlm::EventType::SimulationStep)
        .value("MemoryAllocation", nlm::EventType::MemoryAllocation)
        .value("MemoryDeallocation", nlm::EventType::MemoryDeallocation)
        .value("CacheMiss", nlm::EventType::CacheMiss)
        .value("LockContention", nlm::EventType::LockContention)
        .value("ThreadStart", nlm::EventType::ThreadStart)
        .value("ThreadEnd", nlm::EventType::ThreadEnd)
        .value("SIMDInstruction", nlm::EventType::SIMDInstruction)
        .value("BranchPrediction", nlm::EventType::BranchPrediction)
        .value("PageFault", nlm::EventType::PageFault)
        .value("Custom", nlm::EventType::Custom)
        .export_values();
    
    m.doc() = "Real-time performance monitoring for NLM neural simulation";
}
```

### 10. Command Line Interface

```cpp
// src/cli/MonitoringCommand.cpp

#include "PerformanceMonitorController.hpp"
#include <iostream>
#include <fstream>

namespace nlm {

class MonitoringCommand {
public:
    static void execute(int argc, char* argv[]);
    static void printHelp();
    static PerformanceMonitorController createMonitorController(int argc, char* argv[]);
    
private:
    static void parseArguments(int argc, char* argv[], PerformanceMonitorController& controller);
    static void startMonitoring(PerformanceMonitorController& controller);
    static void stopMonitoring(PerformanceMonitorController& controller);
    static void exportMetrics(PerformanceMonitorController& controller, const std::string& outputFile);
    static void generateReport(PerformanceMonitorController& controller, const std::string& reportFile);
};

} // namespace nlm
```

### 11. Example Usage

```python
# python/example_monitoring.py

import nlm.monitoring
import time
import threading

def main():
    # Create a brain (simplified example)
    print("NLM Performance Monitoring Example")
    print("=" * 50)
    
    # Note: This is a simplified example
    # In practice, you would create a Brain instance and integrate monitoring
    
    # Create event bus and monitor controller
    event_bus = nlm.monitoring.PerformanceEventBus()
    
    # Create monitor controller (requires Brain instance)
    # monitor_controller = nlm.monitoring.PerformanceMonitorController(brain)
    
    # Configure monitoring
    # monitor_controller.configureRealTimeMonitor(updateInterval=0.1)  # 100ms updates
    # monitor_controller.configureMemoryProfiler(track_allocations=True)
    # monitor_controller.configureCacheMonitor(hardware_counters=True)
    # monitor_controller.configureSIMDMonitor(instruction_tracking=True)
    # monitor_controller.configureParallelMonitor(thread_analysis=True)
    
    # Attach to event bus
    # event_bus.set_event_handler(monitor_controller.onEvent)
    
    # Start all monitors
    # monitor_controller.start_all_monitors()
    
    # Run simulation (simplified)
    print("Monitoring started...")
    print("Real-time metrics available:")
    
    # Get current metrics (simplified)
    # metrics = monitor_controller.get_real_time_metrics()
    # print(f"  Current step: {metrics.currentStep}")
    # print(f"  Steps per second: {metrics.stepsPerSecond:.2f}")
    # print(f"  Memory usage: {metrics.memoryUsageMB:.2f} MB")
    # print(f"  Cache hit rate: {metrics.cacheHitRate:.3f}")
    
    # Wait for user input or simulation to complete
    try:
        input("Press Enter to stop monitoring...")
    except:
        pass
    
    # Stop monitoring
    # monitor_controller.stop_all_monitors()
    
    # Export metrics
    # output_file = "nlm_performance_metrics.json"
    # monitor_controller.export_all_metrics("results/")
    # print(f"Metrics exported to {output_file}")
    
    # Generate report
    # report_file = "nlm_performance_report.html"
    # monitor_controller.generate_performance_report(report_file)
    # print(f"Report generated to {report_file}")
    
    print("\nMonitoring complete!")

if __name__ == "__main__":
    main()
```

### 12. Configuration Files

#### monitoring_config.json

```json
{
    "monitoring": {
        "enabled": true,
        "event_bus_capacity": 1000000,
        "real_time_interval": 0.1,
        "export_format": "json",
        "generate_reports": true
    },
    
    "monitors": {
        "real_time": {
            "enabled": true,
            "update_interval": 0.1,
            "thresholds": {
                "max_memory_usage_mb": 2048,
                "min_steps_per_second": 1000,
                "max_cache_miss_rate": 0.1,
                "max_cpu_usage_percent": 90
            },
            "alerts": {
                "enable_memory_alerts": true,
                "enable_performance_alerts": true,
                "enable_resource_alerts": true
            }
        },
        
        "memory_profiler": {
            "enabled": true,
            "track_allocations": true,
            "track_deallocations": true,
            "track_memory_pools": true,
            "detect_leaks": true,
            "fragmentation_analysis": true
        },
        
        "cache_monitor": {
            "enabled": true,
            "track_hardware_counters": true,
            "monitor_l1": true,
            "monitor_l2": true,
            "monitor_l3": true,
            "detect_cache_bloat": true
        },
        
        "simd_monitor": {
            "enabled": true,
            "track_instruction_mix": true,
            "detect_scalar_fallbacks": true,
            "analyze_utilization": true
        },
        
        "parallel_monitor": {
            "enabled": true,
            "track_thread_utilization": true,
            "analyze_load_balance": true,
            "detect_lock_contention": true
        }
    },
    
    "alerts": {
        "enable_console_output": true,
        "enable_file_output": true,
        "alert_log_file": "monitoring_alerts.log",
        "email_notifications": false,
        "slack_webhook": null
    },
    
    "export": {
        "output_directory": "./monitoring_results",
        "format": "json",
        "include_raw_events": true,
        "compress_output": true
    }
}
```

#### monitoring_runner.py

```python
#!/usr/bin/env python3
"""
NLM Performance Monitoring Runner
"""

import sys
import os
import time
import json
import argparse
from pathlib import Path

# Add the monitoring module to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import nlm.monitoring as monitoring

def main():
    parser = argparse.ArgumentParser(description="NLM Performance Monitoring")
    parser.add_argument("--config", help="Path to monitoring configuration file", 
                       default="monitoring_config.json")
    parser.add_argument("--output", help="Output directory for monitoring results")
    parser.add_argument("--real-time", action="store_true", help="Enable real-time monitoring")
    parser.add_argument("--events", action="store_true", help="Record events")
    parser.add_argument("--duration", type=int, default=300, help="Monitoring duration in seconds")
    parser.add_argument("--help", action="store_true", help="Show help information")
    
    args = parser.parse_args()
    
    if args.help:
        print("NLM Performance Monitoring Runner")
        print("===============================")
        print(""")
        print("This tool provides real-time and offline performance monitoring")
        print("for the NLM neural simulation framework.")
        print(""")
        print("Usage examples:")
        print("  nlm_monitoring --config monitoring_config.json --duration 600")
        print("  nlm_monitoring --real-time --events --output ./results")
        print(""")
        print("Monitoring features:")
        print("  - Real-time performance metrics")
        print("  - Memory allocation tracking")
        print("  - Cache performance analysis")
        print("  - SIMD instruction monitoring")
        print("  - Parallel processing analysis")
        print("  - Bottleneck identification")
        print("  - Performance optimization recommendations")
        return
    
    print("NLM Performance Monitoring Runner")
    print("=" * 50)
    
    # Load configuration
    print(f"Loading configuration from {args.config}")
    # Note: In practice, you would load the configuration
    
    # Create monitoring components
    print("Initializing monitoring components...")
    
    # Create event bus
    event_bus = monitoring.PerformanceEventBus()
    
    # Create monitor controller (requires Brain instance)
    # brain = create_brain_from_config()
    # monitor_controller = monitoring.PerformanceMonitorController(brain)
    # event_bus.set_event_handler(monitor_controller.onEvent)
    
    # Configure monitoring based on command line arguments
    if args.real_time:
        print("Enabling real-time monitoring...")
        # monitor_controller.configureRealTimeMonitor(updateInterval=0.1)
    
    if args.events:
        print("Enabling event recording...")
        # monitor_controller.start_all_monitors()
    
    # Start monitoring
    print(f"Starting monitoring for {args.duration} seconds...")
    start_time = time.time()
    
    # Main monitoring loop
    while time.time() - start_time < args.duration:
        # In practice, you would run the simulation and collect metrics
        time.sleep(1)
        
        # Get and display current metrics
        # metrics = monitor_controller.get_real_time_metrics()
        # print(f"Step: {metrics.currentStep}, ", end="")
        # print(f"SPS: {metrics.stepsPerSecond:.1f}, ", end="")
        # print(f"Mem: {metrics.memoryUsageMB:.1f}MB, ", end="")
        # print(f"Cache: {metrics.cacheHitRate:.3f}, ", end="")
        # print(f"Threads: {metrics.activeThreadCount}")
        
        # Check for performance issues
        # bottlenecks = monitor_controller.identify_bottlenecks()
        # if bottlenecks:
        #     print(f"  BOTTLENECKS: {bottlenecks}")
    
    # Stop monitoring
    print("\nStopping monitoring...")
    # monitor_controller.stop_all_monitors()
    
    # Export results
    if args.output:
        print(f"Exporting results to {args.output}...")
        # monitor_controller.export_all_metrics(args.output)
        # monitor_controller.generate_performance_report(f"{args.output}/report.html")
    
    # Print summary
    print("\nMonitoring complete!")
    print("Results have been saved to the specified output directory.")
    
    # Display performance analysis
    # monitor_controller = get_monitor_controller_instance()
    # recommendations = monitor_controller.get_performance_recommendations()
    # print("\nPerformance Optimization Recommendations:")
    # for i, rec in enumerate(recommendations, 1):
    #     print(f"  {i}. {rec}")

if __name__ == "__main__":
    main()
```

## Performance Optimization Integration

The monitoring system integrates with NLM's performance infrastructure:

1. **Event-driven Architecture**: Uses PerformanceEventBus for efficient event distribution
2. **Real-time Metrics**: RealTimePerformanceMonitor provides live performance data
3. **Memory Analysis**: MemoryProfiler tracks allocation patterns and efficiency
4. **Cache Optimization**: CacheMonitor analyzes cache performance and provides tuning recommendations
5. **SIMD Efficiency**: SIMDMonitor tracks vectorization effectiveness
6. **Parallel Analysis**: ParallelMonitor analyzes thread utilization and load balancing

## Usage Instructions

### Building and Running

```bash
# Build with monitoring support
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON -DBUILD_MONITORING=ON ..
make -j$(nproc)

# Or build Python bindings with monitoring
python -m pip install -e .
```

### Running Monitoring

```bash
# Run from command line
./nlm monitoring --config monitoring_config.json --duration 600

# Real-time monitoring with event recording
./nlm monitoring --real-time --events --output ./results

# Help information
./nlm monitoring --help

# Python bindings
python monitoring_runner.py --duration 300 --output ./monitoring_results
```

### Analyzing Results

The monitoring system generates:

1. **Real-time console output**: Live performance metrics
2. **JSON export**: Machine-readable performance data
3. **HTML reports**: Visual performance analysis
4. **CSV export**: Spreadsheet-compatible data
5. **Optimization recommendations**: Actionable improvement suggestions

### Performance Target Guidelines

#### Real-time Metrics
- **Steps per second**: Target based on scale (Micro: 10k+, Small: 5k+, Medium: 1k+, Large: 100+)
- **Memory usage**: Target based on scale (Micro: <10MB, Small: <100MB, Medium: <1GB, Large: <10GB)
- **Cache hit rate**: > 90% for optimal performance
- **CPU utilization**: Balanced between threads and efficiency

#### Memory Efficiency
- **Allocation overhead**: Minimize dynamic allocations
- **Fragmentation**: Keep < 20% for optimal memory usage
- **Pool utilization**: Ensure memory pools are effectively utilized
- **Cache locality**: Optimize for spatial and temporal locality

#### SIMD Utilization
- **Instruction mix**: > 80% SIMD instructions for compute-bound workloads
- **Scalar fallbacks**: < 20% fallback operations
- **Throughput**: Maximize SIMD operations per cycle

#### Parallel Efficiency
- **Load balance**: > 80% thread utilization
- **Lock contention**: Minimize lock wait times
- **Speedup**: Near-linear scaling up to available cores
- **CPU efficiency**: Balance between parallelism and overhead

## Bottleneck Detection and Analysis

The monitoring system automatically detects and reports:

1. **Memory bottlenecks**: High allocation counts, poor cache locality
2. **Computation bottlenecks**: Low SIMD utilization, serial operations
3. **Concurrency bottlenecks**: Poor load balancing, lock contention
4. **Memory bottlenecks**: High memory usage, inefficient allocations
5. **Hardware limitations**: Cache misses, branch mispredictions

## Integration with Benchmark Suite

The monitoring system complements the benchmark suite by:

1. **Live profiling**: Monitor performance during benchmark runs
2. **Bottleneck identification**: Identify performance constraints
3. **Optimization validation**: Verify performance improvements
4. **Continuous monitoring**: Track performance across runs
5. **Regression detection**: Identify performance regressions

## Future Enhancements

1. **Hardware performance counters**: Native OS/hardware metrics
2. **GPU monitoring**: CUDA/GPU performance metrics
3. **Distributed monitoring**: Multi-machine performance analysis
4. **Machine learning**: Predictive performance analysis
5. **Automated tuning**: Automatic performance optimization
6. **Cloud monitoring**: Cloud resource utilization tracking

## Summary

The NLM Performance Monitoring System provides comprehensive performance analysis capabilities:

1. **Real-time monitoring**: Live performance metrics and alerts
2. **Memory analysis**: Allocation tracking and efficiency metrics
3. **Cache optimization**: Cache performance analysis and tuning recommendations
4. **SIMD monitoring**: Vectorization effectiveness tracking
5. **Parallel analysis**: Thread utilization and load balancing analysis
6. **Integration**: Seamless integration with existing NLM architecture
7. **Export**: Multiple export formats for analysis tools
8. **Automation**: Automated bottleneck detection and optimization recommendations

This system enables NLM developers to identify performance bottlenecks, optimize system performance, and maintain high-performance operation across different scales and configurations.
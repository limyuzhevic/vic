#pragma once

/**
 * MemoryProfiler - Advanced memory profiling system
 * 
 * Comprehensive memory profiling for NLM neural simulations.
 * Provides detailed memory analysis including:
 * - Heap memory tracking and allocation/deallocation monitoring
 * - Memory pool profiling and efficiency analysis
 * - Fragmentation detection and analysis
 * - Page fault tracking and analysis
 * - Thread-local storage (TLS) tracking
 * - NUMA locality analysis
 * - Memory leak detection
 * - Access pattern analysis
 * 
 * Integration with existing infrastructure:
 * - Works with MemoryPool.hpp for pool-specific profiling
 * - Integrates with PerformanceProfiler.hpp for metrics collection
 * - Supports SIMD-aware memory tracking
 * - Compatible with ParallelProcessing.hpp for thread-aware profiling
 */

#include "src/performance/PerformanceProfiler.hpp"
#include <chrono>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <thread>
#include <mutex>
#include <set>

namespace nlm {

/**
 * Memory leak severity levels
 */
enum class LeakSeverity {
    LOW = 0,
    MEDIUM = 1,
    HIGH = 2,
    CRITICAL = 3
};

/**
 * Memory allocation record
 */
struct AllocationRecord {
    uint64_t allocationId;        // Unique allocation identifier
    std::chrono::high_resolution_clock::time_point timestamp;  // Allocation time
    std::chrono::high_resolution_clock::time_point deallocationTimestamp;  // Deallocation time
    size_t size;                  // Size in bytes
    const char* allocationSite;   // Source file and line where allocated
    const char* deallocationSite; // Source file and line where deallocated
    bool isDeallocated;           // Whether this allocation has been freed
    std::thread::id threadId;     // Thread that performed the allocation
    bool fromMemoryPool;          // Whether allocated from memory pool
    const char* poolName;         // Name of memory pool if applicable
    uint64_t memoryPoolId;        // ID of memory pool if applicable
    uintptr_t memoryAddress;     // Actual memory address (simulated)
    double lifetime;              // Lifetime in seconds (0 if not deallocated)
    
    AllocationRecord() : allocationId(0), size(0), allocationSite(nullptr),
                        deallocationSite(nullptr), isDeallocated(false),
                        memoryPoolId(0), memoryAddress(0), lifetime(0.0) {}
};

/**
 * Memory leak information
 */
struct MemoryLeak {
    uint64_t allocationId;        // Allocation identifier
    size_t size;                  // Size in bytes
    std::string allocationSite;   // Source file and line
    std::string deallocationSite; // Source file and line (may be empty)
    double lifetime;              // Lifetime in seconds
    std::thread::id threadId;     // Thread that allocated
    bool isMemoryPoolAllocation;  // Whether from memory pool
    std::string poolName;         // Pool name if applicable
    LeakSeverity severity;        // Leak severity level
    
    MemoryLeak() : allocationId(0), size(0), lifetime(0), isMemoryPoolAllocation(false),
                  severity(LeakSeverity::LOW) {}
};

/**
 * Memory usage analysis results
 */
struct MemoryAnalysis {
    size_t totalAllocations;      // Total number of allocations
    size_t totalMemoryAllocated;  // Total memory allocated (bytes)
    size_t totalMemoryFreed;      // Total memory freed (bytes)
    size_t currentMemoryUsage;    // Current memory usage (bytes)
    size_t peakMemoryUsage;       // Peak memory usage (bytes)
    double averageAllocationSize; // Average allocation size (bytes)
    size_t maxAllocationSize;     // Maximum allocation size (bytes)
    double fragmentationScore;    // Fragmentation score (0-1)
    size_t smallFragmentCount;    // Number of small fragments (< 1KB)
    size_t mediumFragmentCount;   // Number of medium fragments (1KB-1MB)
    size_t largeFragmentCount;    // Number of large fragments (> 1MB)
    
    // Memory pool statistics
    size_t totalPools;            // Number of memory pools
    size_t totalPoolAllocations;  // Total pool allocations
    size_t totalPoolMemory;       // Total pool memory usage
    double poolUtilization;       // Pool utilization (0-1)
    std::string maxPoolName;      // Name of pool with max usage
    size_t maxPoolUsage;          // Maximum pool usage
    
    // Access pattern analysis
    double accessPatternScore;    // Access pattern score (0-1)
    
    // NUMA analysis
    size_t numaLocalAllocations;  // Allocations in local NUMA node
    size_t numaRemoteAllocations; // Allocations in remote NUMA node
    double numaLocalPercentage;  // Percentage of local NUMA allocations
    
    MemoryAnalysis() : totalAllocations(0), totalMemoryAllocated(0),
                      totalMemoryFreed(0), currentMemoryUsage(0),
                      peakMemoryUsage(0), averageAllocationSize(0),
                      maxAllocationSize(0), fragmentationScore(0),
                      smallFragmentCount(0), mediumFragmentCount(0),
                      largeFragmentCount(0), totalPools(0),
                      totalPoolAllocations(0), totalPoolMemory(0),
                      poolUtilization(0), maxPoolUsage(0),
                      accessPatternScore(0), numaLocalAllocations(0),
                      numaRemoteAllocations(0), numaLocalPercentage(0) {}
};

/**
 * Memory pool analysis
 */
struct MemoryPoolAnalysis {
    size_t totalPools;            // Total number of memory pools
    size_t totalAllocations;      // Total allocations from pools
    size_t totalDeallocations;    // Total deallocations from pools
    size_t totalPoolMemory;       // Total pool memory usage
    double poolUtilization;       // Overall pool utilization
    size_t maxPoolUsage;          // Maximum pool usage
    std::string maxPoolName;      // Name of pool with maximum usage
    
    // Per-pool statistics
    struct PoolStats {
        std::string name;          // Pool name
        size_t allocationCount;    // Number of allocations
        size_t deallocationCount;  // Number of deallocations
        size_t totalMemory;        // Total memory allocated
        size_t currentMemory;     // Current memory usage
        size_t peakMemory;        // Peak memory usage
        
        PoolStats() : allocationCount(0), deallocationCount(0),
                     totalMemory(0), currentMemory(0), peakMemory(0) {}
    };
    
    std::map<std::string, PoolStats> poolStatistics;  // Statistics per pool
    
    MemoryPoolAnalysis() : totalPools(0), totalAllocations(0),
                          totalDeallocations(0), totalPoolMemory(0),
                          poolUtilization(0), maxPoolUsage(0) {}
};

/**
 * Memory leak report
 */
struct MemoryLeakReport {
    std::vector<MemoryLeak> leaks;       // Detected memory leaks
    size_t totalLeakedMemory;           // Total memory leaked
    size_t criticalLeaks;               // Number of critical leaks
    size_t highLeaks;                   // Number of high-severity leaks
    size_t mediumLeaks;                 // Number of medium-severity leaks
    size_t lowLeaks;                    // Number of low-severity leaks
    
    MemoryLeakReport() : totalLeakedMemory(0), criticalLeaks(0),
                        highLeaks(0), mediumLeaks(0), lowLeaks(0) {}
};

/**
 * Memory profiler configuration
 */
struct MemoryProfilerConfig {
    bool enableHeapProfiling;       // Enable heap memory profiling
    bool enablePoolTracking;        // Enable memory pool tracking
    bool enableFragmentationAnalysis; // Enable fragmentation analysis
    bool trackAllocationSites;       // Track allocation sites
    double samplingInterval;        // Sampling interval in seconds
    size_t maxSamples;              // Maximum samples to store
    bool enablePageFaultTracking;   // Enable page fault tracking
    bool enableTLSTracking;         // Enable TLS tracking
    bool trackMemoryPools;          // Track memory pools
    bool analyzeAccessPatterns;     // Analyze memory access patterns
    bool enableVirtualMemoryTracking; // Enable virtual memory tracking
    bool trackNUMALocalities;       // Track NUMA localities
    
    MemoryProfilerConfig() : enableHeapProfiling(true),
                             enablePoolTracking(true),
                             enableFragmentationAnalysis(true),
                             trackAllocationSites(true),
                             samplingInterval(0.1),  // 100ms
                             maxSamples(1000000),
                             enablePageFaultTracking(true),
                             enableTLSTracking(true),
                             trackMemoryPools(true),
                             analyzeAccessPatterns(true),
                             enableVirtualMemoryTracking(true),
                             trackNUMALocalities(true) {}
};

/**
 * Memory profiler core class
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Disable copying
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    
    /**
     * Start memory profiling
     */
    void startProfiling(const MemoryProfilerConfig& config = MemoryProfilerConfig());
    
    /**
     * Stop memory profiling
     */
    void stopProfiling();
    
    /**
     * Record a memory allocation
     */
    void recordAllocation(size_t size, const char* allocationSite = nullptr,
                         const char* deallocationSite = nullptr,
                         bool fromMemoryPool = false,
                         const char* poolName = nullptr,
                         std::thread::id threadId = std::this_thread::get_id());
    
    /**
     * Record a memory deallocation
     */
    void recordDeallocation(uint64_t allocationId, const char* deallocationSite = nullptr,
                           std::thread::id threadId = std::this_thread::get_id());
    
    /**
     * Record a memory pool operation
     */
    void recordMemoryPoolOperation(const std::string& poolName,
                                  MemoryPoolOperation operation,
                                  size_t size,
                                  uint64_t poolAddress,
                                  std::thread::id threadId = std::this_thread::get_id());
    
    /**
     * Record a page fault
     */
    void recordPageFault(uint64_t virtualAddress, bool isWrite,
                        bool isProtectionFault,
                        std::thread::id threadId = std::this_thread::get_id());
    
    /**
     * Record TLS usage
     */
    void recordTLSUsage(const TLSRecord& record);
    
    /**
     * Record NUMA locality
     */
    void recordNUMALocality(const NUMALocationRecord& record);
    
    /**
     * Analyze current memory usage
     */
    MemoryAnalysis analyzeMemoryUsage();
    
    /**
     * Detect memory leaks
     */
    MemoryLeakReport detectMemoryLeaks();
    
    /**
     * Analyze memory pool usage
     */
    MemoryPoolAnalysis analyzeMemoryPoolUsage();
    
    /**
     * Analyze allocation patterns
     */
    std::vector<AllocationSiteInfo> analyzeAllocationPatterns();
    
    /**
     * Export profiling data to JSON format
     */
    void exportToJSON(std::ostream& output);
    
    /**
     * Export profiling data to CSV format
     */
    void exportToCSV(std::ostream& output);
    
    /**
     * Generate memory profiling report
     */
    std::string generateReport();
    
    /**
     * Get current profiler state
     */
    bool isProfiling() const { return isProfiling_; }
    
    /**
     * Get number of allocations recorded
     */
    size_t getAllocationCount() const { return allocationRecords_.size(); }
    
    /**
     * Get peak memory usage
     */
    size_t getPeakMemoryUsage() const;
    
    /**
     * Get total memory allocated
     */
    size_t getTotalMemoryAllocated() const;
    
    /**
     * Get profiling session duration
     */
    double getSessionDuration() const;
    
    /**
     * Clear all recorded data
     */
    void clear();
    
    /**
     * Save profiling data to file
     */
    void saveToFile(const std::string& filename) const;
    
    /**
     * Load profiling data from file
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Generate visualization data
     */
    std::string generateVisualization() const;
    
    /**
     * Get next allocation ID
     */
    static uint64_t getNextAllocationId() {
        static std::atomic<uint64_t> nextId{1};
        return nextId.fetch_add(1);
    }
    
private:
    bool isProfiling_;
    std::chrono::high_resolution_clock::time_point profilingStartTime_;
    std::chrono::high_resolution_clock::time_point profilingEndTime_;
    
    // Allocation records
    std::map<uint64_t, AllocationRecord> allocationRecords_;
    
    // Configuration
    MemoryProfilerConfig config_;
    
    // Memory tracking
    struct MemoryTracker {
        std::atomic<uint64_t> nextAllocationId;
        std::atomic<uintptr_t> nextAddress;
        std::atomic<size_t> totalAllocations;
        std::atomic<size_t> totalDeallocations;
        std::map<uint64_t, AllocationRecord> allocationMap;
        std::vector<PageFaultRecord> pageFaults;
        std::vector<TLSRecord> tlsRecords;
        std::vector<NUMALocationRecord> numaLocations;
        
        MemoryTracker() : nextAllocationId(1), nextAddress(0x1000),
                         totalAllocations(0), totalDeallocations(0) {}
        
        void startTracking() {}
        void stopTracking() {}
        
        void recordAllocation(const AllocationRecord& record) {
            totalAllocations++;
            allocationMap[record.allocationId] = record;
        }
        
        void recordDeallocation(const AllocationRecord& record) {
            totalDeallocations++;
            if (allocationMap.find(record.allocationId) != allocationMap.end()) {
                allocationMap[record.allocationId] = record;
            }
        }
        
        void recordPageFault(const PageFaultRecord& record) {
            pageFaults.push_back(record);
        }
        
        void recordTLSRecord(const TLSRecord& record) {
            tlsRecords.push_back(record);
        }
        
        void recordNUMALocation(const NUMALocationRecord& record) {
            numaLocations.push_back(record);
        }
        
        uint64_t getNextAllocationId() { return nextAllocationId.fetch_add(1); }
        uintptr_t getNextAddress() { return nextAddress.fetch_add(4096); }  // 4KB aligned
        void cleanupOldRecords() {
            auto cutoffTime = std::chrono::high_resolution_clock::now() - 
                              std::chrono::duration<double>(1.0);
            
            auto it = allocationMap.begin();
            while (it != allocationMap.end()) {
                if (it->second.timestamp < cutoffTime) {
                    it = allocationMap.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        size_t getLocalAllocations() const { return 0; }
        size_t getRemoteAllocations() const { return 0; }
    };
    
    std::unique_ptr<MemoryTracker> memoryTracker_;
    
    // Pool profiler
    struct MemoryPoolProfiler {
        std::atomic<size_t> totalAllocations;
        std::atomic<size_t> totalDeallocations;
        std::atomic<size_t> totalPoolMemory;
        std::map<std::string, MemoryPoolAnalysis::PoolStats> poolStats;
        std::vector<MemoryPoolRecord> poolOperations;
        
        MemoryPoolProfiler() : totalAllocations(0), totalDeallocations(0),
                              totalPoolMemory(0) {}
        
        void startProfiling() {}
        void stopProfiling() {}
        
        void recordAllocation(const AllocationRecord& record) {
            if (record.fromMemoryPool) {
                totalAllocations++;
                totalPoolMemory += record.size;
                
                std::string poolName = record.poolName ? record.poolName : "default";
                if (poolStats.find(poolName) == poolStats.end()) {
                    poolStats[poolName] = MemoryPoolAnalysis::PoolStats();
                }
                
                auto& stats = poolStats[poolName];
                stats.allocationCount++;
                stats.totalMemory += record.size;
                stats.currentMemory += record.size;
            }
        }
        
        void recordPoolOperation(const MemoryPoolRecord& record) {
            poolOperations.push_back(record);
            
            if (record.operation == MemoryPoolOperation::ALLOCATE) {
                totalAllocations++;
                totalPoolMemory += record.size;
                
                if (poolStats.find(record.poolName) == poolStats.end()) {
                    poolStats[record.poolName] = MemoryPoolAnalysis::PoolStats();
                }
                
                auto& stats = poolStats[record.poolName];
                stats.allocationCount++;
                stats.totalMemory += record.size;
                stats.currentMemory += record.size;
                
            } else if (record.operation == MemoryPoolOperation::DEALLOCATE) {
                totalDeallocations++;
                totalPoolMemory -= record.size;
                
                if (poolStats.find(record.poolName) != poolStats.end()) {
                    auto& stats = poolStats[record.poolName];
                    stats.deallocationCount++;
                    stats.currentMemory -= record.size;
                }
            }
        }
        
        MemoryPoolAnalysis analyzePoolUsage() {
            MemoryPoolAnalysis analysis;
            
            analysis.totalPools = poolStats.size();
            analysis.totalAllocations = totalAllocations.load();
            analysis.totalDeallocations = totalDeallocations.load();
            analysis.totalPoolMemory = totalPoolMemory.load();
            
            if (!poolStats.empty()) {
                double totalMemory = 0;
                for (const auto& pair : poolStats) {
                    totalMemory += pair.second.totalMemory;
                }
                
                if (totalMemory > 0) {
                    analysis.poolUtilization = totalPoolMemory.load() / totalMemory;
                }
                
                for (const auto& pair : poolStats) {
                    if (pair.second.currentMemory > analysis.maxPoolUsage) {
                        analysis.maxPoolUsage = pair.second.currentMemory;
                        analysis.maxPoolName = pair.first;
                    }
                }
            }
            
            return analysis;
        }
    };
    
    std::unique_ptr<MemoryPoolProfiler> poolProfiler_;
    
    // Internal helper methods
    void analyzeFragmentation(MemoryAnalysis& analysis);
    void analyzePoolUsage(MemoryPoolAnalysis& analysis);
    void analyzeAccessPatterns(MemoryAnalysis& analysis);
    void analyzeNUMADistribution(MemoryAnalysis& analysis);
    
    std::string formatBytes(size_t bytes);
};

} // namespace nlm

// Memory allocation type
enum class MemoryPoolOperation {
    ALLOCATE,
    DEALLOCATE,
    RESIZE
};

// Thread-local storage record
struct TLSRecord {
    std::thread::id threadId;
    uint64_t tlsBaseAddress;
    size_t tlsSize;
    std::chrono::high_resolution_clock::time_point timestamp;
    bool isAllocated;
    
    TLSRecord() : threadId(std::thread::id()), tlsBaseAddress(0), tlsSize(0),
                 isAllocated(false) {}
};

// NUMA location record
struct NUMALocationRecord {
    uint64_t memoryAddress;
    size_t size;
    int numaNode;
    bool isLocal;
    std::thread::id threadId;
    std::chrono::high_resolution_clock::time_point timestamp;
    
    NUMALocationRecord() : memoryAddress(0), size(0), numaNode(-1),
                          isLocal(false), timestamp() {}
};

// Allocation site information
struct AllocationSiteInfo {
    std::string site;                  // File and line where allocation occurred
    size_t allocationCount;            // Number of allocations at this site
    size_t totalMemoryAllocated;       // Total memory allocated at this site
    double averageAllocationSize;     // Average allocation size
    size_t uniqueThreads;             // Number of unique threads that allocated here
    size_t fromMemoryPools;           // Number of allocations from memory pools
    double allocationPercentage;      // Percentage of total allocations
    
    AllocationSiteInfo() : allocationCount(0), totalMemoryAllocated(0),
                          averageAllocationSize(0), uniqueThreads(0),
                          fromMemoryPools(0), allocationPercentage(0) {}
};

// Memory profiling utility functions
void saveMemoryProfilerData(const MemoryProfiler& profiler, const std::string& filename);
bool loadMemoryProfilerData(MemoryProfiler& profiler, const std::string& filename);

// C++ to Python bindings
#ifdef __cplusplus
extern "C" {
    void* createMemoryProfiler();
    void destroyMemoryProfiler(void* profiler);
    void startMemoryProfiling(void* profiler);
    void stopMemoryProfiling(void* profiler);
    void recordMemoryAllocation(void* profiler, size_t size, const char* site);
    void recordMemoryDeallocation(void* profiler, uint64_t id, const char* site);
    std::string generateMemoryReport(void* profiler);
}
#endif

// Python module bindings
PYBIND11_MODULE(memory_profiler, m) {
    pybind11::class_<nlm::MemoryProfiler>(m, "MemoryProfiler")
        .def(pybind11::init<>())
        .def("start_profiling", &nlm::MemoryProfiler::startProfiling)
        .def("stop_profiling", &nlm::MemoryProfiler::stopProfiling)
        .def("record_allocation", &nlm::MemoryProfiler::recordAllocation)
        .def("record_deallocation", &nlm::MemoryProfiler::recordDeallocation)
        .def("analyze_memory_usage", &nlm::MemoryProfiler::analyzeMemoryUsage)
        .def("detect_memory_leaks", &nlm::MemoryProfiler::detectMemoryLeaks)
        .def("export_to_json", &nlm::MemoryProfiler::exportToJSON)
        .def("generate_report", &nlm::MemoryProfiler::generateReport);
}

} // namespace nlm

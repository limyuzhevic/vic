#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <deque>
#include <iomanip>
#include <sstream>

namespace nlm {

// Memory block for memory pool
struct MemoryBlock {
    void* address;
    size_t size;
    Timestamp allocatedTime;
    bool isFreed;
    std::string allocationId;
    std::string fileName;
    int lineNumber;
    std::string functionName;
};

// Memory pool statistics
struct PoolStats {
    size_t allocatedBlocks;
    size_t totalBlocks;
    size_t fragmentationLevel;
    size_t allocationCount;
    size_t deallocationCount;
    double utilization;
    double fragmentationRatio;
    size_t totalAllocatedSize;
    size_t peakAllocatedSize;
    Timestamp lastAllocation;
    Timestamp lastDeallocation;
};

// Memory allocation request
struct AllocationRequest {
    size_t size;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    Timestamp timestamp;
    std::string allocationId;
    std::string typeName;
    size_t alignment;
};

// Memory pool - Memory pool management
class MemoryPoolManager {
public:
    MemoryPoolManager();
    ~MemoryPoolManager();
    
    // Pool management
    void createPool(const std::string& poolName, size_t blockSize, size_t initialCapacity = 0);
    bool destroyPool(const std::string& poolName);
    bool poolExists(const std::string& poolName) const;
    std::vector<std::string> getPoolNames() const;
    
    // Allocation/deallocation
    void* allocate(const std::string& poolName, size_t size, 
                   const std::string& sourceFile = "", 
                   int sourceLine = 0, 
                   const std::string& functionName = "");
    void deallocate(const std::string& poolName, void* address);
    bool deallocate(const std::string& poolName, const std::string& allocationId);
    
    // Memory tracking
    void trackAllocation(const std::string& poolName, const AllocationRequest& request);
    void trackDeallocation(const std::string& poolName, const std::string& allocationId);
    
    // Pool statistics
    PoolStats getPoolStats(const std::string& poolName) const;
    std::map<std::string, PoolStats> getAllPoolStats() const;
    
    // Memory monitoring
    size_t getPoolMemoryUsage(const std::string& poolName) const;
    size_t getPoolFreeMemory(const std::string& poolName) const;
    size_t getPoolFragmentation(const std::string& poolName) const;
    
    // Allocation analysis
    std::vector<AllocationRequest> getAllocationHistory(const std::string& poolName, 
                                                         size_t limit = 100) const;
    std::vector<MemoryBlock> getAllocatedBlocks(const std::string& poolName) const;
    std::vector<MemoryBlock> getFreeBlocks(const std::string& poolName) const;
    
    // Fragmentation analysis
    double calculateFragmentationRatio(const std::string& poolName) const;
    std::vector<size_t> getFreeBlockSizes(const std::string& poolName) const;
    size_t findBestFit(size_t size, const std::vector<size_t>& freeBlocks) const;
    
    // Pool optimization
    void optimizePool(const std::string& poolName);
    void resizePool(const std::string& poolName, size_t newCapacity);
    void consolidatePool(const std::string& poolName);
    
    // Performance optimization
    void setAllocationThreshold(size_t threshold);
    void setFragmentationThreshold(double threshold);
    void enableFragmentationRecovery(bool enable);
    void setPoolGrowthStrategy(const std::string& strategy); // "linear", "exponential", "adaptive"
    
    // Memory efficiency
    double getPoolEfficiency(const std::string& poolName) const;
    size_t getTotalMemoryUsage() const;
    size_t getTotalFragmentation() const;
    std::map<std::string, double> getPoolEfficiencies() const;
    
    // Pool maintenance
    void cleanupPool(const std::string& poolName);
    void emergencyCleanup();
    void resetPool(const std::string& poolName);
    
    // Reporting and analysis
    std::string generatePoolReport(const std::string& poolName) const;
    std::string generateAllPoolsReport() const;
    std::string generateEfficiencyReport() const;
    std::string generateFragmentationReport() const;
    
    // Export/import
    void exportPoolData(const std::string& poolName, const std::string& filepath, 
                       const std::string& format = "json") const;
    bool importPoolData(const std::string& poolName, const std::string& filepath,
                       const std::string& format = "json");
    
    // Control
    void enable(bool enable);
    bool isEnabled() const;
    void setVerboseLogging(bool enable);
    bool isVerboseLogging() const;
    
    // Maintenance
    void collectGarbage();
    void compactPools();
    void rebalancePools();
    void updatePoolMetrics();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
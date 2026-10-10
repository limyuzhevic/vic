#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <sstream>

namespace nlm {

// Memory allocation record for tracking
struct AllocationRecord {
    size_t size;
    std::string source;
    Timestamp timestamp;
    std::string allocationId;
};

// Memory deallocation record for tracking
struct DeallocationRecord {
    size_t size;
    std::string source;
    Timestamp timestamp;
    std::string deallocationId;
    std::string allocationId; // Link to allocation for leak detection
};

// Memory monitoring statistics
struct MemoryStats {
    size_t totalAllocated;
    size_t totalDeallocated;
    size_t currentUsage;
    size_t peakUsage;
    double allocationRate;
    double deallocationRate;
    std::map<std::string, size_t> allocationsBySource;
};

// MemoryMonitor - Memory monitoring class
class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    // Start/stop tracking
    void startTracking();
    void stopTracking();
    bool isTracking() const;
    
    // Memory tracking
    void trackAllocation(size_t size, const std::string& source);
    void trackDeallocation(size_t size, const std::string& source);
    void trackAllocation(size_t size, const std::string& source, const std::string& allocationId);
    void trackDeallocation(size_t size, const std::string& source, const std::string& allocationId);
    
    // Memory statistics
    size_t getTotalMemoryUsage() const;
    size_t getAllocatedMemory() const;
    size_t getDeallocatedMemory() const;
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    
    // Memory pool information
    void recordPoolUsage(const std::string& poolName, size_t size, size_t capacity);
    std::map<std::string, std::pair<size_t, size_t>> getPoolStats() const;
    
    // Leak detection
    std::vector<std::string> detectMemoryLeaks() const;
    bool hasMemoryLeaks() const;
    
    // Statistics and analysis
    MemoryStats getMemoryStats() const;
    std::vector<AllocationRecord> getAllocationHistory(const std::string& source = "") const;
    std::vector<DeallocationRecord> getDeallocationHistory(const std::string& source = "") const;
    
    // Garbage collection analysis
    void recordGcEvent(size_t freedMemory, size_t gcTime, const std::string& gcType = "generational");
    std::vector<std::pair<size_t, size_t>> getGcStats() const; // (freed_memory, gc_time)
    
    // Performance metrics
    double getAllocationRate() const;
    double getDeallocationRate() const;
    std::map<std::string, double> getAllocationRatesBySource() const;
    
    // Memory efficiency analysis
    double getMemoryEfficiency() const;
    size_t getTotalAllocationCount() const;
    size_t getTotalDeallocationCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
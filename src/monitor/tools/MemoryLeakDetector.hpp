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
#include <algorithm>

namespace nlm {

// Memory allocation info for leak detection
struct AllocationInfo {
    size_t size;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    Timestamp timestamp;
    std::string allocationId;
    std::string stackTrace;
    std::string typeName;
};

// Memory deallocation info for leak detection
struct DeallocationInfo {
    size_t size;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    Timestamp timestamp;
    std::string deallocationId;
    std::string allocationId; // Link to allocation
    std::string typeName;
};

// Memory leak detection results
struct LeakResult {
    std::string allocationId;
    size_t leakedSize;
    std::string sourceInfo;
    Timestamp firstAllocation;
    Timestamp lastAllocation;
    std::string leakType; // "direct", "indirect", "circular"
    double confidence; // 0.0 to 1.0
};

// Memory block tracking for heap analysis
struct MemoryBlock {
    void* address;
    size_t size;
    Timestamp allocatedTime;
    std::string allocatorName;
    std::string fileName;
    int lineNumber;
    std::string stackTrace;
    bool isFreed;
};

// Memory pool information
struct PoolInfo {
    std::string poolName;
    size_t blockSize;
    size_t allocatedBlocks;
    size_t totalBlocks;
    size_t fragmentationLevel;
    size_t allocationCount;
    size_t deallocationCount;
    double utilization;
};

// MemoryLeakDetector - Memory leak detection
class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Start/stop leak detection
    void startDetection();
    void stopDetection();
    bool isDetecting() const;
    
    // Allocation tracking
    void trackAllocation(size_t size, const std::string& sourceFile = "", 
                        int sourceLine = 0, const std::string& functionName = "");
    void trackDeallocation(const std::string& allocationId);
    void trackAllocation(const std::string& typeName, size_t size,
                        const std::string& sourceFile = "", int sourceLine = 0,
                        const std::string& functionName = "");
    
    // Leak detection
    std::vector<LeakResult> detectMemoryLeaks() const;
    std::vector<LeakResult> detectMemoryLeaksByType(const std::string& typeName) const;
    bool hasMemoryLeaks() const;
    size_t getTotalLeakedMemory() const;
    double getLeakDetectionConfidence() const;
    
    // Heap analysis
    std::vector<MemoryBlock> getMemoryBlocks() const;
    std::vector<MemoryBlock> getAllocatedMemoryBlocks() const;
    std::vector<MemoryBlock> getUnfreedMemoryBlocks() const;
    std::map<std::string, size_t> getAllocationCountsByType() const;
    
    // Circular reference detection
    std::vector<std::string> detectCircularReferences() const;
    bool hasCircularReferences() const;
    
    // Memory pool integration
    void setPoolInfo(const std::string& poolName, const PoolInfo& info);
    std::map<std::string, PoolInfo> getPoolInfo() const;
    std::vector<LeakResult> detectPoolLeaks() const;
    
    // Detailed leak analysis
    LeakResult getLeakDetails(const std::string& allocationId) const;
    std::string generateLeakReport() const;
    std::string generateDetailedLeakReport() const;
    
    // Leak tracking
    void clearLeakData();
    void exportLeakData(const std::string& filepath, const std::string& format = "json") const;
    
    // Performance considerations
    double getDetectionOverhead() const;
    void enableStackTrace(bool enable);
    void setAllocationThreshold(size_t threshold);
    void setTimeBasedLeakDetection(bool enable, Timestamp maxAge = 3600.0);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
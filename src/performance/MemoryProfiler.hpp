#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <unordered_set>

namespace nlm {

// MemoryProfiler: Memory usage tracking and leak detection
class MemoryProfiler {
public:
    MemoryProfiler();
    explicit MemoryProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~MemoryProfiler();
    
    // Configuration
    void setMemoryCheckInterval(size_t interval) { memoryCheckInterval_ = interval; }
    void setDetectMemoryLeaks(bool enable) { detectMemoryLeaks_ = enable; }
    void setLeakThreshold(size_t threshold) { leakThreshold_ = threshold; }
    void setMaxSnapshots(size_t max) { maxSnapshots_ = max; }
    void setEnableDetailedTracking(bool enable) { enableDetailedTracking_ = enable; }
    void setTrackAllocationStack(bool enable) { trackAllocationStack_ = enable; }
    
    // Memory tracking
    void recordAllocation(size_t size, const std::string& context = "",
                         const std::string& site = "",
                         const std::vector<std::string>& callStack = {});
    void recordDeallocation(size_t size, const std::string& context = "",
                          const std::string& site = "",
                          const std::vector<std::string>& callStack = {});
    
    // Snapshot management
    void takeSnapshot();
    std::vector<MemorySnapshot> getSnapshots() const { return snapshots_; }
    size_t getSnapshotCount() const { return snapshots_.size(); }
    
    // Leak detection
    std::vector<MemoryLeakInfo> detectMemoryLeaks() const;
    std::string getLeakReport() const;
    std::string getDetailedLeakReport() const;
    
    // Memory analysis
    size_t getTotalAllocated() const { return totalAllocated_; }
    size_t getTotalFreed() const { return totalFreed_; }
    size_t getCurrentUsage() const { return currentUsage_; }
    
    // Context analysis
    std::unordered_map<std::string, size_t> getAllocationStats() const;
    std::unordered_map<std::string, size_t> getDeallocationStats() const;
    
    // Allocation pattern analysis
    std::string analyzeAllocationPatterns() const;
    std::string analyzeAllocationTrends() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { 
        debuggingSystem_ = debuggingSystem; 
    }
    
    // Export
    bool exportMemoryProfile(const std::string& filepath) const;
    std::string exportMemoryProfileAsString() const;
    
    // Leak tracking
    void addLeakedAllocation(const MemoryAllocation& allocation);
    void removeLeakedAllocation(const MemoryAllocation& allocation);
    
    // Statistics
    size_t getLeakedAllocationCount() const { return currentLeaks_.size(); }
    size_t getAllocationCount() const { return allocations_.size(); }
    size_t getDeallocationCount() const { return deallocations_.size(); }
    
    // Real-time monitoring
    std::string getRealTimeMemoryStats() const;
    bool isMemoryUsageHigh() const;
    
private:
    struct MemoryAllocation {
        std::chrono::system_clock::time_point timestamp;
        size_t size;
        std::string context;
        std::string allocationSite;
        uint64_t threadId;
        std::vector<std::string> callStack;
        
        MemoryAllocation(size_t size, const std::string& context, const std::string& site,
                         const std::vector<std::string>& stack = {})
            : size(size), context(context), allocationSite(site),
              threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
              callStack(stack) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    struct MemoryDeallocation {
        std::chrono::system_clock::time_point timestamp;
        size_t size;
        std::string context;
        std::string deallocationSite;
        uint64_t threadId;
        std::vector<std::string> callStack;
        
        MemoryDeallocation(size_t size, const std::string& context, const std::string& site,
                          const std::vector<std::string>& stack = {})
            : size(size), context(context), deallocationSite(site),
              threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
              callStack(stack) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    struct MemorySnapshot {
        std::chrono::system_clock::time_point timestamp;
        size_t totalAllocated;
        size_t totalFreed;
        size_t currentUsage;
        std::unordered_map<std::string, size_t> allocationsByContext;
        std::unordered_map<std::string, size_t> deallocationsByContext;
        std::unordered_map<std::string, size_t> leakedByContext;
        
        MemorySnapshot() : totalAllocated(0), totalFreed(0), currentUsage(0) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    struct MemoryLeakInfo {
        std::string context;
        size_t leakedSize;
        size_t allocationCount;
        std::vector<MemoryAllocation> allocations;
        std::string suspectedSite;
        std::chrono::system_clock::time_point firstAllocation;
        uint64_t threadId;
        
        MemoryLeakInfo(const std::string& ctx, size_t size, size_t count,
                      const std::vector<MemoryAllocation>& allocs,
                      const std::string& site)
            : context(ctx), leakedSize(size), allocationCount(count),
              allocations(allocs), suspectedSite(site) {
            if (!allocs.empty()) {
                firstAllocation = allocs.front().timestamp;
                threadId = allocs.front().threadId;
            }
        }
    };
    
    std::vector<MemoryAllocation> allocations_;
    std::vector<MemoryDeallocation> deallocations_;
    std::vector<MemorySnapshot> snapshots_;
    std::vector<MemoryLeakInfo> currentLeaks_;
    
    mutable std::mutex allocationsMutex_;
    mutable std::mutex deallocationsMutex_;
    mutable std::mutex snapshotsMutex_;
    mutable std::mutex leaksMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    
    size_t totalAllocated_;
    size_t totalFreed_;
    size_t currentUsage_;
    
    size_t memoryCheckInterval_;
    bool detectMemoryLeaks_;
    size_t leakThreshold_;
    size_t maxSnapshots_;
    bool enableDetailedTracking_;
    bool trackAllocationStack_;
    
    // Helper methods
    void updateStats();
    bool isMemoryLeak(const MemoryAllocation& alloc) const;
    std::string getAllocationSite(const std::string& context) const;
    double calculateLeakScore(const MemoryLeakInfo& leak) const;
    void cleanupOldAllocations();
    void cleanupOldDeallocations();
    std::vector<MemoryAllocation> findMatchingDeallocation(
        const MemoryAllocation& alloc) const;
    size_t calculateFragmentation() const;
    std::string analyzeAllocationSources() const;
    std::string analyzeDeallocationSources() const;
};

} // namespace nlm

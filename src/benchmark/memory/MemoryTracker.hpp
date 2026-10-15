#pragma once

/**
 * Memory Tracking System for NLM Benchmarking
 * 
 * Comprehensive memory tracking and analysis system for monitoring
 * memory usage, allocation patterns, and detecting memory leaks.
 */

#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

/**
 * Memory allocation record
 */
struct MemoryAllocation {
    size_t size;                    // Size in bytes
    std::string allocator;          // Allocator name/type
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string stackTrace;         // Simplified stack trace
    
    MemoryAllocation(size_t size, const std::string& allocator)
        : size(size), allocator(allocator), timestamp(std::chrono::high_resolution_clock::now()) {}
};

/**
 * Memory deallocation record
 */
struct MemoryDeallocation {
    size_t size;                    // Size in bytes
    std::string allocator;          // Allocator name/type
    std::chrono::high_resolution_clock::time_point timestamp;
    bool validDeallocation;         // Whether this matches an allocation
    
    MemoryDeallocation(size_t size, const std::string& allocator)
        : size(size), allocator(allocator), timestamp(std::chrono::high_resolution_clock::now()),
          validDeallocation(false) {}
};

/**
 * Memory block tracking
 */
struct MemoryBlock {
    uint64_t address;               // Memory address
    size_t size;                    // Size in bytes
    std::string allocator;          // Allocator name/type
    std::string allocationSite;      // File and line where allocated
    std::chrono::high_resolution_clock::time_point timestamp; // Creation time
    bool isFree;                    // Whether block is free
    
    MemoryBlock(uint64_t addr, size_t size, const std::string& alloc, const std::string& site)
        : address(addr), size(size), allocator(alloc), allocationSite(site),
          timestamp(std::chrono::high_resolution_clock::now()), isFree(false) {}
};

/**
 * Memory tracking statistics
 */
struct MemoryStats {
    size_t totalAllocated;          // Total bytes allocated
    size_t totalDeallocated;        // Total bytes deallocated
    size_t currentUsage;            // Current memory usage
    size_t peakUsage;               // Peak memory usage
    size_t allocationCount;         // Number of allocations
    size_t deallocationCount;       // Number of deallocations
    double allocationRate;          // Allocations per second
    double deallocationRate;        // Deallocations per second
    double fragmentationScore;      // Memory fragmentation score (0-1)
    size_t averageAllocationSize;   // Average allocation size
    size_t largestAllocation;       // Largest allocation
    
    MemoryStats()
        : totalAllocated(0), totalDeallocated(0), currentUsage(0), peakUsage(0),
          allocationCount(0), deallocationCount(0), allocationRate(0.0), deallocationRate(0.0),
          fragmentationScore(0.0), averageAllocationSize(0), largestAllocation(0) {}
};

/**
 * Memory leak detector
 */
class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Leak detection
    bool detectLeaks(double threshold = 0.01);  // threshold = leaked/total
    std::vector<MemoryBlock> getLeakedBlocks() const { return leakedBlocks_; }
    size_t getTotalLeakedMemory() const { return totalLeakedMemory_; }
    double getLeakPercentage() const { return leakPercentage_; }
    
    // Leak analysis
    std::string generateLeakReport() const;
    std::string getLeakAnalysis() const;
    
    // Configuration
    void setLeakThreshold(double threshold) { leakThreshold_ = threshold; }
    void setMinLeakSize(size_t minSize) { minLeakSize_ = minSize; }
    
private:
    double leakThreshold_;
    size_t minLeakSize_;
    std::vector<MemoryBlock> leakedBlocks_;
    size_t totalLeakedMemory_;
    double leakPercentage_;
    
    void scanForLeaks();
    bool isLikelyLeak(const MemoryBlock& block) const;
};

/**
 * Fragmentation analyzer
 */
class FragmentationAnalyzer {
public:
    FragmentationAnalyzer();
    ~FragmentationAnalyzer();
    
    // Fragmentation calculation
    double calculateFragmentation(const std::vector<MemoryBlock>& blocks);
    double calculateExternalFragmentation(const std::vector<MemoryBlock>& blocks) const;
    double calculateInternalFragmentation(const std::vector<MemoryBlock>& blocks) const;
    
    // Fragmentation metrics
    size_t getFreeBlocks() const { return freeBlocks_; }
    size_t getTotalFreeMemory() const { return totalFreeMemory_; }
    double getCoalescenceRatio() const { return coalescenceRatio_; }
    double getUtilizationEfficiency() const { return utilizationEfficiency_; }
    
    // Analysis
    std::string getFragmentationReport() const;
    std::string getCoalescenceAnalysis() const;
    
    // Configuration
    void setFragmentationThreshold(double threshold) { fragmentationThreshold_ = threshold; }
    
private:
    double fragmentationThreshold_;
    size_t freeBlocks_;
    size_t totalFreeMemory_;
    double coalescenceRatio_;
    double utilizationEfficiency_;
    
    void analyzeFragmentation(const std::vector<MemoryBlock>& blocks);
    double calculateCoalescence(const std::vector<MemoryBlock>& blocks) const;
    double calculateUtilization(const std::vector<MemoryBlock>& blocks) const;
};

/**
 * Memory Tracker - Main memory tracking interface
 */
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Memory pool tracking
    void trackMemoryPool(const MemoryPool<NeuronId>* pool, const std::string& name);
    void trackMemoryPool(const MemoryPool<SynapseId>* pool, const std::string& name);
    
    // Memory allocation tracking
    void trackAllocation(size_t size, const std::string& allocator);
    void trackDeallocation(size_t size, const std::string& allocator);
    
    // Memory block tracking (for leak detection)
    void trackBlockAllocation(uint64_t address, size_t size, 
                             const std::string& allocator, const std::string& site);
    void trackBlockDeallocation(uint64_t address);
    
    // Event queue tracking
    void trackEventQueue(const SpikeRingBuffer& queue);
    void trackDelayedQueue(const DelayedSpikeQueue& queue);
    
    // Update and statistics
    void update();
    void reset();
    
    // Memory stats
    MemoryStats getMemoryStats() const;
    size_t getTotalMemoryUsage() const { return stats_.currentUsage; }
    size_t getPeakMemoryUsage() const { return stats_.peakUsage; }
    
    // Leak detection
    bool detectMemoryLeak(double threshold = 0.01);
    std::vector<MemoryBlock> getLeakedBlocks() const { return leakDetector_.getLeakedBlocks(); }
    size_t getTotalLeakedMemory() const { return leakDetector_.getTotalLeakedMemory(); }
    std::string getLeakReport() const { return leakDetector_.generateLeakReport(); }
    
    // Fragmentation analysis
    double analyzeFragmentation() const { return fragmentationAnalyzer_.calculateFragmentation(blocks_); }
    std::string getFragmentationReport() const { return fragmentationAnalyzer_.getFragmentationReport(); }
    
    // Detailed tracking
    std::vector<MemoryBlock> getTrackedBlocks() const { return blocks_; }
    std::vector<MemoryAllocation> getAllocations() const { return allocations_; }
    std::vector<MemoryDeallocation> getDeallocations() const { return deallocations_; }
    
    // Pool stats
    std::vector<std::pair<std::string, std::pair<size_t, size_t>>> getPoolStats() const;
    
    // Event queue stats
    std::pair<size_t, size_t> getEventQueueStats() const; // {current, capacity}
    
    // Reports
    std::string getMemoryUsageReport() const;
    std::string getAllocationReport() const;
    std::string getDeallocationReport() const;
    std::string getCompleteMemoryReport() const;
    
    // Configuration
    void setLeakDetectionEnabled(bool enabled) { leakDetectionEnabled_ = enabled; }
    void setFragmentationAnalysisEnabled(bool enabled) { fragmentationAnalysisEnabled_ = enabled; }
    void setTrackingInterval(double interval) { trackingInterval_ = interval; }
    
private:
    bool leakDetectionEnabled_;
    bool fragmentationAnalysisEnabled_;
    double trackingInterval_;
    double lastUpdateTime_;
    
    mutable std::mutex trackerMutex_;
    
    // Statistics
    MemoryStats stats_;
    
    // Tracking data
    std::vector<MemoryAllocation> allocations_;
    std::vector<MemoryDeallocation> deallocations_;
    std::vector<MemoryBlock> blocks_;
    
    // Pool tracking
    std::vector<std::pair<std::string, std::pair<const MemoryPool<NeuronId>*, const MemoryPool<SynapseId>*>>> poolTracks_;
    
    // Event queue tracking
    std::vector<std::pair<std::string, std::pair<const SpikeRingBuffer*, const DelayedSpikeQueue*>>> queueTracks_;
    
    // Analyzers
    MemoryLeakDetector leakDetector_;
    FragmentationAnalyzer fragmentationAnalyzer_;
    
    // Update methods
    void updateStats();
    void cleanup();
    void analyzeLeaks();
    void analyzeFragmentation();
    
    // Memory management
    void freeBlock(uint64_t address);
    MemoryBlock* findBlock(uint64_t address);
    bool isAddressTracked(uint64_t address) const;
}; // class MemoryTracker

} // namespace nlm
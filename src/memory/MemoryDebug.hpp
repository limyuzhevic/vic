#pragma once

/**
 * NLM Memory Debugging Infrastructure
 * 
 * Comprehensive memory debugging utilities for the NLM neural simulation framework.
 * Provides detailed memory inspection, corruption detection, and debugging tools
 * for identifying and resolving memory issues in neural simulations.
 * 
 * Features:
 * - Memory debugging with stack traces
 * - Allocation size and pattern tracking
 * - Memory block lifecycle monitoring
 * - Cross-platform memory debugging support
 * - Memory state inspection and analysis
 * - Memory corruption detection
 * - Integration with existing NLM memory systems
 * - Zero-overhead operation when disabled
 * 
 * Integration:
 * - Works with MemoryTracker for comprehensive tracking
 * - Integrates with Brain.cpp for simulation debugging
 * - Provides hooks for EventQueue and ParallelProcessing
 * - Compatible with existing MemoryPool infrastructure
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <stack>
#include <set>

// Configuration
#define NLM_MEMORY_DEBUG_ENABLED 1

// Memory corruption detection
enum class MemoryCorruptionType {
    BufferOverflow,
    BufferUnderflow,
    DoubleFree,
    UseAfterFree,
    MemoryLeak,
    InvalidPointer,
    StackCorruption,
    HeapCorruption,
    AlignmentError,
    PoolCorruption
};

struct MemoryCorruption {
    MemoryCorruptionType type;         // Type of corruption detected
    size_t address;                      // Address of corruption
    size_t size;                         // Size of corrupted block
    std::string corruptionSite;          // File and line where corruption detected
    std::string stackTrace;             // Stack trace at corruption point
    uint32_t threadId;                   // Thread that caused corruption
    size_t detectionTime;                // Time when detected
    bool fatal;                         // Whether corruption is fatal
    std::string details;                // Additional details
};

struct MemoryBlockDebug {
    size_t address;                      // Block address
    size_t size;                         // Block size
    bool isValid;                        // Whether block is valid
    bool isAllocated;                    // Whether block is allocated
    std::string allocationSite;          // Original allocation site
    std::string deallocationSite;        // Deallocation site (if freed)
    uint32_t allocationThreadId;         // Thread that allocated
    uint32_t deallocationThreadId;       // Thread that deallocated
    size_t allocationTime;               // Time of allocation
    size_t deallocationTime;             // Time of deallocation
    std::string pattern;                 // Memory pattern for debugging
    std::vector<size_t> accessHistory;   // History of memory accesses
    bool isCorrupted;                    // Whether block is corrupted
    MemoryCorruption corruption;         // Corruption details (if any)
};

struct AllocationPattern {
    std::vector<size_t> allocationSizes;  // Sequence of allocation sizes
    std::vector<size_t> deallocationSizes; // Sequence of deallocation sizes
    std::map<size_t, size_t> sizeDistribution; // Size -> count mapping
    std::map<size_t, double> averageAccessPattern; // Size -> average access frequency
};

struct MemoryDebugConfig {
    bool enableStackTrace;               // Whether to capture stack traces
    bool enablePatternTracking;          // Whether to track allocation patterns
    bool enableCorruptionDetection;      // Whether to detect memory corruption
    bool enableAccessTracking;           // Whether to track memory access patterns
    bool enableSizeTracking;             // Whether to track allocation/deallocation sizes
    double corruptionDetectionThreshold; // Sensitivity for corruption detection
    size_t maxAccessHistorySize;         // Maximum number of access records to keep
    std::string debugOutputFile;         // File for debug output
};

class MemoryDebug {
public:
    MemoryDebug();
    ~MemoryDebug();
    
    // Singleton pattern
    static MemoryDebug& getInstance();
    
    // Configuration and control
    void enable(bool enable = true);
    bool isEnabled() const { return enabled_; }
    void setConfig(const MemoryDebugConfig& config);
    MemoryDebugConfig getConfig() const { return config_; }
    
    // Memory block debugging
    void debugBlock(size_t address, size_t size, const std::string& site);
    void trackAllocation(size_t address, size_t size, const std::string& site,
                        uint32_t threadId = 0);
    void trackDeallocation(size_t address, const std::string& site,
                          uint32_t threadId = 0);
    MemoryBlockDebug getBlockDebugInfo(size_t address) const;
    bool isBlockCorrupted(size_t address) const;
    
    // Corruption detection
    std::vector<MemoryCorruption> detectCorruption(size_t address, size_t size) const;
    std::vector<MemoryCorruption> getDetectedCorruptions() const { return detectedCorruptions_; }
    
    // Pattern tracking
    void recordAccess(size_t address, size_t size, uint32_t threadId);
    AllocationPattern getAllocationPattern(size_t address) const;
    std::map<size_t, AllocationPattern> getAllAllocationPatterns() const;
    
    // Memory analysis
    std::vector<size_t> findUnfreedBlocks() const;
    std::vector<size_t> findDoubleFrees() const;
    std::vector<size_t> findBufferOverflows() const;
    std::vector<size_t> findUseAfterFree() const;
    
    // Integration with NLM systems
    void integrateWithBrain(size_t brainAddress, size_t brainSize);
    void integrateWithMemoryPool(size_t poolAddress, size_t poolSize, const std::string& poolName);
    void integrateWithEventQueue(size_t queueAddress, size_t queueSize);
    void integrateWithParallelProcessing(size_t procAddress, size_t procSize);
    
    // Simulation integration
    void startStep(size_t step, double time);
    void endStep(size_t step, double time);
    
    // Reporting and analysis
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    std::vector<std::string> getDebugRecommendations() const;
    
    // Memory state inspection
    std::string inspectMemoryState() const;
    std::string analyzeMemoryHealth() const;
    std::vector<size_t> findMemoryLeaks() const;
    
    // Cleanup
    void flush();
    void reset();
    
private:
    struct AccessRecord {
        size_t timestamp;                   // Access timestamp
        size_t address;                      // Address accessed
        size_t size;                         // Size accessed
        uint32_t threadId;                   // Thread that accessed
        bool isWrite;                        // Whether it was a write access
        bool isCorrupting;                   // Whether access is corrupting
        
        AccessRecord(size_t ts, size_t addr, size_t sz, uint32_t tid, bool write)
            : timestamp(ts), address(addr), size(sz), threadId(tid), 
              isWrite(write), isCorrupting(false) {}
    };
    
    // Core debugging state
    bool enabled_;
    bool initialized_;
    MemoryDebugConfig config_;
    
    // Integration pointers
    size_t brainAddress_;
    size_t brainSize_;
    std::string currentPoolName_;
    size_t poolAddress_;
    size_t poolSize_;
    
    // Debugging data structures
    std::unordered_map<size_t, MemoryBlockDebug> blockDebugInfo_;
    std::vector<AccessRecord> accessHistory_;
    std::vector<MemoryCorruption> detectedCorruptions_;
    std::unordered_map<size_t, AllocationPattern> allocationPatterns_;
    
    // Memory pools and regions
    std::set<size_t> memoryPoolAddresses_;
    std::set<size_t> brainAddresses_;
    std::set<size_t> eventQueueAddresses_;
    std::set<size_t> parallelProcAddresses_;
    
    // Statistics
    size_t totalAllocations_;
    size_t totalDeallocations_;
    size_t totalAccesses_;
    size_t corruptionDetections_;
    
    // Simulation tracking
    size_t currentStep_;
    double currentTime_;
    
    // Thread safety
    mutable std::shared_mutex dataMutex_;
    std::mutex corruptionDetectionMutex_;
    
    // Utility functions
    bool isAddressInMemoryPool(size_t address) const;
    bool isAddressInBrain(size_t address) const;
    bool isAddressInEventQueue(size_t address) const;
    bool isAddressInParallelProcessing(size_t address) const;
    void checkForCorruption(size_t address, size_t size);
    std::string formatAccessRecord(const AccessRecord& access) const;
    std::string formatCorruption(const MemoryCorruption& corruption) const;
    
    // Pattern analysis
    void updateAllocationPattern(size_t address, size_t size, bool isAllocation);
    AllocationPattern analyzePattern(size_t address) const;
    
    // Corruption detection algorithms
    void detectBufferOverflow(size_t address, size_t size);
    void detectBufferUnderflow(size_t address, size_t size);
    void detectDoubleFree(size_t address);
    void detectUseAfterFree(size_t address);
    void detectMemoryLeak(size_t address);
    
    // Memory health analysis
    std::vector<size_t> analyzeMemoryLeaks() const;
    std::vector<size_t> analyzeMemoryLeaksBySize() const;
    std::vector<size_t> analyzeFragmentation() const;
};
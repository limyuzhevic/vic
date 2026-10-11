#pragma once

#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <set>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <stacktrace>
#include <execinfo.h>
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace nlm {

struct MemoryAllocation {
    std::chrono::system_clock::time_point timestamp;
    size_t address;
    size_t size;
    std::string type;
    std::string allocationSite;
    std::string callStack;
    bool isLeaked;
    
    MemoryAllocation(size_t addr, size_t sz, const std::string& typeStr, const std::string& site)
        : timestamp(std::chrono::system_clock::now()), address(addr), size(sz),
          type(typeStr), allocationSite(site), isLeaked(true) {
        callStack = captureStackTrace();
    }
};

struct MemoryLeak {
    std::string leakType;
    size_t leakSize;
    std::string leakSite;
    std::string description;
    double leakRate;
    std::chrono::system_clock::time_point firstOccurrence;
    std::chrono::system_clock::time_point lastOccurrence;
    std::vector<std::string> affectedStackTraces;
};

class MemoryLeakDetector {
public:
    enum class LeakDetectionMode {
        DETAILED,
        SUMMARY,
        CALLSTACK_ONLY,
        SIZE_ONLY
    };
    
    enum class AllocationHookType {
        MALLOC,
        FREE,
        NEW,
        DELETE,
        NEW_ARRAY,
        DELETE_ARRAY,
        MMAP,
        MUNMAP
    };
    
    struct LeakDetectionConfig {
        LeakDetectionMode mode;
        size_t thresholdSize;
        double thresholdRate;
        int maxStackTraceDepth;
        bool trackAllocations;
        bool trackDeallocations;
        bool detectGrowthPatterns;
        bool enableDetailedReporting;
        
        LeakDetectionConfig() : mode(LeakDetectionMode::DETAILED), thresholdSize(1024), // 1KB
                               thresholdRate(0.01), maxStackTraceDepth(10), trackAllocations(true),
                               trackDeallocations(true), detectGrowthPatterns(true), enableDetailedReporting(true) {}
    };
    
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    static MemoryLeakDetector& getInstance() {
        static MemoryLeakDetector instance;
        return instance;
    }
    
    // Configuration
    void setConfig(const LeakDetectionConfig& config) { config_ = config; }
    const LeakDetectionConfig& getConfig() const { return config_; }
    
    // Hook management
    void installHooks();
    void removeHooks();
    bool areHooksInstalled() const { return hooksInstalled; }
    
    // Memory tracking
    void trackAllocation(size_t address, size_t size, const std::string& type, 
                        const std::string& site = "");
    void trackDeallocation(size_t address, const std::string& type, 
                          const std::string& site = "");
    
    // Leak detection
    std::vector<MemoryLeak> detectLeaks() const;
    bool hasLeaks() const;
    size_t getTotalLeakSize() const;
    
    // Leak analysis
    std::vector<MemoryLeak> analyzeLeaksByType() const;
    std::vector<MemoryLeak> analyzeLeaksBySite() const;
    std::vector<MemoryLeak> analyzeLeaksByStackTrace() const;
    std::vector<MemoryLeak> analyzeLeaksByGrowth() const;
    
    // Leak tracking
    void startTracking(const std::string& sessionName);
    void stopTracking();
    bool isTracking() const { return tracking; }
    
    // Reporting
    std::string generateLeakReport() const;
    std::string generateLeakSummary() const;
    std::string generateDetailedLeakReport() const;
    std::string generateCallStackReport() const;
    
    // Export/Import
    void exportLeaks(const std::string& filename) const;
    void importLeaks(const std::string& filename);
    
    // Memory analysis utilities
    std::vector<size_t> findLeakPoints() const;
    double calculateLeakRate() const;
    std::string analyzeMemoryPattern() const;
    
    // Integration with test framework
    void setupForTest(const std::string& testName);
    void cleanupAfterTest(const std::string& testName);
    bool verifyNoLeaksSinceSetup(const std::string& testName);
    
    // Memory pressure and stress testing
    void simulateMemoryPressure(size_t pressureBytes, const std::string& description);
    void simulateMemoryFragmentation(size_t fragmentSize, int fragmentCount);
    void simulateAllocationDeallocationPattern(int iterations, size_t patternSize);
    
    // Advanced detection
    std::vector<MemoryLeak> detectHeapLeaks() const;
    std::vector<MemoryLeak> detectStackLeaks() const;
    std::vector<MemoryLeak> detectGlobalLeaks() const;
    
    // Memory corruption detection
    bool detectMemoryCorruption() const;
    std::string checkMemoryIntegrity() const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(bool enable);
    bool isRealTimeMonitoringEnabled() const { return realTimeMonitoring; }
    void onAllocation(size_t address, size_t size, const std::string& type);
    void onDeallocation(size_t address, const std::string& type);
    
private:
    // Hook implementations
    static void* mallocHook(size_t size);
    static void freeHook(void* ptr);
    static void* newHook(size_t size);
    static void deleteHook(void* ptr);
    static void* newArrayHook(size_t size);
    static void deleteArrayHook(void* ptr);
    static void* mmapHook(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    static void munmapHook(void* addr, size_t length);
    
    // Helper functions
    std::string captureStackTrace() const;
    std::string getAllocationSite() const;
    bool isAddressTracked(size_t address) const;
    const MemoryAllocation* getTrackedAllocation(size_t address) const;
    
    // Analysis functions
    std::vector<MemoryLeak> analyzeLeakedAllocations() const;
    std::vector<MemoryLeak> analyzeGrowthPatterns() const;
    std::vector<MemoryLeak> analyzeCallStacks() const;
    
    // Configuration
    LeakDetectionConfig config_;
    
    // State tracking
    bool hooksInstalled;
    bool tracking;
    bool realTimeMonitoring;
    std::string currentSession;
    std::chrono::system_clock::time_point sessionStartTime;
    
    // Memory tracking
    std::vector<MemoryAllocation> allocations;
    std::vector<MemoryAllocation> deallocations;
    std::map<size_t, std::string> allocationMap;
    std::set<size_t> deallocatedAddresses;
    
    // Thread safety
    mutable std::mutex allocationsMutex;
    
    // Session tracking
    std::map<std::string, std::vector<MemoryAllocation>> sessionAllocations;
    std::map<std::string, std::vector<MemoryAllocation>> sessionDeallocations;
};

// Memory leak detection utilities
namespace memory {
    
    // Memory allocation tracking
    class AllocationTracker {
    public:
        struct AllocationInfo {
            std::chrono::system_clock::time_point timestamp;
            size_t address;
            size_t size;
            std::string type;
            std::string threadId;
            std::string fileName;
            int lineNumber;
            
            AllocationInfo(size_t addr, size_t sz, const std::string& typeStr, 
                          const std::string& file, int line)
                : timestamp(std::chrono::system_clock::now()), address(addr),
                  size(sz), type(typeStr), threadId(""), fileName(file), lineNumber(line) {}
        };
        
        static AllocationTracker& getInstance() {
            static AllocationTracker instance;
            return instance;
        }
        
        void trackAllocation(size_t address, size_t size, const std::string& type,
                            const std::string& file = "", int line = 0);
        void trackDeallocation(size_t address, const std::string& type);
        
        std::vector<AllocationInfo> getAllocations() const { return allocations; }
        std::vector<AllocationInfo> getDeallocations() const { return deallocations; }
        
        size_t getTrackedCount() const { return allocations.size(); }
        size_t getDeallocatedCount() const { return deallocations.size(); }
        
        void clear();
        
    private:
        std::vector<AllocationInfo> allocations;
        std::vector<AllocationInfo> deallocations;
        std::mutex mutex;
    };
    
    // Memory leak analysis
    class LeakAnalyzer {
    public:
        struct LeakInfo {
            std::string type;
            size_t address;
            size_t size;
            std::string allocationSite;
            std::string callStack;
            double confidenceScore;
            
            LeakInfo(const std::string& type, size_t addr, size_t sz,
                    const std::string& site, const std::string& stack)
                : type(type), address(addr), size(sz), allocationSite(site),
                  callStack(stack), confidenceScore(1.0) {}
        };
        
        static LeakAnalyzer& getInstance() {
            static LeakAnalyzer instance;
            return instance;
        }
        
        std::vector<LeakInfo> analyzeLeaks(const std::vector<MemoryAllocation>& allocations,
                                          const std::vector<MemoryAllocation>& deallocations);
        
        std::vector<LeakInfo> findLeaksByType(const std::vector<LeakInfo>& leaks,
                                              const std::string& type) const;
        std::vector<LeakInfo> findLeaksBySite(const std::vector<LeakInfo>& leaks,
                                             const std::string& site) const;
        
        double calculateLeakConfidence(const LeakInfo& leak) const;
        std::string generateLeakReport(const std::vector<LeakInfo>& leaks) const;
        
        bool isLikelyLeak(const MemoryAllocation& alloc, 
                         const std::vector<MemoryAllocation>& deallocations) const;
        
    private:
        std::mutex analyzerMutex;
    };
    
    // Memory corruption detection
    class MemoryCorruptionDetector {
    public:
        struct CorruptionInfo {
            std::string corruptionType;
            size_t corruptionAddress;
            size_t corruptionSize;
            std::string corruptionContext;
            double corruptionScore;
            
            CorruptionInfo(const std::string& type, size_t addr, size_t sz,
                          const std::string& context)
                : corruptionType(type), corruptionAddress(addr), corruptionSize(sz),
                  corruptionContext(context), corruptionScore(1.0) {}
        };
        
        static MemoryCorruptionDetector& getInstance() {
            static MemoryCorruptionDetector instance;
            return instance;
        }
        
        std::vector<CorruptionInfo> detectCorruption(const std::vector<MemoryAllocation>& allocations);
        
        bool checkBufferOverflows(const MemoryAllocation& alloc) const;
        bool checkBufferUnderflows(const MemoryAllocation& alloc) const;
        bool checkUseAfterFree(const MemoryAllocation& alloc) const;
        bool checkDoubleFree(const MemoryAllocation& alloc) const;
        
        std::string getCorruptionReport(const std::vector<CorruptionInfo>& corruption) const;
        
    private:
        std::mutex detectorMutex;
    };
    
    // Memory pattern analysis
    class MemoryPatternAnalyzer {
    public:
        struct PatternInfo {
            std::string patternType;
            std::vector<size_t> addresses;
            std::vector<size_t> sizes;
            std::chrono::duration<double> patternDuration;
            double patternStrength;
            
            PatternInfo(const std::string& type) : patternType(type),
                                                  patternDuration(0.0), patternStrength(0.0) {}
        };
        
        static MemoryPatternAnalyzer& getInstance() {
            static MemoryPatternAnalyzer instance;
            return instance;
        }
        
        std::vector<PatternInfo> analyzePatterns(const std::vector<MemoryAllocation>& allocations);
        
        bool detectAllocationPattern(const std::vector<MemoryAllocation>& allocations) const;
        bool detectDeallocationPattern(const std::vector<MemoryAllocation>& allocations) const;
        bool detectGrowthPattern(const std::vector<MemoryAllocation>& allocations) const;
        
        double calculatePatternStrength(const PatternInfo& pattern) const;
        std::string getPatternReport(const std::vector<PatternInfo>& patterns) const;
        
    private:
        std::mutex analyzerMutex;
    };
    
} // namespace memory

} // namespace nlm

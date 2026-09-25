#pragma once

/**
 * NLM Memory Profiling Infrastructure
 * 
 * Comprehensive memory profiling framework for the NLM neural simulation framework.
 * Provides detailed memory usage analysis, performance optimization recommendations,
 * and memory pool resizing guidance for neural simulations.
 * 
 * Features:
 * - Memory profiling with detailed call graphs
 * - Memory hierarchy analysis (L1, L2, L3, RAM)
 * - Memory pressure monitoring and alerting
 * - Memory pool resizing recommendations
 * - Memory overhead analysis
 * - Integration with existing NLM systems
 * - Zero-overhead operation when disabled
 * 
 * Integration:
 * - Works with MemoryTracker for comprehensive tracking
 * - Integrates with MemoryDebug for corruption detection
 * - Supports Brain.cpp for simulation profiling
 * - Compatible with existing PerformanceMonitor
 * - Provides Python bindings for user-friendly APIs
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
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <queue>
#include <cmath>
#include <numeric>

// Configuration
#define NLM_MEMORY_PROFILING_ENABLED 1

// Memory hierarchy levels
enum class MemoryLevel {
    L1_CACHE,
    L2_CACHE,
    L3_CACHE,
    RAM,
    DISK
};

enum class ProfileType {
    AllocationProfile,
    DeallocationProfile,
    AccessProfile,
    PoolProfile,
    ThreadProfile
};

struct MemoryAccessInfo {
    size_t timestamp;              // Access timestamp
    size_t address;                 // Address accessed
    MemoryLevel level;              // Memory level accessed
    uint32_t threadId;              // Thread that accessed
    bool isRead;                    // Whether it was a read access
    bool isInstruction;             // Whether it was an instruction access
    double latency;                 // Access latency
    size_t size;                    // Size accessed
    std::string callSite;           // Call site information
};

struct AllocationProfile {
    size_t timestamp;               // Allocation timestamp
    size_t address;                  // Address allocated
    size_t size;                     // Size allocated
    std::string allocationSite;      // File and line where allocated
    std::string stackTrace;         // Stack trace at allocation
    uint32_t threadId;               // Thread that allocated
    std::vector<size_t> futureAccesses; // Future accesses to this block
    double lifetime;                 // Expected lifetime
    MemoryLevel expectedLevel;       // Expected memory level
    bool isPoolAllocation;           // Whether allocated from pool
    std::string poolName;            // Pool name (if applicable)
};

struct DeallocationProfile {
    size_t timestamp;               // Deallocation timestamp
    size_t address;                  // Address deallocated
    std::string deallocationSite;    // File and line where deallocated
    uint32_t threadId;               // Thread that deallocated
    AllocationProfile allocationInfo; // Related allocation profile
};

struct MemoryPoolProfile {
    std::string name;                 // Pool name
    size_t totalCapacity;             // Total pool capacity
    size_t currentUsage;              // Current usage
    size_t peakUsage;                 // Peak usage
    size_t allocationCount;           // Number of allocations
    size_t deallocationCount;         // Number of deallocations
    std::vector<size_t> blockSizes;   // Sizes of allocated blocks
    double efficiency;                // Pool efficiency (0-1)
    std::vector<double> timeSeries;   // Usage over time
    std::string tuningRecommendations; // Recommendations for optimization
};

struct MemoryHierarchyStats {
    MemoryLevel level;                // Memory level
    size_t totalSize;                 // Total size
    size_t usedSize;                  // Used size
    size_t availableSize;             // Available size
    size_t hitCount;                  // Number of cache hits
    size_t missCount;                 // Number of cache misses
    double hitRate;                   // Hit rate (0-1)
    double averageLatency;            // Average access latency
    std::map<size_t, size_t> sizeDistribution; // Size -> access count
};

struct MemoryPressure {
    double currentPressure;            // Current memory pressure (0-1)
    double pressureThreshold;         // Pressure threshold (0-1)
    size_t availableMemory;           // Available memory
    size_t totalMemory;               // Total memory
    double pressureIncreaseRate;      // Rate of pressure increase
    std::vector<double> pressureHistory; // Pressure over time
    std::vector<std::string> alerts;  // Memory pressure alerts
    bool isCritical;                  // Whether pressure is critical
};

struct MemoryOptimization {
    std::string optimizationType;     // Type of optimization
    double potentialGain;             // Expected performance gain
    std::string description;          // Description of optimization
    bool isAutoFixable;              // Whether auto-fix is possible
    std::string implementationPlan;   // Plan for implementation
    double implementationCost;        // Cost to implement (1-10)
};

struct ProfileSession {
    std::string sessionName;          // Session name
    double startTime;                 // Session start time
    double endTime;                   // Session end time
    size_t stepCount;                 // Number of simulation steps
    std::vector<MemoryAccessInfo> accesses; // Memory accesses
    std::vector<AllocationProfile> allocations; // Allocations
    std::vector<DeallocationProfile> deallocations; // Deallocations
    std::vector<MemoryPoolProfile> poolProfiles; // Memory pool profiles
    std::vector<MemoryHierarchyStats> hierarchyStats; // Memory hierarchy stats
    MemoryPressure pressure;          // Current memory pressure
};

class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Singleton pattern
    static MemoryProfiler& getInstance();
    
    // Configuration and control
    void enable(bool enable = true);
    bool isEnabled() const { return enabled_; }
    void setProfileType(ProfileType type) { profileType_ = type; }
    void setProfileInterval(double interval) { profileInterval_ = std::max(0.1, interval); }
    void setPressureThreshold(double threshold) { pressure_.pressureThreshold = std::max(0.0, std::min(1.0, threshold)); }
    
    // Memory access profiling
    void recordAccess(size_t address, MemoryLevel level, uint32_t threadId, bool isRead,
                     bool isInstruction, double latency, size_t size, const std::string& callSite);
    void recordAllocation(size_t address, size_t size, const std::string& site,
                         const std::string& stackTrace, uint32_t threadId);
    void recordDeallocation(size_t address, const std::string& site, uint32_t threadId);
    
    // Memory pool profiling
    void recordPoolUsage(const std::string& poolName, size_t allocated, size_t available);
    void recordPoolAllocation(const std::string& poolName, size_t size);
    void recordPoolDeallocation(const std::string& poolName, size_t size);
    
    // Hierarchy and pressure monitoring
    void updateMemoryHierarchy(const std::vector<MemoryHierarchyStats>& stats);
    void updateMemoryPressure(const MemoryPressure& pressure);
    
    // Profile session management
    void startProfile(const std::string& sessionName);
    void endProfile(const std::string& sessionName);
    ProfileSession getCurrentProfile() const;
    std::vector<ProfileSession> getProfileHistory() const;
    
    // Analysis and reporting
    std::vector<MemoryOptimization> getOptimizationRecommendations() const;
    std::string analyzeMemoryUsage() const;
    std::string analyzeAccessPatterns() const;
    std::string analyzePoolEfficiency() const;
    std::string analyzeMemoryHierarchy() const;
    std::string generateProfileReport() const;
    
    // Real-time analysis
    void analyzeCurrentUsage() const;
    void analyzeCurrentAccesses() const;
    void analyzeCurrentPools() const;
    
    // Integration with NLM systems
    void integrateWithMemoryTracker(const nlm::MemoryTracker* tracker);
    void integrateWithMemoryDebug(const nlm::MemoryDebug* debugger);
    void integrateWithBrain(const nlm::Brain* brain);
    void integrateWithPerformanceMonitor(const nlm::PerformanceMonitor* monitor);
    
    // Simulation integration
    void startStep(size_t step, double time);
    void endStep(size_t step, double time);
    
    // Reporting and analysis
    void saveProfileReport(const std::string& filename) const;
    void saveOptimizationReport(const std::string& filename) const;
    std::vector<std::string> getPerformanceAlerts() const;
    
    // Cleanup
    void flush();
    void reset();
    
private:
    // Core profiling state
    bool enabled_;
    bool initialized_;
    ProfileType profileType_;
    double profileInterval_;
    
    // Integration pointers
    const nlm::MemoryTracker* memoryTracker_;
    const nlm::MemoryDebug* memoryDebug_;
    const nlm::Brain* brain_;
    const nlm::PerformanceMonitor* performanceMonitor_;
    
    // Profiling data structures
    std::vector<MemoryAccessInfo> memoryAccesses_;
    std::vector<AllocationProfile> allocations_;
    std::vector<DeallocationProfile> deallocations_;
    std::vector<MemoryPoolProfile> poolProfiles_;
    std::vector<MemoryHierarchyStats> hierarchyStats_;
    MemoryPressure pressure_;
    
    // Profile sessions
    std::vector<ProfileSession> profileHistory_;
    ProfileSession currentProfile_;
    
    // Performance alerts
    std::vector<std::string> performanceAlerts_;
    
    // Statistics
    size_t totalAccesses_;
    size_t totalAllocations_;
    size_t totalDeallocations_;
    size_t totalBytesAccessed_;
    size_t totalBytesAllocated_;
    size_t totalBytesDeallocated_;
    
    // Simulation tracking
    size_t currentStep_;
    double currentTime_;
    
    // Thread safety
    mutable std::shared_mutex dataMutex_;
    std::mutex profileSessionMutex_;
    
    // Utility functions
    std::string getCurrentStackTrace() const;
    std::string getAllocationSite() const;
    void updateMemoryPressure();
    void checkForPressureAlerts();
    void updateProfileEfficiency();
    void analyzeAccessPatterns() const;
    void analyzeMemoryUsage() const;
    void analyzePoolEfficiency() const;
    void analyzeHierarchyEfficiency() const;
    void generateOptimizationRecommendations() const;
    void detectMemoryIssues() const;
    void detectPerformanceIssues() const;
    
    // Memory usage analysis
    size_t calculateWorkingSet() const;
    double calculateLocalityScore() const;
    double calculateFragmentationIndex() const;
    double calculatePoolEfficiency(const MemoryPoolProfile& pool) const;
    
    // Access pattern analysis
    std::map<size_t, double> analyzeTemporalPatterns() const;
    std::map<size_t, double> analyzeSpatialPatterns() const;
    std::vector<size_t> findHotSpots() const;
    std::vector<size_t> findColdSpots() const;
    
    // Performance optimization
    void recommendPoolResizing() const;
    void recommendCacheOptimizations() const;
    void recommendMemoryCompression() const;
    void recommendThreadLocalStorage() const;
    void recommendAccessPatternOptimizations() const;
    
    // Profile session management
    void finalizeCurrentProfile();
    void startNewProfile(const std::string& sessionName);
    ProfileSession createSessionSnapshot() const;
    
    // Reporting utilities
    std::string formatProfileReport() const;
    std::string formatOptimizationReport() const;
    std::string formatMemoryUsageReport() const;
    std::string formatAccessPatternReport() const;
    std::string formatPoolEfficiencyReport() const;
    std::string formatHierarchyReport() const;
    
    // Memory level utilities
    MemoryLevel getMemoryLevel(size_t address) const;
    double getEstimatedLatency(MemoryLevel level) const;
    bool isInCache(size_t address, MemoryLevel level) const;
    
    // Profile type utilities
    bool shouldRecord(const std::string& site) const;
    bool shouldSample() const;
    
    // Alert management
    void addAlert(const std::string& alert);
    void clearAlerts();
    bool hasCriticalAlerts() const;
    
    // Optimization recommendation generation
    void generateMemoryOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const;
    void generatePerformanceOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const;
    void generateCacheOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const;
    void generatePoolOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const;
    void generateAccessOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const;
    
    // Report generation
    std::string generateProfileSummary() const;
    std::string generateMemoryUsageAnalysis() const;
    std::string generateAccessPatternAnalysis() const;
    std::string generatePoolEfficiencyAnalysis() const;
    std::string generateHierarchyAnalysis() const;
    std::string generateOptimizationAnalysis() const;
    
    // Profile utilities
    std::string getProfileTypeString(ProfileType type) const;
    std::string getMemoryLevelString(MemoryLevel level) const;
    double calculateTimeSince(size_t timestamp) const;
    size_t getCurrentTime() const;
    std::string getCurrentTimeString() const;
};
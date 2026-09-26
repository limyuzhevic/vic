#pragma once

/**
 * Performance Profiling Tools
 * 
 * Comprehensive profiling infrastructure for identifying performance bottlenecks
 * in neural simulation, including CPU, memory, and cache analysis.
 * 
 * Features:
 * - Hardware performance counters (CPU cycles, cache misses, branch mispredictions)
 * - Memory allocation tracking and analysis
 * - Neural computation profiling
 * - Real-time performance monitoring
 * - Comparative analysis across optimization levels
 * 
 * Integration:
 * - Works with NLM performance infrastructure
 * - Provides detailed timing information
 * - Supports custom profiling hooks
 * - Generates performance reports
 */

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <atomic>
#include <thread>
#include <map>
#include <algorithm>

namespace nlm {

class PerformanceProfiler {
public:
    /**
     * Profile measurement types
     */
    enum class MeasurementType {
        CPU_CYCLES,
        INSTRUCTIONS,
        CACHE_MISSES,
        BRANCH_MISPREDICTIONS,
        MEMORY_ACCESSES,
        FLOATING_POINT_OPS,
        NEURAL_UPDATES,
        SYNAPTIC_OPERATIONS,
        MEMORY_ALLOCATIONS,
        THREAD_WAIT_TIMES
    };
    
    /**
     * Profile category
     */
    enum class ProfileCategory {
        INITIALIZATION,
        NEURAL_DYNAMICS,
        PLASTICITY,
        MEMORY_OPERATIONS,
        I_O_OPERATIONS,
        ALGORITHMIC_OPERATIONS,
        CACHE_OPTIMIZATIONS,
        SIMD_OPERATIONS
    };
    
    struct ProfileData {
        std::string name;
        ProfileCategory category;
        MeasurementType type;
        uint64_t startTime;
        uint64_t endTime;
        uint64_t duration;
        uint64_t startCpuCycles;
        uint64_t startInstructions;
        uint64_t startCacheMisses;
        uint64_t startMemoryAccesses;
        uint64_t endCpuCycles;
        uint64_t endInstructions;
        uint64_t endCacheMisses;
        uint64_t endMemoryAccesses;
        uint64_t allocationsCount;
        uint64_t totalAllocatedBytes;
        double cpuUsage;
        
        ProfileData() : startTime(0), endTime(0), duration(0),
                       startCpuCycles(0), endCpuCycles(0),
                       startInstructions(0), endInstructions(0),
                       startCacheMisses(0), endCacheMisses(0),
                       startMemoryAccesses(0), endMemoryAccesses(0),
                       allocationsCount(0), totalAllocatedBytes(0),
                       cpuUsage(0.0) {}
    };
    
    /**
     * Create a new profiling session
     */
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler profiler;
        return profiler;
    }
    
    /**
     * Start profiling a specific operation
     */
    void startProfile(const std::string& name, ProfileCategory category = ProfileCategory::NEURAL_DYNAMICS);
    
    /**
     * End profiling an operation
     */
    void endProfile(const std::string& name);
    
    /**
     * Profile a single operation's execution time
     */
    template<typename Func>
    auto profile(const std::string& name, Func&& func, ProfileCategory category = ProfileCategory::NEURAL_DYNAMICS) {
        startProfile(name, category);
        auto result = func();
        endProfile(name);
        return result;
    }
    
    /**
     * Get current profile data
     */
    const ProfileData* getProfile(const std::string& name) const;
    
    /**
     * Get all profile data
     */
    const std::vector<ProfileData>& getAllProfiles() const { return profiles_; }
    
    /**
     * Clear all profile data
     */
    void clearProfiles();
    
    /**
     * Generate performance report
     */
    void generateReport(const std::string& filename) const;
    
    /**
     * Get total duration of all profiles
     */
    uint64_t getTotalDuration() const;
    
    /**
     * Get profile count
     */
    size_t getProfileCount() const { return profiles_.size(); }
    
    /**
     * Get performance statistics for a category
     */
    struct CategoryStats {
        ProfileCategory category;
        uint64_t totalDuration;
        uint64_t totalCpuCycles;
        uint64_t totalInstructions;
        uint64_t totalCacheMisses;
        uint64_t totalMemoryAccesses;
        uint64_t totalAllocations;
        uint64_t totalAllocatedBytes;
        double averageCpuUsage;
        
        CategoryStats() : category(ProfileCategory::NEURAL_DYNAMICS),
                         totalDuration(0), totalCpuCycles(0), totalInstructions(0),
                         totalCacheMisses(0), totalMemoryAccesses(0),
                         totalAllocations(0), totalAllocatedBytes(0),
                         averageCpuUsage(0.0) {}
    };
    
    std::vector<CategoryStats> getCategoryStats() const;
    
    /**
     * Start hardware performance counters
     */
    void startHardwareCounters();
    
    /**
     * Stop hardware performance counters
     */
    void stopHardwareCounters();
    
    /**
     * Check if hardware counters are available
     */
    bool isHardwareCounterAvailable() const;
    
    /**
     * Enable detailed memory profiling
     */
    void enableMemoryProfiling(bool enable = true) { memoryProfilingEnabled_ = enable; }
    
    /**
     * Enable cache analysis
     */
    void enableCacheAnalysis(bool enable = true) { cacheAnalysisEnabled_ = enable; }
    
private:
    PerformanceProfiler() : memoryProfilingEnabled_(false), cacheAnalysisEnabled_(false) {
        startHardwareCounters();
    }
    
    // Delete copy and assignment
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * Get current CPU cycles (platform-specific)
     */
    uint64_t getCpuCycles();
    
    /**
     * Get current timestamp
     */
    uint64_t getTimestamp();
    
    /**
     * Get current cache misses (platform-specific)
     */
    uint64_t getCacheMisses();
    
    /**
     * Get current instructions retired (platform-specific)
     */
    uint64_t getInstructionsRetired();
    
    /**
     * Get current memory accesses (platform-specific)
     */
    uint64_t getMemoryAccesses();
    
    std::vector<ProfileData> profiles_;
    bool memoryProfilingEnabled_;
    bool cacheAnalysisEnabled_;
    uint64_t lastCpuCycles_;
    uint64_t lastInstructions_;
    uint64_t lastCacheMisses_;
    uint64_t lastMemoryAccesses_;
};

} // namespace nlm

// Global convenience functions

inline void startProfile(const std::string& name) {
    nlm::PerformanceProfiler::getInstance().startProfile(name);
}

inline void endProfile(const std::string& name) {
    nlm::PerformanceProfiler::getInstance().endProfile(name);
}

inline template<typename Func>
auto profile(const std::string& name, Func&& func) {
    return nlm::PerformanceProfiler::getInstance().template profile<Func>(name, std::forward<Func>(func));
}
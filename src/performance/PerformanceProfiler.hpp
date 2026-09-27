#pragma once

/**
 * Performance Profiler
 * 
 * High-resolution performance profiler for detailed call stack analysis
 * and automatic identification of performance bottlenecks in the NLM brain
 * simulation project.
 * 
 * Features:
 * - Call stack profiling with stack traces
 * - Hot path identification
 * - Dependency analysis for performance impact
 * - SIMD instruction usage tracking
 * - Memory layout analysis
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <stacktrace>
#include <optional>

namespace nlm {

class PerformanceProfiler {
public:
    /**
     * Profile entry for a function call
     */
    struct ProfileEntry {
        std::string function_name;
        std::string file_path;
        int line_number;
        std::chrono::high_resolution_clock::time_point timestamp;
        double duration_seconds;
        int call_depth;
        std::stacktrace stack_trace;
        uint64_t cpu_cycles;
        bool is_hot_path;
        int call_count;
    };

    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /** Start profiling a function */
    void startProfile(const std::string& function_name, const std::string& file_path = "", int line_number = 0);
    
    /** Stop profiling and record results */
    void stopProfile();
    
    /** Record a function call */
    void recordCall(const std::string& function_name, double duration, uint64_t cpu_cycles = 0);
    
    /** Get all profile entries */
    const std::vector<ProfileEntry>& getProfileEntries() const { return profile_entries_; }
    
    /** Identify hot paths based on duration */
    std::vector<std::string> identifyHotPaths(double threshold_seconds = 0.001) const;
    
    /** Analyze call dependencies */
    std::unordered_map<std::string, std::vector<std::string>> analyzeDependencies() const;
    
    /** Get total profile time */
    double getTotalProfileTime() const { return total_profile_time_; }
    
    /** Clear all profile data */
    void clear();
    
    /** Enable/disable SIMD tracking */
    void enableSimdTracking(bool enable) { track_simd_ = enable; }
    
    /** Enable/disable memory layout tracking */
    void enableMemoryLayoutTracking(bool enable) { track_memory_layout_ = enable; }
    
private:
    std::vector<ProfileEntry> profile_entries_;
    double total_profile_time_;
    std::stacktrace current_stack_;
    std::string current_function_name_;
    std::chrono::high_resolution_clock::time_point start_time_;
    bool track_simd_;
    bool track_memory_layout_;
};

} // namespace nlm

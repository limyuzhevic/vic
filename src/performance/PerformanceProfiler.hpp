#pragma once

/**
 * Performance Profiler
 * 
 * Tools for profiling specific functions and code sections
 * with minimal overhead
 */

#include <chrono>
#include <string>
#include <unordered_map>
#include <mutex>
#include "PerformanceMonitoring.hpp"

namespace nlm {

/**
 * Performance Profiler Implementation
 */
class PerformanceProfiler {
private:
    struct ProfileScope {
        uint64_t start_time_ns = 0;
        std::string function_name;
        std::string file_name;
        uint32_t line_number = 0;
        uint32_t thread_id = 0;
    };
    
    std::vector<ProfileScope> active_scopes;
    mutable std::mutex scopes_mutex;
    PerformanceMonitor* monitor;

public:
    PerformanceProfiler(PerformanceMonitor* monitor = nullptr);
    ~PerformanceProfiler();
    
    // Manual profiling
    void start(const std::string& function_name, const std::string& file_name = "", uint32_t line_number = 0);
    void stop(const std::string& function_name);
    
    // RAII-style profiling
    class Scope {
    private:
        PerformanceProfiler* profiler;
        std::string function_name;
        std::string file_name;
        uint32_t line_number;
    public:
        Scope(PerformanceProfiler* profiler, const std::string& function_name, 
              const std::string& file_name = "", uint32_t line_number = 0);
        ~Scope();
    };
    
    Scope makeScope(const std::string& function_name, const std::string& file_name = "", uint32_t line_number = 0);
};

} // namespace nlm
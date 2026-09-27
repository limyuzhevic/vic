#pragma once

/**
 * CPU Profiler
 * 
 * CPU performance and instruction analysis for the NLM brain simulation project.
 * 
 * Features:
 * - CPU utilization monitoring
 * - Instruction timing analysis
 * - SIMD instruction usage tracking
 * - Cache performance monitoring
 * - Thread scheduling analysis
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class CpuProfiler {
public:
    /**
     * CPU performance metrics
     */
    struct PerformanceMetrics {
        double cpu_utilization; // Percentage (0-100)
        uint64_t cycles_per_instruction;
        uint64_t instructions_per_cycle;
        uint64_t cache_misses;
        uint64_t cache_hits;
        double memory_bandwidth;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    /**
     * SIMD instruction metrics
     */
    struct SimdMetrics {
        std::string simd_instruction_set; // "SSE", "AVX", "AVX2", "AVX512"
        uint64_t simd_instructions_executed;
        uint64_t scalar_instructions_executed;
        double simd_speedup;
    };

    CpuProfiler();
    ~CpuProfiler();
    
    /** Record CPU performance metrics */
    void recordMetrics(const PerformanceMetrics& metrics);
    
    /** Record SIMD instruction usage */
    void recordSimdMetrics(const SimdMetrics& metrics);
    
    /** Get current CPU utilization */
    double getCpuUtilization() const;
    
    /** Get average cycles per instruction */
    double getAverageCPI() const { return average_cpi_; }
    
    /** Get average instructions per cycle */
    double getAverageIPC() const { return average_ipc_; }
    
    /** Get cache hit rate */
    double getCacheHitRate() const;
    
    /** Get total cache misses */
    uint64_t getTotalCacheMisses() const { return total_cache_misses_; }
    
    /** Get total cache hits */
    uint64_t getTotalCacheHits() const { return total_cache_hits_; }
    
    /** Get current SIMD metrics */
    const SimdMetrics& getCurrentSimdMetrics() const { return current_simd_metrics_; }
    
    /** Calculate performance efficiency */
    double calculatePerformanceEfficiency() const;
    
    /** Get performance history */
    const std::vector<PerformanceMetrics>& getPerformanceHistory() const { return performance_history_; }
    
    /** Clear history */
    void clear();
    
    /** Enable SIMD tracking */
    void enableSimdTracking(bool enable) { track_simd_ = enable; }
    
    /** Enable cache tracking */
    void enableCacheTracking(bool enable) { track_cache_ = enable; }
    
private:
    std::vector<PerformanceMetrics> performance_history_;
    SimdMetrics current_simd_metrics_;
    double average_cpi_;
    double average_ipc_;
    uint64_t total_cache_misses_;
    uint64_t total_cache_hits_;
    bool track_simd_;
    bool track_cache_;
};

} // namespace nlm

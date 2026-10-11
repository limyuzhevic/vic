// Memory Usage Monitor Header for NLM
// Comprehensive memory monitoring and analysis for NLM tests
// Includes memory tracking, peak detection, and memory leak detection

#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <fixedpoint>

namespace test_memory {

struct MemorySample {
    uint64_t timestamp;      // Elapsed time in milliseconds
    size_t sample_id;         // Sample sequence number
    size_t memory_usage;      // Memory usage in bytes
    size_t memory_increase;   // Memory increase since previous sample
};

struct MemoryUsageReport {
    size_t total_samples;
    size_t peak_memory;
    size_t avg_memory_usage;
    size_t current_memory;
    size_t total_allocated;
    size_t total_freed;
    double memory_growth_rate;
    size_t max_memory_increase;
    double avg_memory_increase;
    uint64_t duration_ms;
    double samples_per_second;
    double memory_per_second; // bytes per millisecond
    double memory_leak_slope;
    bool memory_leak_detected;
};

class MemoryUsageMonitor {
public:
    MemoryUsageMonitor();
    ~MemoryUsageMonitor();
    
    void initializeMonitor();
    void cleanupMonitor();
    
    void startMonitoring();
    void stopMonitoring();
    void takeSample();
    
    void trackAllocation(size_t size);
    void trackDeallocation(size_t size);
    
    void setSampleInterval(int interval_ms);
    void setMaxSamples(int max);
    
    MemoryUsageReport calculateStatistics();
    bool detectMemoryLeak(MemoryUsageReport& report);
    
    void printMemoryReport();
    
private:
    bool is_monitoring;
    std::vector<MemorySample> memory_samples;
    size_t peak_memory;
    size_t current_memory;
    size_t total_allocated;
    size_t total_freed;
    int sample_interval_ms;
    size_t max_samples;
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point monitoringStartTime;
    std::chrono::high_resolution_clock::time_point monitoringEndTime;
};

} // namespace test_memory

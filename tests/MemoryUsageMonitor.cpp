// Memory Usage Monitor for NLM
// Comprehensive memory monitoring and analysis for NLM tests
// Includes memory tracking, peak detection, and memory leak detection

#include "tests/MemoryUsageMonitor.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <ctime>

namespace test_memory {

MemoryUsageMonitor::MemoryUsageMonitor() {
    initializeMonitor();
}

MemoryUsageMonitor::~MemoryUsageMonitor() {
    cleanupMonitor();
}

void MemoryUsageMonitor::initializeMonitor() {
    sample_interval_ms = 100;
    max_samples = 1000;
    is_monitoring = false;
    
    memory_samples.clear();
    peak_memory = 0;
    current_memory = 0;
    total_allocated = 0;
    total_freed = 0;
    
    // Initialize memory tracking
    startTime = std::chrono::high_resolution_clock::now();
}

void MemoryUsageMonitor::cleanupMonitor() {
    memory_samples.clear();
    is_monitoring = false;
    peak_memory = 0;
    current_memory = 0;
}

void MemoryUsageMonitor::startMonitoring() {
    if (is_monitoring) {
        std::cout << "Warning: Monitoring already started" << std::endl;
        return;
    }
    
    is_monitoring = true;
    monitoringStartTime = std::chrono::high_resolution_clock::now();
    
    std::cout << "Memory monitoring started" << std::endl;
}

void MemoryUsageMonitor::stopMonitoring() {
    if (!is_monitoring) {
        std::cout << "Warning: Monitoring not started" << std::endl;
        return;
    }
    
    is_monitoring = false;
    monitoringEndTime = std::chrono::high_resolution_clock::now();
    
    // Calculate final statistics
    calculateStatistics();
    
    std::cout << "Memory monitoring stopped" << std::endl;
    printMemoryReport();
}

void MemoryUsageMonitor::takeSample() {
    if (!is_monitoring) return;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - monitoringStartTime);
    
    // Only sample at specified intervals
    if (elapsed.count() < sample_interval_ms * (memory_samples.size() + 1)) {
        return;
    }
    
    // Get memory usage (placeholder - would use system-specific memory monitoring)
    // For now, we'll use simulated memory tracking
    MemorySample sample;
    sample.timestamp = elapsed.count();
    sample.sample_id = memory_samples.size();
    
    // Simulate memory usage based on sample number
    // In real implementation, this would query system memory usage
    double base_memory = 10000.0; // 10MB base
    double growth_rate = 0.5; // 0.5MB per sample
    double variability = (rand() % 100) / 100.0 * 2.0 - 1.0; // ±1MB variability
    
    current_memory = static_cast<size_t>(base_memory + growth_rate * sample.sample_id + variability * 1024.0);
    sample.memory_usage = current_memory;
    sample.memory_increase = current_memory - (sample.sample_id > 0 ? memory_samples[sample.sample_id - 1].memory_usage : current_memory);
    
    // Update peak memory
    if (current_memory > peak_memory) {
        peak_memory = current_memory;
    }
    
    memory_samples.push_back(sample);
    
    // Limit number of samples
    if (memory_samples.size() > max_samples) {
        memory_samples.erase(memory_samples.begin());
    }
}

void MemoryUsageMonitor::trackAllocation(size_t size) {
    if (is_monitoring) {
        total_allocated += size;
        current_memory += size;
        if (current_memory > peak_memory) {
            peak_memory = current_memory;
        }
    }
}

void MemoryUsageMonitor::trackDeallocation(size_t size) {
    if (is_monitoring) {
        total_freed += size;
        current_memory = (current_memory >= size) ? current_memory - size : 0;
    }
}

void MemoryUsageMonitor::setSampleInterval(int interval_ms) {
    sample_interval_ms = interval_ms;
}

void MemoryUsageMonitor::setMaxSamples(int max) {
    max_samples = max;
}

MemoryUsageReport MemoryUsageMonitor::calculateStatistics() {
    MemoryUsageReport report;
    
    if (memory_samples.empty()) {
        return report;
    }
    
    report.total_samples = memory_samples.size();
    report.peak_memory = peak_memory;
    report.total_allocated = total_allocated;
    report.total_freed = total_freed;
    report.current_memory = current_memory;
    
    // Calculate average memory usage
    double sum_memory = 0.0;
    double sum_increase = 0.0;
    double max_increase = 0.0;
    
    for (const auto& sample : memory_samples) {
        sum_memory += sample.memory_usage;
        sum_increase += sample.memory_increase;
        if (sample.memory_increase > max_increase) {
            max_increase = sample.memory_increase;
        }
    }
    
    report.avg_memory_usage = sum_memory / report.total_samples;
    report.avg_memory_increase = sum_increase / report.total_samples;
    report.max_memory_increase = max_increase;
    
    // Calculate memory growth rate
    if (report.total_samples >= 2) {
        double initial_memory = memory_samples.front().memory_usage;
        double final_memory = memory_samples.back().memory_usage;
        report.memory_growth_rate = (final_memory - initial_memory) / initial_memory * 100.0; // percentage
    }
    
    // Calculate time metrics
    if (monitoringEndTime.time_since_epoch().count() > 0) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(monitoringEndTime - monitoringStartTime);
        report.duration_ms = duration.count();
        
        if (duration.count() > 0) {
            report.samples_per_second = (report.total_samples * 1000.0) / duration.count();
            report.memory_per_second = report.total_allocated / duration.count(); // bytes per ms
        }
    }
    
    // Detect memory leaks
    report.memory_leak_detected = detectMemoryLeak(report);
    
    return report;
}

bool MemoryUsageMonitor::detectMemoryLeak(MemoryUsageReport& report) {
    if (memory_samples.size() < 10) {
        return false;
    }
    
    // Analyze last 50% of samples for memory growth trend
    size_t analysis_start = memory_samples.size() / 2;
    size_t analysis_end = memory_samples.size();
    
    if (analysis_start >= analysis_end) {
        return false;
    }
    
    // Calculate linear regression slope for memory growth
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
    size_t n = analysis_end - analysis_start;
    
    for (size_t i = analysis_start; i < analysis_end; ++i) {
        double x = static_cast<double>(i - analysis_start);
        double y = static_cast<double>(memory_samples[i].memory_usage);
        
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
    }
    
    // Calculate slope (memory growth rate per sample)
    double slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    
    // Consider it a memory leak if slope is positive and significant
    report.memory_leak_slope = slope;
    report.memory_leak_detected = (slope > 0.1 && slope < 100.0); // 0.1-100 bytes per sample
    
    return report.memory_leak_detected;
}

void MemoryUsageMonitor::printMemoryReport() {
    MemoryUsageReport report = calculateStatistics();
    
    std::cout << "\n=== Memory Usage Report ===" << std::endl;
    std::cout << "Total Samples: " << report.total_samples << std::endl;
    std::cout << "Peak Memory: " << report.peak_memory << " bytes" << std::endl;
    std::cout << "Average Memory: " << report.avg_memory_usage << " bytes" << std::endl;
    std::cout << "Current Memory: " << report.current_memory << " bytes" << std::endl;
    std::cout << "Memory Growth Rate: " << std::fixed << std::setprecision(2) << report.memory_growth_rate << "%" << std::endl;
    std::cout << "Max Memory Increase: " << report.max_memory_increase << " bytes" << std::endl;
    std::cout << "Total Allocated: " << report.total_allocated << " bytes" << std::endl;
    std::cout << "Total Freed: " << report.total_freed << " bytes" << std::endl;
    std::cout << "Memory Leak Detected: " << (report.memory_leak_detected ? "YES" : "NO") << std::endl;
    
    if (report.memory_leak_detected) {
        std::cout << "Memory Leak Slope: " << std::fixed << std::setprecision(6) << report.memory_leak_slope << " bytes/sample" << std::endl;
        std::cout << "Warning: Potential memory leak detected!" << std::endl;
    }
}

} // namespace test_memory

// NLM Performance Optimizations Implementation
// Performance Monitor Implementation

#include "core/performance/PerformanceMonitor.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>

namespace nlm {
namespace performance {

// PerformanceMonitor implementation

PerformanceMetrics PerformanceMonitor::getMetrics() const {
    PerformanceMetrics metrics;
    
    // Calculate average operation time
    if (totalOperations_ > 0) {
        metrics.responseTime = totalDuration_ / totalOperations_;
    }
    
    metrics.maxOperationTime = maxOperationTime_;
    
    // Simulate other metrics (in real implementation, these would be measured)
    metrics.cpuUsage = getCpuUsage();
    metrics.memoryUsage = getMemoryUsage();
    metrics.memoryAvailable = getMemoryAvailable();
    metrics.diskUsage = getDiskUsage();
    metrics.networkUsage = getNetworkUsage();
    metrics.activeThreads = getActiveThreads();
    metrics.throughput = (totalOperations_ > 0) ? (totalOperations_ / (totalDuration_ / 1000.0)) : 0.0;
    
    return metrics;
}

std::string PerformanceMonitor::getPerformanceReport() const {
    std::ostringstream oss;
    oss << "=== NLM Performance Report ===" << std::endl;
    oss << "Monitoring: " << (isMonitoring() ? "YES" : "NO") << std::endl;
    
    if (isMonitoring()) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - startTime_).count();
        oss << "Uptime: " << duration << " seconds" << std::endl;
    }
    
    PerformanceMetrics metrics = getMetrics();
    oss << std::endl << "=== Current Metrics ===" << std::endl;
    oss << "Response Time: " << metrics.responseTime << " ms" << std::endl;
    oss << "Max Operation Time: " << metrics.maxOperationTime << " ms" << std::endl;
    oss << "Throughput: " << metrics.throughput << " ops/sec" << std::endl;
    oss << "FPS: " << metrics.getFPS() << std::endl;
    oss << std::endl << "=== System Resources ===" << std::endl;
    oss << "CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    oss << "Memory Usage: " << metrics.memoryUsage << "%" << std::endl;
    oss << "Memory Available: " << (metrics.memoryAvailable / (1024.0 * 1024.0)) << " MB" << std::endl;
    oss << "Disk Usage: " << metrics.diskUsage << "%" << std::endl;
    oss << "Network Usage: " << metrics.networkUsage << " KB/s" << std::endl;
    oss << "Active Threads: " << metrics.activeThreads << std::endl;
    oss << std::endl << "=== Operation Statistics ===" << std::endl;
    
    for (const auto& pair : operations_) {
        oss << pair.first << ": " << pair.second.size() << " calls, ";
        if (!pair.second.empty()) {
            double avgTime = std::accumulate(pair.second.begin(), pair.second.end(), 0.0) / pair.second.size();
            oss << "avg: " << avgTime << "ms, ";
            oss << "min: " << *std::min_element(pair.second.begin(), pair.second.end()) << "ms, ";
            oss << "max: " << *std::max_element(pair.second.begin(), pair.second.end()) << "ms";
        }
        oss << std::endl;
    }
    
    return oss.str();
}

} // namespace performance
} // namespace nlm

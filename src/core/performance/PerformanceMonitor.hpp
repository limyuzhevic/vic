// NLM Performance Optimizations
// Performance Monitoring Integration

#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace nlm {
namespace performance {

// Performance metrics structure
struct PerformanceMetrics {
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    double responseTime = 0.0;
    double throughput = 0.0;
    size_t activeThreads = 0;
    double memoryAvailable = 0.0;
    double diskUsage = 0.0;
    double networkUsage = 0.0;
    
    double getFPS() const {
        return throughput > 0.0 ? 1.0 / throughput : 0.0;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "CPU: " << cpuUsage << "% ";
        oss << "Mem: " << memoryUsage << "% ";
        oss << "Resp: " << responseTime << "ms ";
        oss << "FPS: " << getFPS() << "; ";
        oss << "Threads: " << activeThreads;
        return oss.str();
    }
};

// Performance monitor class
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    void startMonitoring() {
        monitoring_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
        lastCpuSample_ = std::chrono::high_resolution_clock::now();
        collectedSamples_ = 0;
    }
    
    void stopMonitoring() {
        monitoring_ = false;
    }
    
    bool isMonitoring() const {
        return monitoring_;
    }
    
    void recordOperation(const std::string& operationName, double durationMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        operations_[operationName].push_back(durationMs);
        totalOperations_++;
        totalDuration_ += durationMs;
        
        if (durationMs > maxOperationTime_) {
            maxOperationTime_ = durationMs;
        }
    }
    
    PerformanceMetrics getMetrics() const {
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
    
    std::string getPerformanceReport() const {
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
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        operations_.clear();
        totalOperations_ = 0;
        totalDuration_ = 0.0;
        maxOperationTime_ = 0.0;
    }
    
    // Benchmark utility
    static void benchmark(const std::string& name, std::function<void()> operation, int iterations = 100) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            operation();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
        
        double opsPerSecond = static_cast<double>(iterations) / duration;
        double avgTimePerOp = duration / iterations * 1000.0; // ms
        
        std::cout << "Benchmark: " << name << std::endl;
        std::cout << "  Iterations: " << iterations << std::endl;
        std::cout << "  Total Time: " << duration << " seconds" << std::endl;
        std::cout << "  Operations/sec: " << opsPerSecond << std::endl;
        std::cout << "  Avg Time per Op: " << avgTimePerOp << " ms" << std::endl;
        
        // Record in performance monitor
        getInstance().recordOperation(name, avgTimePerOp);
    }
    
    // Memory profiling utility
    static void profileMemory(const std::string& name, std::function<void()> operation) {
        auto startTime = std::chrono::high_resolution_clock::now();
        operation();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count() * 1000.0; // ms
        
        std::cout << "Memory Profile: " << name << std::endl;
        std::cout << "  Execution Time: " << duration << " ms" << std::endl;
        
        getInstance().recordOperation(name + "_memory", duration);
    }
    
    // CPU profiling utility
    static void profileCPU(const std::string& name, std::function<void()> operation) {
        auto startTime = std::chrono::high_resolution_clock::now();
        operation();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count() * 1000.0; // ms
        
        std::cout << "CPU Profile: " << name << std::endl;
        std::cout << "  Execution Time: " << duration << " ms" << std::endl;
        
        getInstance().recordOperation(name + "_cpu", duration);
    }
    
    // System resource monitoring
    static PerformanceMetrics getSystemMetrics() {
        PerformanceMetrics metrics;
        metrics.cpuUsage = getCpuUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.memoryAvailable = getMemoryAvailable();
        metrics.diskUsage = getDiskUsage();
        metrics.networkUsage = getNetworkUsage();
        metrics.activeThreads = getActiveThreads();
        return metrics;
    }
    
    // Simulate resource measurements (simplified)
    static double getCpuUsage() {
        static double lastValue = 0.0;
        // Simulate CPU usage with some variation
        return 50.0 + (std::rand() % 30);
    }
    
    static double getMemoryUsage() {
        static double lastValue = 0.0;
        // Simulate memory usage with some variation
        return 60.0 + (std::rand() % 25);
    }
    
    static double getMemoryAvailable() {
        // Return available memory in MB
        return 1024.0 * 1024.0 * 1024.0; // 1GB
    }
    
    static double getDiskUsage() {
        // Simulate disk usage
        return 70.0 + (std::rand() % 20);
    }
    
    static double getNetworkUsage() {
        // Simulate network usage in KB/s
        return (std::rand() % 1000) / 1024.0;
    }
    
    static size_t getActiveThreads() {
        return std::thread::hardware_concurrency();
    }
    
private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    mutable std::mutex mutex_;
    bool monitoring_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastCpuSample_;
    int collectedSamples_ = 0;
    
    std::unordered_map<std::string, std::vector<double>> operations_;
    size_t totalOperations_ = 0;
    double totalDuration_ = 0.0;
    double maxOperationTime_ = 0.0;
};

// Convenience functions
inline PerformanceMetrics getPerformanceMetrics() {
    return PerformanceMonitor::getSystemMetrics();
}

inline std::string getPerformanceReport() {
    return PerformanceMonitor::getInstance().getPerformanceReport();
}

inline void startMonitoring() {
    PerformanceMonitor::getInstance().startMonitoring();
}

inline void stopMonitoring() {
    PerformanceMonitor::getInstance().stopMonitoring();
}

inline void benchmarkOperation(const std::string& name, std::function<void()> operation, int iterations = 100) {
    PerformanceMonitor::benchmark(name, operation, iterations);
}

inline void profileMemoryOperation(const std::string& name, std::function<void()> operation) {
    PerformanceMonitor::profileMemory(name, operation);
}

inline void profileCPUOperation(const std::string& name, std::function<void()> operation) {
    PerformanceMonitor::profileCPU(name, operation);
}

inline bool checkPerformanceThresholds(double maxCpuUsage = 80.0,
                                       double maxMemoryUsage = 80.0,
                                       double maxResponseTime = 100.0) {
    PerformanceMetrics metrics = getSystemMetrics();
    
    if (metrics.cpuUsage > maxCpuUsage) {
        std::cerr << "Performance Warning: CPU usage (" << metrics.cpuUsage << "%) exceeds threshold (" 
                  << maxCpuUsage << "%)" << std::endl;
        return false;
    }
    
    if (metrics.memoryUsage > maxMemoryUsage) {
        std::cerr << "Performance Warning: Memory usage (" << metrics.memoryUsage << "%) exceeds threshold (" 
                  << maxMemoryUsage << "%)" << std::endl;
        return false;
    }
    
    if (metrics.responseTime > maxResponseTime) {
        std::cerr << "Performance Warning: Response time (" << metrics.responseTime << "ms) exceeds threshold (" 
                  << maxResponseTime << "ms)" << std::endl;
        return false;
    }
    
    return true;
}

} // namespace performance

} // namespace nlm

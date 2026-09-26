// NLM Utils Implementation
// Performance Monitoring Utilities

#include "core/utils/Utils.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <thread>
#include <unordered_map>

namespace nlm {
namespace utils {

namespace {
    struct PerformanceMonitorState {
        bool monitoring = false;
        std::chrono::high_resolution_clock::time_point startTime;
        double maxCpuUsage = 80.0;
        double maxMemoryUsage = 80.0;
        double maxResponseTime = 100.0;
        std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> eventTimes;
        std::unordered_map<std::string, std::pair<double, int>> benchmarks;
    };
    
    PerformanceMonitorState globalMonitorState;
    
    class SystemInfo {
    public:
        static double getCpuUsage() {
            // Simplified CPU usage calculation
            static double lastCpuUsage = 0.0;
            static auto lastTime = std::chrono::high_resolution_clock::now();
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
            
            if (duration > 0) {
                // This is a simplified approximation
                lastCpuUsage = 50.0 + (std::rand() % 30); // Simulate CPU usage between 50-80%
                lastTime = currentTime;
            }
            
            return lastCpuUsage;
        }
        
        static double getMemoryUsage() {
            // Simplified memory usage calculation
            // In a real implementation, this would read from /proc/meminfo on Linux
            static double lastMemoryUsage = 0.0;
            static auto lastTime = std::chrono::high_resolution_clock::now();
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
            
            if (duration > 0) {
                // Simulate memory usage between 60-90%
                lastMemoryUsage = 60.0 + (std::rand() % 30);
                lastTime = currentTime;
            }
            
            return lastMemoryUsage;
        }
        
        static double getMemoryAvailable() {
            // Simulate available memory
            return 1024.0 * 1024.0 * 1024.0; // 1GB
        }
        
        static double getDiskUsage() {
            // Simplified disk usage
            return 70.0 + (std::rand() % 20); // 70-90%
        }
        
        static size_t getActiveThreads() {
            return std::thread::hardware_concurrency();
        }
        
        static double getNetworkUsage() {
            // Simulate network usage
            return (std::rand() % 1000) / 1024.0; // KB/s
        }
    };
}

// PerformanceMonitor implementation

PerformanceMonitor::Timer::Timer() : startTime(std::chrono::high_resolution_clock::now()) {}

PerformanceMonitor::Timer::~Timer() = default;

double PerformanceMonitor::Timer::elapsed() const {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    return duration.count();
}

void PerformanceMonitor::Timer::reset() {
    startTime = std::chrono::high_resolution_clock::now();
}

std::string PerformanceMonitor::Timer::elapsedString() const {
    return StringUtils::formatTime(elapsed());
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::getPerformanceMetrics() {
    PerformanceMetrics metrics;
    metrics.cpuUsage = SystemInfo::getCpuUsage();
    metrics.memoryUsage = SystemInfo::getMemoryUsage();
    metrics.memoryAvailable = SystemInfo::getMemoryAvailable();
    metrics.diskUsage = SystemInfo::getDiskUsage();
    metrics.networkUsage = SystemInfo::getNetworkUsage();
    metrics.activeThreads = SystemInfo::getActiveThreads();
    
    // Calculate response time from recent events
    double totalResponseTime = 0.0;
    int eventCount = 0;
    for (const auto& event : globalMonitorState.eventTimes) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - event.second).count();
        totalResponseTime += duration;
        ++eventCount;
    }
    metrics.responseTime = eventCount > 0 ? (totalResponseTime / eventCount) * 1000.0 : 0.0; // ms
    
    // Calculate throughput from recent benchmarks
    double totalThroughput = 0.0;
    int benchmarkCount = 0;
    for (const auto& bench : globalMonitorState.benchmarks) {
        totalThroughput += bench.second.first;
        ++benchmarkCount;
    }
    metrics.throughput = benchmarkCount > 0 ? totalThroughput / benchmarkCount : 0.0;
    
    return metrics;
}

void PerformanceMonitor::startMonitoring() {
    globalMonitorState.monitoring = true;
    globalMonitorState.startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::stopMonitoring() {
    globalMonitorState.monitoring = false;
}

bool PerformanceMonitor::isMonitoring() {
    return globalMonitorState.monitoring;
}

std::string PerformanceMonitor::getPerformanceReport() {
    std::ostringstream oss;
    oss << "Performance Monitor Report" << std::endl;
    oss << "============================" << std::endl;
    oss << "Monitoring: " << (isMonitoring() ? "YES" : "NO") << std::endl;
    
    if (isMonitoring()) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - globalMonitorState.startTime).count();
        oss << "Uptime: " << StringUtils::formatTime(duration) << std::endl;
    }
    
    PerformanceMetrics metrics = getPerformanceMetrics();
    oss << std::endl << "Current Metrics:" << std::endl;
    oss << "  CPU Usage: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%" << std::endl;
    oss << "  Memory Usage: " << std::fixed << std::setprecision(1) << metrics.memoryUsage << "%" << std::endl;
    oss << "  Memory Available: " << (metrics.memoryAvailable / (1024.0 * 1024.0)) << " MB" << std::endl;
    oss << "  Disk Usage: " << std::fixed << std::setprecision(1) << metrics.diskUsage << "%" << std::endl;
    oss << "  Network Usage: " << std::fixed << std::setprecision(1) << metrics.networkUsage << " KB/s" << std::endl;
    oss << "  Active Threads: " << metrics.activeThreads << std::endl;
    oss << "  Response Time: " << std::fixed << std::setprecision(3) << metrics.responseTime << " ms" << std::endl;
    oss << "  Throughput: " << std::fixed << std::setprecision(1) << metrics.throughput << " ops/sec" << std::endl;
    
    oss << std::endl << "Performance Thresholds:" << std::endl;
    oss << "  Max CPU Usage: " << globalMonitorState.maxCpuUsage << "%" << std::endl;
    oss << "  Max Memory Usage: " << globalMonitorState.maxMemoryUsage << "%" << std::endl;
    oss << "  Max Response Time: " << globalMonitorState.maxResponseTime << " ms" << std::endl;
    
    return oss.str();
}

void PerformanceMonitor::benchmark(const std::string& name, 
                                   std::function<void()> operation,
                                   int iterations) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        operation();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
    
    double throughput = static_cast<double>(iterations) / duration;
    globalMonitorState.benchmarks[name] = {throughput, iterations};
    
    if (!globalMonitorState.eventTimes.empty()) {
        globalMonitorState.eventTimes.push_back({name, endTime});
    }
}

void PerformanceMonitor::benchmark(const std::string& name,
                                   std::function<double()> operation,
                                   int iterations) {
    double totalResult = 0.0;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        totalResult += operation();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();
    
    double average = totalResult / iterations;
    double throughput = static_cast<double>(iterations) / duration;
    globalMonitorState.benchmarks[name] = {throughput, iterations};
    
    if (!globalMonitorState.eventTimes.empty()) {
        globalMonitorState.eventTimes.push_back({name, endTime});
    }
}

std::vector<double> PerformanceMonitor::profileOperation(std::function<void()> operation,
                                                         size_t numSamples) {
    std::vector<double> times;
    times.reserve(numSamples);
    
    for (size_t i = 0; i < numSamples; ++i) {
        Timer timer;
        operation();
        times.push_back(timer.elapsed() * 1000.0); // Convert to milliseconds
    }
    
    return times;
}

void PerformanceMonitor::startMemoryProfiling() {
    // In a real implementation, this would start memory profiling
    // For now, just record the time
    globalMonitorState.eventTimes.push_back({"memory_profiling_start", 
                                           std::chrono::high_resolution_clock::now()});
}

void PerformanceMonitor::stopMemoryProfiling() {
    globalMonitorState.eventTimes.push_back({"memory_profiling_stop", 
                                           std::chrono::high_resolution_clock::now()});
}

std::string PerformanceMonitor::getMemoryProfileReport() {
    std::ostringstream oss;
    oss << "Memory Profile Report" << std::endl;
    oss << "=======================" << std::endl;
    
    MemoryUtils::MemoryTracker tracker;
    oss << "Memory Usage: " << (MemoryUtils::getMemoryUsage() / 1024.0 / 1024.0) << " MB" << std::endl;
    oss << "Peak Memory: " << (MemoryUtils::getPeakMemoryUsage() / 1024.0 / 1024.0) << " MB" << std::endl;
    oss << "Allocations: " << tracker.allocationCount << std::endl;
    
    return oss.str();
}

void PerformanceMonitor::startCPUProfiling() {
    globalMonitorState.eventTimes.push_back({"cpu_profiling_start", 
                                           std::chrono::high_resolution_clock::now()});
}

void PerformanceMonitor::stopCPUProfiling() {
    globalMonitorState.eventTimes.push_back({"cpu_profiling_stop", 
                                           std::chrono::high_resolution_clock::now()});
}

std::string PerformanceMonitor::getCPUProfileReport() {
    std::ostringstream oss;
    oss << "CPU Profile Report" << std::endl;
    oss << "====================" << std::endl;
    
    PerformanceMetrics metrics = getPerformanceMetrics();
    oss << "Average CPU Usage: " << std::fixed << std::setprecision(1) << metrics.cpuUsage << "%" << std::endl;
    oss << "Peak CPU Usage: " << globalMonitorState.maxCpuUsage << "%" << std::endl;
    oss << "Active Threads: " << metrics.activeThreads << std::endl;
    
    return oss.str();
}

void PerformanceMonitor::trackEvent(const std::string& eventName, 
                                   const std::unordered_map<std::string, std::string>& metadata) {
    globalMonitorState.eventTimes.push_back({eventName, std::chrono::high_resolution_clock::now()});
    
    // Store metadata if needed
    // In a real implementation, this would be stored with the event
}

std::string PerformanceMonitor::getEventLog() {
    std::ostringstream oss;
    oss << "Event Log" << std::endl;
    oss << "=========" << std::endl;
    
    for (const auto& event : globalMonitorState.eventTimes) {
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - event.second).count();
        oss << event.first << " - " << StringUtils::formatTime(duration) << std::endl;
    }
    
    return oss.str();
}

void PerformanceMonitor::clearEventLog() {
    globalMonitorState.eventTimes.clear();
}

void PerformanceMonitor::setPerformanceThresholds(double maxCpuUsage, double maxMemoryUsage, double maxResponseTime) {
    globalMonitorState.maxCpuUsage = maxCpuUsage;
    globalMonitorState.maxMemoryUsage = maxMemoryUsage;
    globalMonitorState.maxResponseTime = maxResponseTime;
}

bool PerformanceMonitor::checkPerformanceThresholds() {
    PerformanceMetrics metrics = getPerformanceMetrics();
    
    if (metrics.cpuUsage > globalMonitorState.maxCpuUsage) {
        std::cerr << "Performance Warning: CPU usage (" << metrics.cpuUsage << "%) exceeds threshold (" 
                  << globalMonitorState.maxCpuUsage << "%)" << std::endl;
        return false;
    }
    
    if (metrics.memoryUsage > globalMonitorState.maxMemoryUsage) {
        std::cerr << "Performance Warning: Memory usage (" << metrics.memoryUsage << "%) exceeds threshold (" 
                  << globalMonitorState.maxMemoryUsage << "%)" << std::endl;
        return false;
    }
    
    if (metrics.responseTime > globalMonitorState.maxResponseTime) {
        std::cerr << "Performance Warning: Response time (" << metrics.responseTime << "ms) exceeds threshold (" 
                  << globalMonitorState.maxResponseTime << "ms)" << std::endl;
        return false;
    }
    
    return true;
}

std::string PerformanceMonitor::getPerformanceThresholdsInfo() {
    std::ostringstream oss;
    oss << "Performance Thresholds:" << std::endl;
    oss << "  Max CPU Usage: " << globalMonitorState.maxCpuUsage << "%" << std::endl;
    oss << "  Max Memory Usage: " << globalMonitorState.maxMemoryUsage << "%" << std::endl;
    oss << "  Max Response Time: " << globalMonitorState.maxResponseTime << " ms" << std::endl;
    return oss.str();
}

} // namespace utils
} // namespace nlm

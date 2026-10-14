#include "PerformanceMonitor.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <iomanip>

namespace nlm {

struct PerformanceMonitor::Impl {
    bool monitoringActive;
    std::chrono::steady_clock::time_point startTime;
    PerformanceMetrics currentMetrics;
    
    // Profiling
    std::map<std::string, ProfileData> profileData;
    std::map<std::string, std::chrono::steady_clock::time_point> profileStartTimes;
    std::mutex profileMutex;
    
    // Benchmarking
    std::vector<BenchmarkResult> benchmarkResults;
    
    // Monitoring thresholds
    double cpuMonitorThreshold;
    double memoryMonitorThreshold;
    
    // Performance alerts
    std::vector<std::string> performanceAlerts;
    
    Impl() : monitoringActive(false), cpuMonitorThreshold(80.0), memoryMonitorThreshold(90.0) {
        startTime = std::chrono::steady_clock::now();
    }
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() = default;

bool PerformanceMonitor::startMonitoring() {
    try {
        if (pImpl->monitoringActive) {
            NLM_LOG_WARNING("Performance monitoring is already active");
            return false;
        }
        
        pImpl->startTime = std::chrono::steady_clock::now();
        pImpl->monitoringActive = true;
        
        NLM_LOG_INFO("Performance monitoring started");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to start performance monitoring: " + std::string(e.what()));
        return false;
    }
}

void PerformanceMonitor::stopMonitoring() {
    if (pImpl->monitoringActive) {
        pImpl->monitoringActive = false;
        
        // Collect final metrics
        pImpl->currentMetrics = collectMetrics();
        
        NLM_LOG_INFO("Performance monitoring stopped");
        NLM_LOG_INFO("  Duration: " + std::to_string(std::chrono::duration<double>(
            std::chrono::steady_clock::now() - pImpl->startTime).count()) + " seconds");
    }
}

bool PerformanceMonitor::isMonitoring() const {
    return pImpl->monitoringActive;
}

PerformanceMonitor::PerformanceMetrics PerformanceMonitor::collectMetrics() {
    PerformanceMetrics metrics;
    
    try {
        // Get system thread count
        metrics.threadCount = std::thread::hardware_concurrency();
        
        // Get memory information (simplified)
        metrics.memoryAllocated = 0; // TODO: Get actual allocated memory
        metrics.memoryUsed = 0;      // TODO: Get actual used memory
        
        // Get CPU usage (simplified)
        metrics.cpuUsage = 0.0;      // TODO: Get actual CPU usage
        
        // Get network and disk I/O (simplified)
        metrics.networkUsage = 0.0;  // TODO: Get actual network usage
        metrics.diskIO = 0.0;         // TODO: Get actual disk I/O
        
        metrics.timestamp = std::chrono::system_clock::now();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error collecting performance metrics: " + std::string(e.what()));
    }
    
    return metrics;
}

void PerformanceMonitor::startProfile(const std::string& functionName) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->profileMutex);
        
        if (pImpl->profileData.find(functionName) == pImpl->profileData.end()) {
            pImpl->profileData[functionName] = ProfileData(functionName);
        }
        
        pImpl->profileStartTimes[functionName] = std::chrono::steady_clock::now();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error starting profile for " + functionName + ": " + std::string(e.what()));
    }
}

PerformanceMonitor::ProfileData PerformanceMonitor::stopProfile(const std::string& functionName) {
    try {
        std::lock_guard<std::mutex> lock(pImpl->profileMutex);
        
        if (pImpl->profileStartTimes.find(functionName) == pImpl->profileStartTimes.end()) {
            NLM_LOG_WARNING("No active profile found for: " + functionName);
            return ProfileData(functionName);
        }
        
        auto startTime = pImpl->profileStartTimes[functionName];
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime);
        
        ProfileData& profile = pImpl->profileData[functionName];
        profile.callCount++;
        profile.totalTime += duration.count();
        profile.duration += duration;
        
        if (duration.count() < profile.minTime) {
            profile.minTime = duration.count();
        }
        if (duration.count() > profile.maxTime) {
            profile.maxTime = duration.count();
        }
        
        if (profile.callCount > 0) {
            profile.averageTime = profile.totalTime / profile.callCount;
        }
        
        // Remove from active profiles
        pImpl->profileStartTimes.erase(functionName);
        
        return profile;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error stopping profile for " + functionName + ": " + std::string(e.what()));
        return ProfileData(functionName);
    }
}

std::map<std::string, PerformanceMonitor::ProfileData> PerformanceMonitor::getProfileData() const {
    return pImpl->profileData;
}

void PerformanceMonitor::resetProfileData() {
    pImpl->profileData.clear();
    pImpl->profileStartTimes.clear();
    
    NLM_LOG_INFO("Profile data reset");
}

PerformanceMonitor::BenchmarkResult PerformanceMonitor::runBenchmark(
    const std::string& name,
    std::function<void()> benchmarkFunction,
    size_t iterations,
    const std::string& description) {
    try {
        BenchmarkResult result(name, description);
        
        // Warm-up
        for (size_t i = 0; i < 10; ++i) {
            benchmarkFunction();
        }
        
        // Actual benchmark run
        auto startTime = std::chrono::steady_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            benchmarkFunction();
        }
        auto endTime = std::chrono::steady_clock::now();
        
        auto elapsed = std::chrono::duration<double>(endTime - startTime);
        result.elapsedTime = elapsed;
        result.iterations = iterations;
        result.operationsPerSecond = (iterations * 1.0) / elapsed.count();
        
        pImpl->benchmarkResults.push_back(result);
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Benchmark completed: " + name);
            NLM_LOG_INFO("  Iterations: " + std::to_string(iterations));
            NLM_LOG_INFO("  Elapsed Time: " + std::to_string(elapsed.count()) + " seconds");
            NLM_LOG_INFO("  Operations/sec: " + std::to_string(result.operationsPerSecond));
        }
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running benchmark " + name + ": " + std::string(e.what()));
        return BenchmarkResult(name, description);
    }
}

PerformanceMonitor::BenchmarkResult PerformanceMonitor::runBenchmarkWithMemory(
    const std::string& name,
    std::function<void()> benchmarkFunction,
    size_t iterations,
    const std::string& description) {
    try {
        BenchmarkResult result(name, description);
        
        // Memory profiling is system-dependent and may not be available on all platforms
        // For now, we'll use a simplified version
        auto startTime = std::chrono::steady_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            benchmarkFunction();
        }
        
        auto endTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(endTime - startTime);
        
        result.elapsedTime = elapsed;
        result.iterations = iterations;
        result.operationsPerSecond = (iterations * 1.0) / elapsed.count();
        
        pImpl->benchmarkResults.push_back(result);
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Benchmark with memory completed: " + name);
            NLM_LOG_INFO("  Memory usage: Not measured (platform dependent)");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error running benchmark with memory " + name + ": " + std::string(e.what()));
        return BenchmarkResult(name, description);
    }
}

std::vector<PerformanceMonitor::BenchmarkResult> PerformanceMonitor::getBenchmarkResults() const {
    return pImpl->benchmarkResults;
}

std::string PerformanceMonitor::generateReport() const {
    std::ostringstream report;
    
    report << "NLM Performance Monitor Report" << std::endl;
    report << "=" << std::string(50, '=') << std::endl;
    report << "Monitoring Active: " << (pImpl->monitoringActive ? "Yes" : "No") << std::endl;
    
    if (pImpl->monitoringActive) {
        auto duration = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - pImpl->startTime);
        report << "Duration: " << duration.count() << " seconds" << std::endl;
    }
    
    report << std::endl;
    report << "Profile Data:" << std::endl;
    for (const auto& profile : pImpl->profileData) {
        report << "  " << profile.first << ":" << std::endl;
        report << "    Calls: " << profile.second.callCount << std::endl;
        report << "    Total Time: " << profile.second.totalTime << "s" << std::endl;
        report << "    Average Time: " << profile.second.averageTime << "s" << std::endl;
        report << "    Min Time: " << profile.second.minTime << "s" << std::endl;
        report << "    Max Time: " << profile.second.maxTime << "s" << std::endl;
    }
    
    report << std::endl;
    report << "Benchmark Results:" << std::endl;
    for (const auto& benchmark : pImpl->benchmarkResults) {
        report << "  " << benchmark.benchmarkName << ":" << std::endl;
        report << "    Description: " << benchmark.description << std::endl;
        report << "    Iterations: " << benchmark.iterations << std::endl;
        report << "    Elapsed Time: " << benchmark.elapsedTime.count() << "s" << std::endl;
        report << "    Operations/sec: " << benchmark.operationsPerSecond << std::endl;
        report << "    Memory Usage: " << benchmark.memoryUsageMB << "MB" << std::endl;
    }
    
    report << std::endl;
    report << "Performance Alerts:" << std::endl;
    for (const auto& alert : pImpl->performanceAlerts) {
        report << "  - " << alert << std::endl;
    }
    
    return report.str();
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath, const std::string& format) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing metrics: " + filepath);
            return false;
        }
        
        if (format == "json") {
            // Export as JSON
            file << "{\n";
            file << "  \"monitoring_active\": " << (pImpl->monitoringActive ? "true" : "false") << ",\n";
            file << "  \"profile_data\": [\n";
            
            bool first = true;
            for (const auto& profile : pImpl->profileData) {
                if (!first) file << ",\n";
                first = false;
                file << "    {\n";
                file << "      \"name\": \"" << profile.first << "\",\n";
                file << "      \"calls\": " << profile.second.callCount << ",\n";
                file << "      \"total_time\": " << profile.second.totalTime << ",\n";
                file << "      \"average_time\": " << profile.second.averageTime << "\n";
                file << "    }";
            }
            
            file << "\n  ],\n";
            file << "  \"benchmark_results\": [\n";
            
            first = true;
            for (const auto& benchmark : pImpl->benchmarkResults) {
                if (!first) file << ",\n";
                first = false;
                file << "    {\n";
                file << "      \"name\": \"" << benchmark.benchmarkName << "\",\n";
                file << "      \"description\": \"" << benchmark.description << "\",\n";
                file << "      \"iterations\": " << benchmark.iterations << ",\n";
                file << "      \"elapsed_time\": " << benchmark.elapsedTime.count() << ",\n";
                file << "      \"operations_per_second\": " << benchmark.operationsPerSecond << ",\n";
                file << "      \"memory_usage_mb\": " << benchmark.memoryUsageMB << "\n";
                file << "    }";
            }
            
            file << "\n  ],\n";
            file << "  \"performance_alerts\": [\n";
            
            first = true;
            for (const auto& alert : pImpl->performanceAlerts) {
                if (!first) file << ",\n";
                first = false;
                file << "    \"" << alert << "\"";
            }
            
            file << "\n  ]\n";
            file << "}\n";
        } else {
            // Default to text format
            file << generateReport();
        }
        
        file.close();
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Performance metrics exported to: " + filepath + " (format: " + format + ")");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error exporting performance metrics to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool PerformanceMonitor::importMetrics(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading metrics: " + filepath);
            return false;
        }
        
        // TODO: Implement metrics import (JSON parsing)
        // For now, just read the file and close
        std::string line;
        while (std::getline(file, line)) {
            // Parse line (simplified)
        }
        
        file.close();
        
        if (pImpl->debugEnabled) {
            NLM_LOG_INFO("Performance metrics imported from: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing performance metrics from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void PerformanceMonitor::setCPUMonitorThreshold(double threshold) {
    pImpl->cpuMonitorThreshold = threshold;
}

void PerformanceMonitor::setMemoryMonitorThreshold(double threshold) {
    pImpl->memoryMonitorThreshold = threshold;
}

double PerformanceMonitor::getCPUMonitorThreshold() const {
    return pImpl->cpuMonitorThreshold;
}

double PerformanceMonitor::getMemoryMonitorThreshold() const {
    return pImpl->memoryMonitorThreshold;
}

bool PerformanceMonitor::isCPUMonitorExceeded() const {
    // TODO: Get actual CPU usage
    double cpuUsage = 0.0;  // Placeholder
    return cpuUsage > pImpl->cpuMonitorThreshold;
}

bool PerformanceMonitor::isMemoryMonitorExceeded() const {
    // TODO: Get actual memory usage
    double memoryUsage = 0.0;  // Placeholder
    return memoryUsage > pImpl->memoryMonitorThreshold;
}

std::string PerformanceMonitor::getSystemInfo() const {
    std::ostringstream info;
    
    info << "System Information:" << std::endl;
    info << "  CPU Cores: " << std::thread::hardware_concurrency() << std::endl;
    info << "  Monitoring Active: " << (pImpl->monitoringActive ? "Yes" : "No") << std::endl;
    
    if (pImpl->monitoringActive) {
        auto duration = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - pImpl->startTime);
        info << "  Monitoring Duration: " << duration.count() << " seconds" << std::endl;
    }
    
    return info.str();
}

std::vector<std::string> PerformanceMonitor::getPerformanceAlerts() const {
    return pImpl->performanceAlerts;
}

void PerformanceMonitor::clearPerformanceAlerts() {
    pImpl->performanceAlerts.clear();
    
    NLM_LOG_INFO("Performance alerts cleared");
}

} // namespace nlm

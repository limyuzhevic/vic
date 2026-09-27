#include "PerformanceLogger.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <algorithm>

namespace nlm {

struct PerformanceLogger::Impl {
    PerformanceLogConfig config;
    std::mutex mutex;
    
    // Performance metrics
    struct OperationMetrics {
        double totalTimeMs = 0.0;
        double minTimeMs = std::numeric_limits<double>::max();
        double maxTimeMs = 0.0;
        size_t callCount = 0;
        double avgTimeMs() const { return callCount > 0 ? totalTimeMs / callCount : 0.0; }
        double stdDevTimeMs() const {
            if (callCount <= 1) return 0.0;
            double variance = 0.0;
            // In a real implementation, we would store all times
            return std::sqrt(variance / (callCount - 1));
        }
    };
    
    std::unordered_map<std::string, OperationMetrics> operationMetrics;
    
    // Resource metrics
    struct ResourceMetrics {
        double cpuUsagePercent = 0.0;
        size_t memoryMB = 0;
        size_t allocatedBytes = 0;
        time_t timestamp;
    };
    
    std::vector<ResourceMetrics> resourceHistory;
};

PerformanceLogger::PerformanceLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default config
    pImpl->config = PerformanceLogConfig();
}

PerformanceLogger::~PerformanceLogger() = default;

void PerformanceLogger::configure(const PerformanceLogConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

PerformanceLogConfig PerformanceLogger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

PerformanceLogger::Timer::Timer(PerformanceLogger& logger, const std::string& operation,
                               const std::unordered_map<std::string, std::string>& tags)
    : logger_(&logger), operationName_(operation), tags_(tags) {
    startTime_ = std::chrono::system_clock::now();
}

PerformanceLogger::Timer::~Timer() {
    if (!stopped_) {
        stop();
    }
}

void PerformanceLogger::Timer::stop() {
    if (!stopped_) {
        auto endTime = std::chrono::system_clock::now();
        durationMs_ = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
        stopped_ = true;
        
        if (logger_) {
            logger_->logMetrics(durationMs_, operationName_, tags_);
        }
    }
}

PerformanceLogger::Timer PerformanceLogger::startTiming(const std::string& operation,
                                                         const std::unordered_map<std::string, std::string>& tags) {
    return Timer(*this, operation, tags);
}

void PerformanceLogger::logMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update operation metrics
    auto& opMetrics = pImpl->operationMetrics[metrics.operationName];
    opMetrics.totalTimeMs += metrics.executionTimeMs;
    opMetrics.minTimeMs = std::min(opMetrics.minTimeMs, metrics.executionTimeMs);
    opMetrics.maxTimeMs = std::max(opMetrics.maxTimeMs, metrics.executionTimeMs);
    opMetrics.callCount++;
    
    // Log performance metrics
    EnhancedLogger::getGlobal().log(LogLevel::PerformanceEnd, "Performance metrics", "", 0, "",
                                   {{ "operation", metrics.operationName },
                                    { "execution_time_ms", std::to_string(metrics.executionTimeMs) },
                                    { "memory_used", std::to_string(metrics.memoryUsed) },
                                    { "cpu_usage", std::to_string(metrics.cpuUsagePercent) },
                                    { "cache_hits", std::to_string(metrics.cacheHits) },
                                    { "cache_misses", std::to_string(metrics.cacheMisses) },
                                    { "cache_hit_rate", std::to_string(metrics.getCacheHitRate()) }});
    
    // Check for performance issues
    if (config_.enableProfiling && metrics.executionTimeMs > config_.slowOperationThresholdMs) {
        std::string severity = metrics.executionTimeMs > config_.verySlowOperationThresholdMs ? 
            "CRITICAL" : "WARNING";
        EnhancedLogger::getGlobal().log(severity == "CRITICAL" ? LogLevel::Critical : LogLevel::Warning,
                                      "Slow operation detected", "", 0, "",
                                      {{ "operation", metrics.operationName },
                                       { "execution_time_ms", std::to_string(metrics.executionTimeMs) },
                                       { "severity", severity }});
    }
}

void PerformanceLogger::logMetrics(double executionTimeMs, const std::string& operation,
                                   const std::unordered_map<std::string, std::string>& tags) {
    PerformanceMetrics metrics;
    metrics.operationName = operation;
    metrics.executionTimeMs = executionTimeMs;
    metrics.timestamp = std::time(nullptr);
    metrics.tags = tags;
    
    logMetrics(metrics);
}

void PerformanceLogger::logThroughput(const std::string& operation, size_t operationsCount,
                                      double timeMs, size_t itemsPerSecond) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (itemsPerSecond == 0 && timeMs > 0) {
        itemsPerSecond = static_cast<size_t>((static_cast<double>(operationsCount) / timeMs) * 1000.0);
    }
    
    EnhancedLogger::getGlobal().log(LogLevel::Throughput, "Throughput metrics", "", 0, "",
                                   {{ "operation", operation },
                                    { "operations_count", std::to_string(operationsCount) },
                                    { "time_ms", std::to_string(timeMs) },
                                    { "items_per_second", std::to_string(itemsPerSecond) },
                                    { "items_per_ms", std::to_string(operationsCount / timeMs) }});
}

void PerformanceLogger::logCPUUsage(double cpuUsagePercent, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ResourceMetrics resource;
    resource.cpuUsagePercent = cpuUsagePercent;
    resource.timestamp = std::time(nullptr);
    resource.allocationBytes = 0; // Would be set by system monitoring
    
    pImpl->resourceHistory.push_back(resource);
    
    EnhancedLogger::getGlobal().log(LogLevel::ResourceUsage, "CPU usage", "", 0, "",
                                   {{ "cpu_percent", std::to_string(cpuUsagePercent) },
                                    { "context", context }});
    
    // Check for CPU threshold
    if (config_.enableProfiling && cpuUsagePercent > config_.cpuThresholdPercent) {
        EnhancedLogger::getGlobal().log(LogLevel::Warning, "High CPU usage detected", "", 0, "",
                                      {{ "cpu_percent", std::to_string(cpuUsagePercent) },
                                       { "threshold_percent", std::to_string(config_.cpuThresholdPercent) }});
    }
}

void PerformanceLogger::logMemoryUsage(size_t memoryMB, const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    ResourceMetrics resource;
    resource.memoryMB = memoryMB;
    resource.timestamp = std::time(nullptr);
    
    pImpl->resourceHistory.push_back(resource);
    
    EnhancedLogger::getGlobal().log(LogLevel::ResourceUsage, "Memory usage", "", 0, "",
                                   {{ "memory_mb", std::to_string(memoryMB) },
                                    { "context", context }});
    
    // Check for memory threshold
    if (config_.enableProfiling && memoryMB > config_.memoryThresholdMB) {
        EnhancedLogger::getGlobal().log(LogLevel::Warning, "High memory usage detected", "", 0, "",
                                      {{ "memory_mb", std::to_string(memoryMB) },
                                       { "threshold_mb", std::to_string(config_.memoryThresholdMB) }});
    }
}

void PerformanceLogger::logDiskUsage(const std::string& mountPoint, double usedMB, double totalMB) {
    double usagePercent = totalMB > 0 ? (usedMB / totalMB) * 100.0 : 0.0;
    
    EnhancedLogger::getGlobal().log(LogLevel::ResourceUsage, "Disk usage", "", 0, "",
                                   {{ "mount_point", mountPoint },
                                    { "used_mb", std::to_string(usedMB) },
                                    { "total_mb", std::to_string(totalMB) },
                                    { "usage_percent", std::to_string(usagePercent) }});
}

void PerformanceLogger::logNetworkUsage(double uploadMBps, double downloadMBps) {
    EnhancedLogger::getGlobal().log(LogLevel::ResourceUsage, "Network usage", "", 0, "",
                                   {{ "upload_mbps", std::to_string(uploadMBps) },
                                    { "download_mbps", std::to_string(downloadMBps) }});
}

void PerformanceLogger::logFunctionProfile(const std::string& functionName,
                                         double executionTimeMs,
                                         size_t memoryDelta,
                                         int callCount,
                                         const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Function profile", "", 0, "",
                                   {{ "function_name", functionName },
                                    { "execution_time_ms", std::to_string(executionTimeMs) },
                                    { "memory_delta", std::to_string(memoryDelta) },
                                    { "call_count", std::to_string(callCount) }});
    
    // Update operation metrics
    auto& opMetrics = pImpl->operationMetrics[functionName];
    opMetrics.totalTimeMs += executionTimeMs;
    opMetrics.minTimeMs = std::min(opMetrics.minTimeMs, executionTimeMs);
    opMetrics.maxTimeMs = std::max(opMetrics.maxTimeMs, executionTimeMs);
    opMetrics.callCount += callCount;
}

void PerformanceLogger::logThreadProfile(const std::string& threadName,
                                        double cpuTimeMs, double wallTimeMs,
                                        size_t stackSize,
                                        const std::unordered_map<std::string, std::string>& metadata) {
    double cpuUtilization = wallTimeMs > 0 ? (cpuTimeMs / wallTimeMs) * 100.0 : 0.0;
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Thread profile", "", 0, "",
                                   {{ "thread_name", threadName },
                                    { "cpu_time_ms", std::to_string(cpuTimeMs) },
                                    { "wall_time_ms", std::to_string(wallTimeMs) },
                                    { "cpu_utilization", std::to_string(cpuUtilization) },
                                    { "stack_size", std::to_string(stackSize) }});
}

void PerformanceLogger::logPerformanceRegression(const std::string& operation,
                                                 double baselineTimeMs,
                                                 double currentTimeMs,
                                                 double regressionPercent,
                                                 const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::Warning, "Performance regression detected", "", 0, "",
                                   {{ "operation", operation },
                                    { "baseline_time_ms", std::to_string(baselineTimeMs) },
                                    { "current_time_ms", std::to_string(currentTimeMs) },
                                    { "regression_percent", std::to_string(regressionPercent) },
                                    { "context", context }});
}

void PerformanceLogger::logPerformanceImprovement(const std::string& operation,
                                                  double baselineTimeMs,
                                                  double currentTimeMs,
                                                  double improvementPercent,
                                                  const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::Info, "Performance improvement detected", "", 0, "",
                                   {{ "operation", operation },
                                    { "baseline_time_ms", std::to_string(baselineTimeMs) },
                                    { "current_time_ms", std::to_string(currentTimeMs) },
                                    { "improvement_percent", std::to_string(improvementPercent) },
                                    { "context", context }});
}

void PerformanceLogger::logPerformanceAlert(const std::string& alertType,
                                           const std::string& message,
                                           double severity,
                                           const std::unordered_map<std::string, std::string>& metadata) {
    std::string level = severity >= 2.0 ? "CRITICAL" : severity >= 1.5 ? "ERROR" : "WARNING";
    LogLevel logLevel = (severity >= 2.0 ? LogLevel::Critical : severity >= 1.5 ? LogLevel::Error : LogLevel::Warning);
    
    EnhancedLogger::getGlobal().log(logLevel, "Performance alert: " + message, "", 0, "",
                                   {{ "alert_type", alertType },
                                    { "message", message },
                                    { "severity", std::to_string(severity) }});
}

double PerformanceLogger::getAverageOperationTime(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->operationMetrics.find(operation);
    if (it != pImpl->operationMetrics.end()) {
        return it->second.avgTimeMs();
    }
    return 0.0;
}

 size_t PerformanceLogger::getOperationCount(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->operationMetrics.find(operation);
    if (it != pImpl->operationMetrics.end()) {
        return it->second.callCount;
    }
    return 0;
}

 size_t PerformanceLogger::getTotalMemoryUsed() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t totalMemory = 0;
    for (const auto& resource : pImpl->resourceHistory) {
        totalMemory += resource.memoryMB * 1024 * 1024; // Convert MB to bytes
    }
    return totalMemory;
}

 double PerformanceLogger::getTotalCPUTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    double totalCPUTime = 0.0;
    for (const auto& resource : pImpl->resourceHistory) {
        totalCPUTime += resource.cpuUsagePercent;
    }
    return totalCPUTime;
}

 void PerformanceLogger::exportToCSV(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return;
    }
    
    outFile << "Operation,TotalTimeMs,MinTimeMs,MaxTimeMs,CallCount,AvgTimeMs" << std::endl;
    for (const auto& pair : pImpl->operationMetrics) {
        const auto& metrics = pair.second;
        outFile << pair.first << ","
                << metrics.totalTimeMs << ","
                << metrics.minTimeMs << ","
                << metrics.maxTimeMs << ","
                << metrics.callCount << ","
                << metrics.avgTimeMs() << std::endl;
    }
    
    outFile.close();
}

void PerformanceLogger::exportToJSON(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return;
    }
    
    outFile << "{\n  \"operationMetrics\": [" << std::endl;
    bool first = true;
    for (const auto& pair : pImpl->operationMetrics) {
        const auto& metrics = pair.second;
        if (!first) outFile << "," << std::endl;
        outFile << "    {\"operation\": \"" << pair.first << "\", \"totalTimeMs\": " << metrics.totalTimeMs
                << ", \"minTimeMs\": " << metrics.minTimeMs
                << ", \"maxTimeMs\": " << metrics.maxTimeMs
                << ", \"callCount\": " << metrics.callCount
                << ", \"avgTimeMs\": " << metrics.avgTimeMs() << "}";
        first = false;
    }
    outFile << std::endl << "  ],\n  \"resourceHistory\": [" << std::endl;
    first = true;
    for (const auto& resource : pImpl->resourceHistory) {
        if (!first) outFile << "," << std::endl;
        outFile << "    {\"timestamp\": " << resource.timestamp
                << ", \"cpuUsagePercent\": " << resource.cpuUsagePercent
                << ", \"memoryMB\": " << resource.memoryMB << "}";
        first = false;
    }
    outFile << std::endl << "  ]\n}" << std::endl;
    
    outFile.close();
}

std::string PerformanceLogger::exportToString(const std::string& format) const {
    std::ostringstream oss;
    if (format == "json") {
        oss << "{\n  \"operationMetrics\": [" << std::endl;
        bool first = true;
        for (const auto& pair : pImpl->operationMetrics) {
            const auto& metrics = pair.second;
            if (!first) oss << "," << std::endl;
            oss << "    {\"operation\": \"" << pair.first << "\", \"totalTimeMs\": " << metrics.totalTimeMs
                << ", \"minTimeMs\": " << metrics.minTimeMs
                << ", \"maxTimeMs\": " << metrics.maxTimeMs
                << ", \"callCount\": " << metrics.callCount
                << ", \"avgTimeMs\": " << metrics.avgTimeMs() << "}";
            first = false;
        }
        oss << std::endl << "  ]\n}" << std::endl;
    } else if (format == "csv") {
        oss << "Operation,TotalTimeMs,MinTimeMs,MaxTimeMs,CallCount,AvgTimeMs" << std::endl;
        for (const auto& pair : pImpl->operationMetrics) {
            const auto& metrics = pair.second;
            oss << pair.first << ","
                << metrics.totalTimeMs << ","
                << metrics.minTimeMs << ","
                << metrics.maxTimeMs << ","
                << metrics.callCount << ","
                << metrics.avgTimeMs() << std::endl;
        }
    }
    return oss.str();
}

PerformanceLogger& PerformanceLogger::getGlobal() {
    static PerformanceLogger globalLogger;
    return globalLogger;
}

void PerformanceLogger::setGlobal(std::shared_ptr<PerformanceLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

} // namespace nlm

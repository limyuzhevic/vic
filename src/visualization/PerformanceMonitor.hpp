#pragma once

#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>

namespace nlm {

// Performance monitoring and benchmarking
class PerformanceMonitor {
public:
    struct ProcessMetrics {
        double cpuUsage;
        double memoryUsage;
        double gpuUsage;
        uint64_t memoryAllocated;
        uint64_t memoryPeak;
        double networkThroughput;
        double simulationFPS;
        double visualizationFPS;
        std::chrono::system_clock::time_point timestamp;
    };

    struct BenchmarkResult {
        std::string benchmarkName;
        double executionTime;
        double memoryUsage;
        double cpuUsage;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, double> metrics;
    };

    struct ResourceUsage {
        double cpuUser;
        double cpuSystem;
        uint64_t memoryTotal;
        uint64_t memoryUsed;
        uint64_t memoryAvailable;
        double diskReadSpeed;
        double diskWriteSpeed;
        double networkUploadSpeed;
        double networkDownloadSpeed;
    };

    PerformanceMonitor(std::shared_ptr<VisualizationManager> manager);
    ~PerformanceMonitor();

    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;

    // Resource monitoring
    bool startResourceMonitoring();
    void stopResourceMonitoring();
    bool isResourceMonitoringActive() const;
    ResourceUsage getCurrentResourceUsage() const;
    std::vector<ResourceUsage> getResourceUsageHistory(size_t limit = 1000) const;

    // Process metrics
    void startProcessMetrics();
    void stopProcessMetrics();
    bool isProcessMetricsActive() const;
    ProcessMetrics getCurrentProcessMetrics() const;
    std::vector<ProcessMetrics> getProcessMetricsHistory(size_t limit = 1000) const;

    // Performance benchmarking
    bool startBenchmarking(const std::string& benchmarkName);
    void stopBenchmarking(const std::string& benchmarkName);
    bool isBenchmarking(const std::string& benchmarkName) const;
    BenchmarkResult runBenchmark(const std::string& benchmarkName, std::function<void()> benchmarkFunction);
    std::vector<BenchmarkResult> getBenchmarkResults(const std::string& benchmarkName = "") const;

    // Performance analysis
    void analyzePerformanceTrends();
    void generatePerformanceReport(const std::string& reportPath);
    json getPerformanceStatistics() const;
    json getResourceStatistics() const;

    // Performance alerting
    bool enablePerformanceAlerting();
    void disablePerformanceAlerting();
    bool isAlertingEnabled() const;
    void setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds);
    void addPerformanceAlert(const std::string& metricName, double threshold, const std::string& message);

    // Integration with visualization
    void integrateWithVisualization(std::shared_ptr<VisualizationManager> manager);

    // Configuration
    void setMonitoringInterval(double interval);
    void setHistorySize(size_t size);
    void setAlertThresholds(const std::unordered_map<std::string, double>& thresholds);
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration() const;
    std::string getConfigurationJSON() const;

    // Control
    void pauseMonitoring();
    void resumeMonitoring();
    bool isMonitoringPaused() const;
    void resetMetrics();

    // Export and import
    bool exportMetrics(const std::string& filepath);
    bool importMetrics(const std::string& filepath);
    json getAllMetrics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

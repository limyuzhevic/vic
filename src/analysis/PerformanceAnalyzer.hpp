#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

struct PerformanceMetrics {
    double cpuUsage; // CPU utilization percentage
    double memoryUsage; // Memory utilization (MB)
    double networkUsage; // Network bandwidth usage
    double renderingFrameTime; // Time per frame (ms)
    double updateTime; // Time per update (ms)
    double renderTime; // Time per render (ms)
    double fps; // Frames per second
    double dataProcessingTime; // Data processing time (ms)
    std::map<std::string, double> customMetrics; // Custom metrics
};

struct PerformanceData {
    std::vector<PerformanceMetrics> metrics; // Time series of performance metrics
    std::vector<Timestamp> timestamps; // Corresponding timestamps
    std::string sessionId; // Session identifier
    std::string benchmarkId; // Benchmark identifier
};

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer() = default;
    
    // Add performance data
    void addData(const PerformanceData& data);
    
    // Add performance metrics
    void addMetrics(const PerformanceMetrics& metrics);
    
    // Calculate performance statistics
    PerformanceMetrics calculateStatistics(const PerformanceData& data) const;
    
    // Calculate CPU performance
    double calculateCPUPerformance(const PerformanceData& data) const;
    
    // Calculate memory performance
    double calculateMemoryPerformance(const PerformanceData& data) const;
    
    // Calculate rendering performance
    double calculateRenderingPerformance(const PerformanceData& data) const;
    
    // Calculate data processing performance
    double calculateDataProcessingPerformance(const PerformanceData& data) const;
    
    // Calculate scalability
    double calculateScalability(const PerformanceData& data) const;
    
    // Calculate performance bottlenecks
    std::vector<std::string> calculatePerformanceBottlenecks(const PerformanceData& data) const;
    
    // Predict future performance
    PerformanceMetrics predictPerformance(const PerformanceData& data, double timeHorizon) const;
    
    // Compare performance with baseline
    double compareWithBaseline(const PerformanceData& data, const PerformanceData& baseline) const;
    
    // Get performance data
    const PerformanceData& getData() const;
    
    // Clear performance data
    void clearData();
    
    // Export analysis results
    bool exportResults(const std::string& filename, const std::string& format = "json") const;
    
    // Import analysis results
    bool importResults(const std::string& filename, const std::string& format = "json");
    
    // Generate performance report
    std::string generateReport() const;
    
    // Check if analyzer has data
    bool hasData() const;
    
    // Get analysis timestamp
    Timestamp getTimestamp() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
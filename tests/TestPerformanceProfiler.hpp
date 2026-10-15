// Test Performance Profiler
// Performance monitoring during test execution

#ifndef TEST_PERFORMANCE_PROFILER_HPP
#define TEST_PERFORMANCE_PROFILER_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {
namespace tests {

// Performance measurement types
enum class PerformanceMetricType {
    TIME,
    MEMORY,
    CPU,
    BANDWIDTH,
    THROUGHPUT,
    LATENCY,
    CUSTOM
};

// Performance measurement result
struct PerformanceMeasurement {
    std::string metricName;
    PerformanceMetricType metricType;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::milliseconds duration;
    std::map<std::string, double> parameters;
    bool isActive;
    
    PerformanceMeasurement(const std::string& name, PerformanceMetricType type)
        : metricName(name), metricType(type), isActive(false) {}
};

// Performance statistics
struct PerformanceStatistics {
    std::string metricName;
    PerformanceMetricType metricType;
    double averageValue;
    double minValue;
    double maxValue;
    double medianValue;
    std::vector<double> values;
    size_t measurementCount;
    
    PerformanceStatistics(const std::string& name, PerformanceMetricType type)
        : metricName(name), metricType(type), averageValue(0.0), minValue(0.0), maxValue(0.0), medianValue(0.0), measurementCount(0) {}
};

// Performance profiler for tests
class TestPerformanceProfiler {
public:
    TestPerformanceProfiler();
    ~TestPerformanceProfiler();
    
    // Metric management
    bool registerMetric(const std::string& name, PerformanceMetricType type = PerformanceMetricType::TIME);
    bool unregisterMetric(const std::string& name);
    bool updateMetricParameters(const std::string& name, const std::map<std::string, double>& params);
    
    // Performance measurement
    bool startMeasurement(const std::string& metricName);
    bool stopMeasurement(const std::string& metricName);
    bool pauseMeasurement(const std::string& metricName);
    bool resumeMeasurement(const std::string& metricName);
    bool cancelMeasurement(const std::string& metricName);
    
    // Data access
    double getMeasurement(const std::string& metricName) const;
    std::chrono::milliseconds getTimeMeasurement(const std::string& metricName) const;
    PerformanceMeasurement getCurrentMeasurement(const std::string& metricName) const;
    std::vector<PerformanceMeasurement> getAllMeasurements() const;
    
    // Statistics
    PerformanceStatistics getStatistics(const std::string& metricName) const;
    std::map<std::string, PerformanceStatistics> getAllStatistics() const;
    
    // Specialized measurement functions
    void profilePluginLoading(const std::string& pluginName);
    void profileSimulation(const std::string& simulationId);
    void profileMemoryUsage(const std::string& memoryType);
    void profileNetworkScaling(const std::string& networkId);
    void profileExperimentExecution(const std::string& experimentName);
    
    // Reporting
    std::string generatePerformanceReport() const;
    std::string generateDetailedPerformanceReport() const;
    std::string generateBenchmarkReport() const;
    std::string generateOptimizationReport() const;
    
    // File operations
    bool exportPerformanceData(const std::string& filename, const std::string& format = "csv");
    bool exportStatistics(const std::string& filename, const std::string& format = "json");
    bool importPerformanceData(const std::string& filename);
    
    // Analysis and optimization
    std::string analyzePerformanceBottlenecks() const;
    std::string identifyOptimizationOpportunities() const;
    double calculatePerformanceScore() const;
    std::vector<std::string> getRecommendedOptimizations() const;
    
    // Configuration
    void enableRealTimeReporting(bool enable);
    void setReportingInterval(std::chrono::milliseconds interval);
    void setThresholds(const std::map<std::string, double>& thresholds);
    
    // Control
    void reset();
    void clear();
    void pauseAllMeasurements();
    void resumeAllMeasurements();
    bool isMeasurementActive(const std::string& metricName) const;
    
    // Singleton pattern
    static TestPerformanceProfiler& getInstance();
    static void initialize();
    static void shutdown();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor and destructor
    TestPerformanceProfiler(const TestPerformanceProfiler&) = delete;
    TestPerformanceProfiler& operator=(const TestPerformanceProfiler&) = delete;
    
    // Helper functions
    double calculateMedian(const std::vector<double>& values) const;
    double calculateStdDev(const std::vector<double>& values, double mean) const;
    std::string formatMetricValue(double value, PerformanceMetricType type) const;
    std::string getMetricTypeString(PerformanceMetricType type) const;
};

} // namespace tests
} // namespace nlm

#endif // TEST_PERFORMANCE_PROFILER_HPP

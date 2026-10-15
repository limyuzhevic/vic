// NLM Advanced Debugging and Profiling Tools - Performance Profiler

#pragma once

#include "core/Logger/Logger.hpp"
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>

namespace nlm {

// Performance metric record
struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::string component;
    std::map<std::string, std::string> tags;
    
    PerformanceMetric(const std::string& name, double value, const std::string& unit = "")
        : name(name), value(value), unit(unit), timestamp(std::chrono::system_clock::now()) {}
};

// Profiling session
struct ProfilingSession {
    std::string id;
    std::string name;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<PerformanceMetric> metrics;
    std::map<std::string, double> cpuTimeByOperation;
    size_t memoryAllocated = 0;
    size_t memoryDeallocated = 0;
    
    ProfilingSession(const std::string& name) : name(name), startTime(std::chrono::system_clock::now()) {}
    
    double getDuration() const {
        auto end = endTime.time_since_epoch().count() > 0 ? endTime : std::chrono::system_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    }
    
    std::string getDurationString() const {
        double duration = getDuration();
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << duration << "s";
        return oss.str();
    }
};

// Performance profiler configuration
struct PerformanceProfilerConfig {
    bool enableCPUProfiling = true;
    bool enableMemoryProfiling = true;
    bool enableSystemProfiling = true;
    bool enableTimeProfiling = true;
    bool enableIProfiling = false;
    std::chrono::duration<double> profilingInterval = std::chrono::duration<double>(0.1); // 100ms
    size_t maxMetricsHistory = 100000;
    bool recordStackTraces = false;
    std::string outputDirectory = "performance_data";
    std::string metricsExportFormat = "csv";
    bool autoStartProfiling = false;
    bool autoStopProfiling = true;
};

// Performance profiler class
class PerformanceProfiler {
public:
    PerformanceProfiler(const PerformanceProfilerConfig& config = PerformanceProfilerConfig());
    ~PerformanceProfiler();
    
    // Profiling control
    void startProfile(const std::string& operation);
    void stopProfile(const std::string& operation);
    void startSystemProfiling();
    void stopSystemProfiling();
    void pauseProfiling();
    void resumeProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Metric recording
    void recordMetric(const std::string& name, double value, const std::string& unit = "",
                     const std::string& component = "", const std::map<std::string, std::string>& tags = {});
    void recordCPUTime(const std::string& operation, double cpuTime);
    void recordMemoryStats(size_t allocated, size_t deallocated);
    void recordSystemMetric(const std::string& name, double value, const std::string& component = "");
    
    // Metric retrieval
    const std::vector<PerformanceMetric>& getMetrics(const std::string& name = "") const;
    double getMetricMean(const std::string& name) const;
    double getMetricStdDev(const std::string& name) const;
    double getMetricMin(const std::string& name) const;
    double getMetricMax(const std::string& name) const;
    
    // Session management
    std::string createProfilingSession(const std::string& name);
    void endProfilingSession(const std::string& sessionId);
    ProfilingSession* getProfilingSession(const std::string& sessionId);
    std::vector<std::string> getActiveSessionIds() const;
    
    // Performance analysis
    std::string getPerformanceReport(const std::string& sessionId = "") const;
    std::string getCPUPerformanceReport() const;
    std::string getMemoryPerformanceReport() const;
    std::string getSystemPerformanceReport() const;
    
    // Bottleneck detection
    std::vector<std::string> detectCPU bottlenecks() const;
    std::vector<std::string> detectMemoryBottlenecks() const;
    std::vector<std::string> detectSystemBottlenecks() const;
    
    // Export functionality
    bool exportMetrics(const std::string& filename, const std::string& format = "csv");
    bool exportProfilingData(const std::string& filename, const std::string& format = "json");
    bool exportSessions(const std::string& filename, const std::string& format = "xml");
    
    // Configuration
    void updateConfig(const PerformanceProfilerConfig& config);
    const PerformanceProfilerConfig& getConfig() const { return config_; }
    
    // Integration
    void attachToBrain(const std::string& brainId);
    void attachToExperiment(const std::string& experimentName);
    
    // Statistics
    size_t getMetricCount(const std::string& name = "") const;
    size_t getSessionCount() const;
    std::string getProfilerStatistics() const;
    
    // Event handling
    void setProfilingStartCallback(std::function<void(const std::string&)> callback);
    void setProfilingStopCallback(std::function<void(const std::string&, double)> callback);
    void setMetricRecordedCallback(std::function<void(const PerformanceMetric&)> callback);
    
private:
    std::atomic<bool> isProfiling_;
    mutable std::mutex mutex_;
    
    PerformanceProfilerConfig config_;
    
    // Metric storage
    std::map<std::string, std::vector<PerformanceMetric>> metrics_;
    std::vector<PerformanceMetric> metricsHistory_;
    
    // Profiling sessions
    std::map<std::string, ProfilingSession> sessions_;
    std::map<std::string, std::chrono::system_clock::time_point> activeSessions_;
    
    // System metrics
    std::map<std::string, std::vector<double>> systemMetrics_;
    
    // Callbacks
    std::function<void(const std::string&)> profilingStartCallback_;
    std::function<void(const std::string&, double)> profilingStopCallback_;
    std::function<void(const PerformanceMetric&)> metricRecordedCallback_;
    
    // Internal methods
    void recordSystemMetrics();
    void cleanupOldMetrics();
    void processProfilingCallbacks(const std::string& operation, double duration);
    std::vector<double> getMetricValues(const std::string& name) const;
    std::string formatMetricValue(double value, const std::string& unit) const;
    std::string getProfilerTimestamp() const;
    std::string generateCSVExport(const std::string& filename) const;
    std::string generateJSONExport(const std::string& filename) const;
    std::string generateXMLEmport(const std::string& filename) const;
    std::vector<std::string> detectPerformanceBottlenecks() const;
};

} // namespace nlm

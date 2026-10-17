#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double memoryPeak;
    uint64_t simulationSteps;
    double simulationTime;
    double wallClockTime;
    uint64_t spikeCount;
    double firingRate;
    uint64_t totalNeurons;
    uint64_t totalSynapses;
    double plasticityTime;
    double visualizationTime;
    std::map<std::string, double> customMetrics;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryPeak(0.0),
                          simulationSteps(0), simulationTime(0.0), wallClockTime(0.0),
                          spikeCount(0), firingRate(0.0), totalNeurons(0), totalSynapses(0),
                          plasticityTime(0.0), visualizationTime(0.0) {}
};

// Profiling session structure
struct ProfilingSession {
    std::string sessionName;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::vector<PerformanceMetrics> metricsHistory;
    std::map<std::string, double> accumulatedMetrics;
    bool isActive;
    
    ProfilingSession(const std::string& name) : sessionName(name), isActive(false) {}
};

// Performance analyzer interface
class PerformanceAnalyzer {
public:
    virtual ~PerformanceAnalyzer() = default;
    
    // Start performance profiling
    virtual bool startProfiling(const std::string& sessionName) = 0;
    
    // Stop profiling and collect results
    virtual std::shared_ptr<PerformanceMetrics> stopProfiling(const std::string& sessionName) = 0;
    
    // Get current performance metrics
    virtual std::shared_ptr<PerformanceMetrics> getCurrentMetrics() const = 0;
    
    // Get profiling session data
    virtual std::vector<std::shared_ptr<PerformanceMetrics>> getSessionData(const std::string& sessionName) const = 0;
    
    // Get all session names
    virtual std::vector<std::string> getSessionNames() const = 0;
    
    // Calculate performance statistics
    virtual std::map<std::string, double> calculateStatistics(const std::string& sessionName) const = 0;
    
    // Generate performance report
    virtual std::string generateReport(const std::string& sessionName, 
                                      const std::string& format = "text") const = 0;
};

// System resource monitor
class SystemResourceMonitor {
public:
    virtual ~SystemResourceMonitor() = default;
    
    // Get CPU usage (percentage 0-100)
    virtual double getCPUUsage() const = 0;
    
    // Get memory usage (MB)
    virtual double getMemoryUsage() const = 0;
    
    // Get memory peak (MB)
    virtual double getMemoryPeak() const = 0;
    
    // Get thread usage
    virtual size_t getThreadCount() const = 0;
    
    // Get process ID
    virtual pid_t getProcessID() const = 0;
    
    // Get system information
    virtual std::string getSystemInfo() const = 0;
};

// Simulation profiler
class SimulationProfiler : public PerformanceAnalyzer {
public:
    SimulationProfiler(std::shared_ptr<SystemResourceMonitor> resourceMonitor,
                      std::shared_ptr<Brain> brain);
    
    bool startProfiling(const std::string& sessionName) override;
    std::shared_ptr<PerformanceMetrics> stopProfiling(const std::string& sessionName) override;
    std::shared_ptr<PerformanceMetrics> getCurrentMetrics() const override;
    std::vector<std::shared_ptr<PerformanceMetrics>> getSessionData(const std::string& sessionName) const override;
    std::vector<std::string> getSessionNames() const override;
    std::map<std::string, double> calculateStatistics(const std::string& sessionName) const override;
    std::string generateReport(const std::string& sessionName, 
                             const std::string& format = "text") const override;

private:
    std::shared_ptr<SystemResourceMonitor> resourceMonitor_;
    std::shared_ptr<Brain> brain_;
    std::map<std::string, std::unique_ptr<ProfilingSession>> sessions_;
    std::shared_ptr<PerformanceMetrics> currentMetrics_;
    
    void updateCurrentMetrics();
    double calculateCPUUsage() const;
    double calculateMemoryUsage() const;
    double calculateFiringRate() const;
};

// Basic system resource monitor
class BasicSystemResourceMonitor : public SystemResourceMonitor {
public:
    double getCPUUsage() const override;
    double getMemoryUsage() const override;
    double getMemoryPeak() const override;
    size_t getThreadCount() const override;
    pid_t getProcessID() const override;
    std::string getSystemInfo() const override;
    
private:
    pid_t processID_;
    mutable double lastCPUTime_;
    mutable double lastSystemTime_;
    mutable double cpuUsage_;
    mutable double memoryUsage_;
    mutable double memoryPeak_;
    
    void updateResourceUsage() const;
};

// Performance report generator
class PerformanceReportGenerator {
public:
    virtual ~PerformanceReportGenerator() = default;
    
    // Generate text report
    virtual std::string generateTextReport(const std::string& sessionName,
                                         const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const = 0;
    
    // Generate JSON report
    virtual std::string generateJSONReport(const std::string& sessionName,
                                          const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const = 0;
    
    // Generate CSV report
    virtual std::string generateCSVReport(const std::string& sessionName,
                                         const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const = 0;
    
    // Generate HTML report
    virtual std::string generateHTMLReport(const std::string& sessionName,
                                          const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const = 0;
};

// Text report generator implementation
class TextReportGenerator : public PerformanceReportGenerator {
public:
    std::string generateTextReport(const std::string& sessionName,
                                 const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const override;
    
    std::string generateJSONReport(const std::string& sessionName,
                                 const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const override;
    
    std::string generateCSVReport(const std::string& sessionName,
                                 const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const override;
    
    std::string generateHTMLReport(const std::string& sessionName,
                                 const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const override;

private:
    std::string formatTime(double seconds) const;
    std::string formatMemory(double mb) const;
    std::string formatCPU(double percentage) const;
    std::string formatRate(double rate) const;
};

// Performance monitoring utilities
namespace PerformanceUtils {
    // Get current timestamp
    std::string getTimestamp();
    
    // Calculate percentage
    double percentage(double value, double total);
    
    // Smooth moving average
    class MovingAverage {
    public:
        MovingAverage(size_t windowSize);
        void addValue(double value);
        double getAverage() const;
        void reset();
    
    private:
        std::vector<double> values_;
        size_t windowSize_;
    };
    
    // Calculate statistics
    std::map<std::string, double> calculateStats(const std::vector<double>& values);
    
    // Format number with suffix (KB, MB, GB)
    std::string formatBytes(size_t bytes);
    
    // Check if value is within range
    bool inRange(double value, double min, double max);
}

// Performance monitoring manager
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Get singleton instance
    static PerformanceMonitor& getInstance();
    
    // Start/stop profiling
    bool startProfiling(const std::string& sessionName);
    std::shared_ptr<PerformanceMetrics> stopProfiling(const std::string& sessionName);
    
    // Get current metrics
    std::shared_ptr<PerformanceMetrics> getCurrentMetrics() const;
    
    // Get session data
    std::vector<std::shared_ptr<PerformanceMetrics>> getSessionData(const std::string& sessionName) const;
    std::vector<std::string> getSessionNames() const;
    
    // Generate reports
    std::string generateReport(const std::string& sessionName, const std::string& format = "text") const;
    bool exportReport(const std::string& sessionName, const std::string& format, const std::string& filepath);
    
    // System monitoring
    std::string getSystemInfo() const;
    double getCPUUsage() const;
    double getMemoryUsage() const;
    
    // Performance analysis
    std::map<std::string, double> analyzePerformance(const std::string& sessionName) const;
    
    // Configuration
    void configure(std::shared_ptr<const Config> config);
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void updateMetrics();
};

} // namespace nlm
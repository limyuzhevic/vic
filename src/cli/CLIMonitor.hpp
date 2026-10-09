#pragma once

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <vector>
#include <map>

namespace nlm {

// Simple performance metrics structure for CLI monitoring
struct PerformanceMetrics {
    double stepTimeMs;           // Current step time in milliseconds
    double firingRateHz;         // Current firing rate in Hz
    double cpuUsagePercent;      // CPU usage percentage
    double memoryUsageMB;        // Memory usage in MB
    size_t samplesCount;          // Number of samples collected
    double avgStepTimeMs;        // Average step time
    double avgFiringRateHz;      // Average firing rate
};

class CLIMonitor {
public:
    CLIMonitor();
    ~CLIMonitor();

    // Parse command line arguments
    bool parseCommandLine(int argc, char** argv);

    // Handle specific commands
    bool handleStartCommand(int argc, char** argv);
    bool handleStopCommand();
    bool handleReportCommand();

    // Print help messages
    void printHelp();
    void printStartHelp();
    void printStopHelp();
    void printReportHelp();

    // Performance monitoring functions (simplified C++ version)
    void startMonitoring(double interval, double duration, const std::string& outputFile);
    void stopMonitoring();
    PerformanceMetrics getCurrentMetrics() const;
    std::map<std::string, double> getLiveMetrics() const;

private:
    void monitoringLoop();
    void recordMetrics();
    void saveMetricsToFile(const std::string& filepath) const;

    std::thread monitoringThread_;
    std::mutex metricsMutex_;
    std::atomic<bool> monitoringActive_;
    double interval_;
    double duration_;
    std::string outputFile_;

    // Performance data storage
    std::vector<PerformanceMetrics> metricsHistory_;
    PerformanceMetrics currentMetrics_;
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    size_t totalSteps_;
};

} // namespace nlm
#pragma once

/**
 * I/O monitor for NLM (Neural Learning Machine)
 * Monitors disk I/O performance and analyzes I/O patterns
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Profiler;

/**
 * I/O performance metrics
 */
struct IOMetrics {
    std::chrono::steady_clock::time_point timestamp;
    uint64_t readBytes; // in bytes
    uint64_t writeBytes; // in bytes
    uint64_t readOperations; // number of read operations
    uint64_t writeOperations; // number of write operations
    double readSpeed; // in bytes per second
    double writeSpeed; // in bytes per second
    double ioThroughput; // total throughput in bytes per second
    double readLatency; // in seconds
    double writeLatency; // in seconds
    std::string diskDevice; // device name (e.g., "/dev/sda", "NVMe0n1")
    std::string mountPoint; // mount point
    uint64_t queueDepth; // I/O queue depth
    double cpuWaitTime; // CPU time waiting for I/O
    double ioWaitTime; // I/O wait time
    
    IOMetrics()
        : readBytes(0), writeBytes(0), readOperations(0), writeOperations(0),
          readSpeed(0.0), writeSpeed(0.0), ioThroughput(0.0),
          readLatency(0.0), writeLatency(0.0), queueDepth(0), cpuWaitTime(0.0), ioWaitTime(0.0) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * File system performance metrics
 */
struct FileSystemMetrics {
    std::string mountPoint;
    std::string fileSystemType;
    uint64_t totalSpace; // in bytes
    uint64_t usedSpace; // in bytes
    uint64_t availableSpace; // in bytes
    double usagePercentage; // percentage (0-100)
    double readSpeed; // in bytes per second
    double writeSpeed; // in bytes per second
    uint64_t readCount; // total read operations
    uint64_t writeCount; // total write operations
    double averageReadLatency; // in seconds
    double averageWriteLatency; // in seconds
    
    FileSystemMetrics() : totalSpace(0), usedSpace(0), availableSpace(0),
                          usagePercentage(0.0), readSpeed(0.0), writeSpeed(0.0),
                          readCount(0), writeCount(0), averageReadLatency(0.0),
                          averageWriteLatency(0.0) {}
};

/**
 * I/O profiler configuration
 */
struct IORMonitorConfig {
    // Sampling configuration
    int samplingInterval = 100; // milliseconds
    bool enableRealTimeMonitoring = true;
    bool enableDiskMonitoring = true;
    bool enableFileSystemMonitoring = true;
    bool enableQueueDepthMonitoring = true;
    bool enableLatencyMonitoring = true;
    
    // Performance thresholds
    double highReadLatency = 0.01; // seconds
    double highWriteLatency = 0.01; // seconds
    double highIoThroughput = 1000000000.0; // bytes per second (1GB/s)
    double highQueueDepth = 100; // operations
    double highCpuWaitTime = 0.1; // seconds
    
    // Analysis options
    bool generateIOReport = true;
    bool generateDiskReport = true;
    bool generateFileSystemReport = true;
    
    // Output configuration
    bool exportToJSON = true;
    bool exportToCSV = true;
    bool exportToText = true;
    
    // Integration settings
    bool integrateWithProfiler = true;
    bool integrateWithPhase6 = true;
    
    IORMonitorConfig() = default;
};

/**
 * I/O monitor class
 * Monitors disk I/O performance and analyzes I/O patterns
 */
class IORMonitor {
public:
    IORMonitor(const IORMonitorConfig& config = IORMonitorConfig());
    ~IORMonitor();
    
    // Monitoring operations
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return monitoringActive; }
    
    // I/O metrics operations
    IOMetrics getCurrentMetrics() const;
    std::vector<IOMetrics> getMetricsHistory(size_t count = 100) const;
    double getCurrentReadSpeed() const { return currentMetrics.readSpeed; }
    double getCurrentWriteSpeed() const { return currentMetrics.writeSpeed; }
    double getCurrentIOThroughput() const { return currentMetrics.ioThroughput; }
    
    // File system monitoring
    void updateFileSystemMetrics(const std::vector<FileSystemMetrics>& fsMetrics);
    std::vector<FileSystemMetrics> getFileSystemMetrics() const { return fileSystemMetrics; }
    
    // Session management
    void startSession(const std::string& sessionName);
    void endSession();
    const std::string& getCurrentSessionName() const { return currentSessionName; }
    
    // Statistics and reporting
    struct SessionStats {
        double sessionDuration = 0.0;
        double avgReadSpeed = 0.0;
        double maxReadSpeed = 0.0;
        double avgWriteSpeed = 0.0;
        double maxWriteSpeed = 0.0;
        double avgIOThroughput = 0.0;
        double maxIOThroughput = 0.0;
        double avgReadLatency = 0.0;
        double maxReadLatency = 0.0;
        double avgWriteLatency = 0.0;
        double maxWriteLatency = 0.0;
        size_t totalSamples = 0;
        size_t highReadLatencySamples = 0;
        size_t highWriteLatencySamples = 0;
        size_t highIoThroughputSamples = 0;
        size_t highQueueDepthSamples = 0;
        size_t highCpuWaitSamples = 0;
    };
    
    SessionStats getSessionStats() const { return currentSessionStats; }
    
    // Report generation
    bool generateIOReport(const std::string& filename = "") const;
    bool generateDiskReport(const std::string& filename = "") const;
    bool generateFileSystemReport(const std::string& filename = "") const;
    
    // File operations
    bool exportToJSON(const std::string& filename = "") const;
    bool exportToCSV(const std::string& filename = "") const;
    bool exportToText(const std::string& filename = "") const;
    
    // Thread safety
    void lock() const { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() const { std::lock_guard<std::mutex> lock(mutex_); }
    
    // Integration with profiler
    void setProfiler(Profiler* profiler) { parentProfiler = profiler; }
    
    // Prevent copy construction and assignment
    IORMonitor(const IORMonitor&) = delete;
    IORMonitor& operator=(const IORMonitor&) = delete;
    
private:
    // Configuration
    IORMonitorConfig config_;
    
    // Current session
    std::string currentSessionName;
    std::chrono::steady_clock::time_point sessionStartTime;
    
    // Monitoring data
    std::atomic<bool> monitoringActive;
    mutable std::mutex metricsMutex;
    IOMetrics currentMetrics;
    std::vector<IOMetrics> metricsHistory;
    
    // File system metrics
    std::vector<FileSystemMetrics> fileSystemMetrics;
    
    // Statistics
    SessionStats currentSessionStats;
    
    // Thread management
    mutable std::mutex mutex_;
    std::thread monitoringThread;
    std::atomic<bool> shouldStopMonitoring;
    
    // Parent profiler reference
    Profiler* parentProfiler;
    
    // File and directory management
    std::string outputDir;
    
    // Internal helper methods
    void updateStatistics();
    void sampleMetrics();
    void monitorDiskIO();
    void monitorFileSystems();
    void cleanupOldMetrics();
    void calculateSessionStatistics();
    std::string getDefaultFilename(const std::string& extension) const;
    bool validateConfiguration() const;
};

} // namespace nlm

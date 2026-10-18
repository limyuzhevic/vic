#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

class ResourceMonitor {
public:
    struct ResourceUsage {
        double cpuUsage;           // CPU utilization (0.0 - 1.0)
        size_t memoryUsage;        // Current memory usage (bytes)
        size_t memoryPeak;         // Peak memory usage (bytes)
        uint64_t diskRead;         // Disk read bytes
        uint64_t diskWrite;        // Disk write bytes
        double networkUpload;      // Network upload (bytes/sec)
        double networkDownload;    // Network download (bytes/sec)
        double temperature;        // CPU temperature (Celsius)
        std::chrono::system_clock::time_point timestamp;
    };

    ResourceMonitor();
    virtual ~ResourceMonitor();
    
    // Control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return isMonitoring_; }
    
    // Resource monitoring
    ResourceUsage getCurrentResourceUsage() const;
    std::vector<ResourceUsage> getResourceUsageHistory() const;
    ResourceUsage getAverageResourceUsage() const;
    
    // Specific resource monitoring
    double getCPUUsage() const;
    size_t getMemoryUsage() const;
    size_t getMemoryPeak() const;
    double getDiskUsage() const;
    double getNetworkUsage() const;
    
    // History management
    void clearHistory();
    void setHistorySize(size_t size);
    size_t getHistorySize() const;
    
    // Configuration
    void setSamplingInterval(double interval); // seconds
    void enableTemperatureMonitoring(bool enable);
    void setCPUMonitorEnabled(bool enable);
    void setMemoryMonitorEnabled(bool enable);
    void setDiskMonitorEnabled(bool enable);
    void setNetworkMonitorEnabled(bool enable);
    
    // Reporting
    void generateResourceReport(const std::string& outputFile) const;
    void generateResourceHistoryReport(const std::string& outputFile) const;
    std::string getResourceSummary() const;
    
    // Analysis
    double calculateCPUUsageAverage() const;
    size_t calculateMemoryUsageAverage() const;
    size_t calculateMemoryUsagePeak() const;
    std::vector<std::string> identifyResourceBottlenecks() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool isMonitoring_;
    mutable std::mutex mutex_;
};

} // namespace nlm

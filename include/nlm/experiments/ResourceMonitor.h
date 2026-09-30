#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <thread>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

namespace nlm {

/**
 * System resource monitor for tracking CPU, memory, disk, and network usage
 * during experiment execution to optimize resource allocation and prevent
 * resource exhaustion.
 */
class ResourceMonitor {
public:
    // Resource usage data
    struct ResourceUsage {
        std::chrono::duration<double> timestamp;
        double cpuUsage;
        size_t memoryUsage;
        size_t virtualMemoryUsage;
        double diskUsage;
        double networkUsage;
        std::string processId;
        
        ResourceUsage() : cpuUsage(0.0), memoryUsage(0), virtualMemoryUsage(0),
                         diskUsage(0.0), networkUsage(0.0) {}
    };
    
    // Resource threshold configuration
    struct ResourceThresholds {
        double maxCpuUsage;
        size_t maxMemoryUsage;
        size_t maxVirtualMemoryUsage;
        double maxDiskUsage;
        double maxNetworkUsage;
        std::chrono::duration<double> monitoringInterval;
        std::function<void(const ResourceUsage&)> thresholdExceededCallback;
        
        ResourceThresholds()
            : maxCpuUsage(90.0), maxMemoryUsage(2 * 1024 * 1024 * 1024),  // 2GB
              maxVirtualMemoryUsage(4 * 1024 * 1024 * 1024),  // 4GB
              maxDiskUsage(95.0), maxNetworkUsage(1000.0),  // MB/s
              monitoringInterval(std::chrono::seconds(1)) {}
    };
    
    // Resource monitoring session
    struct MonitoringSession {
        std::string sessionName;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::vector<ResourceUsage> resourceUsageHistory;
        bool isActive;
        
        MonitoringSession() : isActive(false) {}
    };
    
    // Resource statistics
    struct ResourceStatistics {
        double averageCpuUsage;
        size_t averageMemoryUsage;
        size_t peakMemoryUsage;
        double averageDiskUsage;
        double averageNetworkUsage;
        double cpuUsageStdDev;
        double memoryUsageStdDev;
        
        ResourceStatistics() : averageCpuUsage(0.0), averageMemoryUsage(0),
                              peakMemoryUsage(0), averageDiskUsage(0.0),
                              averageNetworkUsage(0.0), cpuUsageStdDev(0.0),
                              memoryUsageStdDev(0.0) {}
    };
    
    ResourceMonitor();
    ~ResourceMonitor() = default;
    
    // Resource monitoring
    void startMonitoring(const std::string& sessionName);
    void stopMonitoring(const std::string& sessionName);
    ResourceUsage getCurrentResourceUsage() const;
    
    // Data management
    void recordResourceUsage(const std::string& sessionName, const ResourceUsage& usage);
    ResourceUsage getLastResourceUsage(const std::string& sessionName) const;
    std::vector<ResourceUsage> getResourceUsageHistory(const std::string& sessionName) const;
    
    // Statistics and analysis
    ResourceStatistics calculateStatistics(const std::string& sessionName) const;
    std::string analyzeResourceUsage(const std::string& sessionName) const;
    std::string generateResourceReport(const std::string& sessionName) const;
    
    // Configuration management
    void setResourceThresholds(const ResourceThresholds& thresholds);
    const ResourceThresholds& getResourceThresholds() const;
    
    // Output
    bool saveMonitoringData(const std::string& sessionName, const std::string& filepath) const;
    bool loadMonitoringData(const std::string& sessionName, const std::string& filepath);
    
    // Session management
    bool hasSession(const std::string& sessionName) const;
    std::vector<std::string> getSessionNames() const;
    void clearSession(const std::string& sessionName);
    void clearAllSessions();
    
    // Resource optimization
    std::string optimizeResourceUsage(const std::string& sessionName);
    std::vector<std::string> getResourceOptimizationRecommendations(const std::string& sessionName) const;
    
    // File system monitoring
    std::vector<std::string> getDiskUsage(const std::string& path = "/") const;
    std::vector<std::string> getProcessList() const;
    ResourceUsage getProcessResourceUsage(const std::string& processId) const;
    
    // Network monitoring
    double getNetworkUploadSpeed() const;
    double getNetworkDownloadSpeed() const;
    std::string getNetworkInterfaceStats() const;
    
    // System information
    std::string getSystemInfo() const;
    size_t getAvailableMemory() const;
    double getSystemLoadAverage() const;
    
private:
    // Session storage
    std::unordered_map<std::string, MonitoringSession> sessions;
    
    // Configuration
    ResourceThresholds thresholds;
    
    // Monitoring thread
    std::atomic<bool> monitoringActive;
    std::thread monitoringThread;
    
    // Helper functions
    ResourceUsage collectResourceUsage() const;
    double calculateCpuUsage() const;
    size_t getMemoryUsage() const;
    size_t getVirtualMemoryUsage() const;
    double getDiskUsage(const std::string& path) const;
    double getNetworkUsage() const;
    std::string getProcessId() const;
    
    // File system utilities
    std::vector<std::string> listDirectoryContents(const std::string& path) const;
    bool isDirectory(const std::string& path) const;
    std::string getFileType(const std::string& path) const;
};

} // namespace nlm
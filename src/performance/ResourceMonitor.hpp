#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <thread>
#include <memory>
#include <map>
#include <algorithm>

namespace nlm {

// ResourceMonitor: System resource usage (CPU, memory, GPU)
class ResourceMonitor {
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Configuration
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    void setCpuThreshold(double threshold) { cpuThreshold_ = threshold; }
    void setMemoryThreshold(double threshold) { memoryThreshold_ = threshold; }
    void setEnableGPUTracking(bool enable) { enableGPUTracking_ = enable; }
    void setEnableNetworkTracking(bool enable) { enableNetworkTracking_ = enable; }
    void setEnableDiskTracking(bool enable) { enableDiskTracking_ = enable; }
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isMonitoring() const { return monitoring_; }
    
    // Resource usage
    double getCurrentCpuUsage() const { return currentCpuUsage_; }
    double getCurrentMemoryUsage() const { return currentMemoryUsage_; }
    size_t getCurrentMemoryAllocations() const { return currentMemoryAllocations_; }
    size_t getCurrentThreadCount() const { return currentThreadCount_; }
    double getCurrentGpuUsage() const { return currentGpuUsage_; }
    double getCurrentNetworkUsage() const { return currentNetworkUsage_; }
    double getCurrentDiskUsage() const { return currentDiskUsage_; }
    
    // History
    std::vector<ResourceUsage> getResourceHistory() const;
    std::string getResourceReport() const;
    std::string getDetailedResourceReport() const;
    
    // Analysis
    std::string analyzeResourceTrends() const;
    std::string identifyResourceBottlenecks() const;
    double calculateAverageCpuUsage() const;
    double calculateAverageMemoryUsage() const;
    
    // Integration
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { 
        debuggingSystem_ = debuggingSystem; 
    }
    
    // Export
    bool exportResourceProfile(const std::string& filepath) const;
    std::string exportResourceProfileAsString() const;
    
    // Statistics
    size_t getSampleCount() const { return resourceHistory_.size(); }
    double getMaxCpuUsage() const;
    double getMaxMemoryUsage() const;
    std::chrono::system_clock::time_point getLastUpdateTime() const { return lastUpdateTime_; }
    
    // Real-time monitoring
    std::string getRealTimeResourceStats() const;
    bool isResourceUsageHigh() const;
    
private:
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        size_t memoryAllocations;
        size_t threadCount;
        double gpuUsage;
        double networkUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
    };
    
    bool monitoring_;
    double samplingInterval_;
    double cpuThreshold_;
    double memoryThreshold_;
    bool enableGPUTracking_;
    bool enableNetworkTracking_;
    bool enableDiskTracking_;
    
    double currentCpuUsage_;
    double currentMemoryUsage_;
    size_t currentMemoryAllocations_;
    size_t currentThreadCount_;
    double currentGpuUsage_;
    double currentNetworkUsage_;
    double currentDiskUsage_;
    
    std::vector<ResourceUsage> resourceHistory_;
    mutable std::mutex historyMutex_;
    
    std::thread monitoringThread_;
    std::mutex monitorMutex_;
    
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    std::chrono::system_clock::time_point lastUpdateTime_;
    
    // Helper methods
    void monitoringLoop();
    void updateResourceUsage();
    void simulateResourceUsage(); // In real implementation, this would query actual system resources
    bool isResourceThresholdExceeded() const;
    std::string getSystemInfo() const;
    std::string formatResourceUsage(const ResourceUsage& usage) const;
    double calculateTrend(const std::vector<ResourceUsage>& history, 
                         const std::string& metric) const;
};

} // namespace nlm

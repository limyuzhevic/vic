#pragma once

#include "../batch_processing/BatchUtils.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance benchmarking for batch runs
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double totalTime;
        double cpuTime;
        double memoryUsageMB;
        double peakMemoryUsageMB;
        size_t totalNeuronsProcessed;
        size_t totalSpikesGenerated;
        size_t totalSynapsesUpdated;
        double spikesPerSecond;
        double neuronsPerSecond;
        double synapsesPerSecond;
        double efficiencyScore; // Normalized efficiency metric
        
        PerformanceMetrics() : totalTime(0), cpuTime(0), memoryUsageMB(0),
                               peakMemoryUsageMB(0), totalNeuronsProcessed(0),
                               totalSpikesGenerated(0), totalSynapsesUpdated(0),
                               spikesPerSecond(0), neuronsPerSecond(0), synapsesPerSecond(0),
                               efficiencyScore(0) {}
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3);
            ss << "Time: " << totalTime << "s, CPU: " << cpuTime << "s, "
               << "Memory: " << memoryUsageMB << "MB (peak: " << peakMemoryUsageMB << "MB), "
               << "Neurons: " << neuronsPerSecond << " neurons/s, "
               << "Spikes: " << spikesPerSecond << " spikes/s, "
               << "Efficiency: " << efficiencyScore;
            return ss.str();
        }
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring a task
    void startTask(const std::string& taskName);
    
    // End monitoring a task
    PerformanceMetrics endTask(const std::string& taskName);
    
    // Get performance metrics for batch
    PerformanceMetrics getBatchMetrics(const std::vector<BatchSimulationResult>& results) const;
    
    // Get overall performance summary
    std::map<std::string, PerformanceMetrics> getOverallMetrics() const;
    
    // Generate optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations() const;
    
    // Reset all metrics
    void reset();
    
private:
    struct TaskMetrics {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point cpuStartTime;
        std::chrono::high_resolution_clock::time_point endTime;
        double memoryUsage;
        double peakMemoryUsage;
        size_t neuronsProcessed;
        size_t spikesGenerated;
        size_t synapsesUpdated;
    };
    
    std::unordered_map<std::string, TaskMetrics> taskMetrics_;
    mutable std::mutex metricsMutex_;
    PerformanceMetrics cumulativeMetrics_;
    std::thread memoryMonitorThread_;
    std::atomic<bool> running_{false};
    
    void memoryMonitorFunc();
};

// Resource monitoring during parallel execution
class ResourceMonitor {
public:
    struct ResourceUsage {
        double cpuUsagePercent;      // CPU usage percentage
        double memoryUsageMB;        // Current memory usage in MB
        double memoryAvailableMB;    // Available memory in MB
        size_t activeThreads;        // Number of active threads
        size_t pendingTasks;         // Number of pending tasks
        double gpuMemoryUsageMB;     // GPU memory usage if available
        double gpuComputeUsage;      // GPU compute usage percentage
        std::chrono::system_clock::time_point timestamp;
        
        ResourceUsage() : cpuUsagePercent(0), memoryUsageMB(0), memoryAvailableMB(0),
                         activeThreads(0), pendingTasks(0), gpuMemoryUsageMB(0),
                         gpuComputeUsage(0) {}
    };
    
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Start resource monitoring
    void startMonitoring();
    
    // Stop resource monitoring
    void stopMonitoring();
    
    // Get current resource usage
    ResourceUsage getCurrentResourceUsage() const;
    
    // Get historical resource usage
    std::vector<ResourceUsage> getResourceHistory(size_t maxHistory = 1000) const;
    
    // Check if resources are overloaded
    bool isOverloaded() const;
    
private:
    mutable std::mutex usageMutex_;
    ResourceUsage currentUsage_;
    std::vector<ResourceUsage> history_;
    std::thread monitorThread_;
    std::atomic<bool> monitoring_{false};
    
    void monitorFunc();
    double getSystemCpuUsage() const;
    double getSystemMemoryUsage() const;
    size_t getActiveThreadCount() const;
};nlm
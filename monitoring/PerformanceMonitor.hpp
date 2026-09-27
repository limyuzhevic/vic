#pragma once

#include "PerformanceMonitor.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>
#include <cmath>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage{0.0};
        double memoryUsage{0.0};
        double diskUsage{0.0};
        size_t activeThreads{0};
        size_t pendingTasks{0};
        float averageResponseTime{0.0};
        double networkUsage{0.0};
        Timestamp timestamp;
        
        PerformanceMetrics() : timestamp(0.0) {}
    };

    PerformanceMonitor();
    ~PerformanceMonitor();

    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;

    // Monitoring
    void startMonitoring(double interval = 1.0);
    void stopMonitoring();
    bool isMonitoring() const;
    
    PerformanceMetrics getCurrentMetrics() const;
    std::vector<PerformanceMetrics> getHistory(size_t count) const;
    
    // Alerting
    void addAlertThreshold(const std::string& metric, double threshold, 
                          std::function<void(const PerformanceMetrics&)> callback);
    void removeAlertThreshold(const std::string& metric);
    std::vector<std::string> getActiveAlerts() const;

    // Performance analysis
    double getCPUUtilization() const;
    double getMemoryUtilization() const;
    size_t getActiveThreadCount() const;
    
    // Integration
    void setSpikeSystem(SpikeSystem* spikeSystem);
    void setBrain(Brain* brain);
    void setNeuralDynamics(NeuralDynamics* dynamics);

    // Statistics
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

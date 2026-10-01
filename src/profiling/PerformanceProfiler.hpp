#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <iostream>

namespace nlm {

// Performance monitoring and profiling
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();

    // Initialize profiler
    bool initialize();

    // Start profiling a section
    void startProfile(const std::string& sectionName);

    // Stop profiling a section
    void stopProfile(const std::string& sectionName);

    // Get profile data for section
    struct ProfileData {
        std::string sectionName;
        std::chrono::duration<double> totalTime;
        std::chrono::duration<double> minTime;
        std::chrono::duration<double> maxTime;
        size_t callCount;
        double averageTime;
        double standardDeviation;
        double cpuTimePercent;
        double memoryTimePercent;
        std::vector<std::chrono::duration<double>> timestamps;
        
        ProfileData(const std::string& name) 
            : sectionName(name), totalTime(0), minTime(std::chrono::duration<double>::max()),
              maxTime(0), callCount(0), averageTime(0.0), standardDeviation(0.0),
              cpuTimePercent(0.0), memoryTimePercent(0.0) {}
    };

    // Get profile data
    std::vector<ProfileData> getProfileData() const;

    // Get profile data for specific section
    ProfileData getProfileData(const std::string& sectionName) const;

    // Get total execution time
    double getTotalExecutionTime() const;

    // Get memory usage
    size_t getMemoryUsage() const;

    // Get memory allocation rate
    double getMemoryAllocationRate() const;

    // Get CPU usage
    double getCPUUsage() const;

    // Get I/O operations count
    size_t getIOOperations() const;

    // Get network operations count
    size_t getNetworkOperations() const;

    // Get thread count
    size_t getThreadCount() const;

    // Check if profiler is active
    bool isActive() const;

    // Clear profile data
    void clearProfileData();

    // Export profile data
    bool exportProfile(const std::string& filename, const std::string& format = "csv") const;

    // Set profiling target
    void setProfileTarget(const std::string& target);

    // Get profiling target
    std::string getProfileTarget() const;

    // Enable/disable detailed timing
    void setDetailedTiming(bool enable);
    bool isDetailedTimingEnabled() const;

    // Get current timestamp
    std::chrono::steady_clock::time_point getCurrentTime() const;

    // Calculate FPS
    double getFPS() const;

    // Get bottleneck sections
    std::vector<std::string> getBottleneckSections(double threshold) const;

    // Get top N slowest sections
    std::vector<std::string> getTopSlowSections(size_t count) const;

    // Start real-time monitoring
    void startRealTimeMonitoring();

    // Stop real-time monitoring
    void stopRealTimeMonitoring();

    // Get real-time statistics
    struct RealTimeStats {
        double currentFPS;
        double averageFPS;
        double minFPS;
        double maxFPS;
        double memoryUsageMB;
        double cpuUsagePercent;
        double diskReadMBps;
        double diskWriteMBps;
        std::chrono::steady_clock::time_point lastUpdate;
        
        RealTimeStats() : currentFPS(0.0), averageFPS(0.0), minFPS(0.0), maxFPS(0.0),
                         memoryUsageMB(0.0), cpuUsagePercent(0.0), diskReadMBps(0.0),
                         diskWriteMBps(0.0) {}
    };

    RealTimeStats getRealTimeStats() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
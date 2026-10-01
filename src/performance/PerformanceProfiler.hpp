#pragma once

#include "../../core/Types/Types.hpp"
#include "../../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <thread>
#include <map>
#include <functional>
#include <optional>

namespace nlm {

class PerformanceProfiler {
public:
    // Profiling data structures
    struct TimerData {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::duration<double> totalDuration;
        std::chrono::duration<double> minDuration;
        std::chrono::duration<double> maxDuration;
        uint64_t callCount;
        uint64_t sampleCount;
        double cpuTimePercent;
        double memoryTimePercent;
        std::vector<std::chrono::duration<double>> samples;
        
        TimerData(const std::string& n) 
            : name(n), totalDuration(0), minDuration(std::chrono::duration<double>::max()),
              maxDuration(0), callCount(0), sampleCount(0), cpuTimePercent(0.0), memoryTimePercent(0.0) {}
    };

    struct MemorySample {
        std::chrono::high_resolution_clock::time_point timestamp;
        uint64_t totalMemoryBytes;
        uint64_t allocatedMemoryBytes;
        uint64_t deallocatedMemoryBytes;
        uint32_t allocationCount;
        uint32_t deallocationCount;
    };

    struct CPUSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double cpuUsagePercent;
        uint32_t threadCount;
        std::vector<double> coreUsage;
    };

    struct IOSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        uint64_t readBytes;
        uint64_t writeBytes;
        uint32_t readOperations;
        uint32_t writeOperations;
        double readSpeedMBps;
        double writeSpeedMBps;
    };

    struct GPUSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double gpuUsagePercent;
        uint64_t memoryUsedMB;
        uint64_t memoryTotalMB;
        uint32_t activeThreads;
        std::vector<double> memoryUsageHistory;
    };

    struct ProfileStats {
        double totalExecutionTime;
        uint64_t totalMemoryUsage;
        double peakMemoryUsage;
        uint64_t totalCPUInstructions;
        uint64_t totalIOLimit;
        double averageFPS;
        double minFPS;
        double maxFPS;
        uint32_t threadCount;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<MemorySample> memorySamples;
        std::vector<CPUSample> cpuSamples;
        std::vector<IOSample> ioSamples;
        std::vector<GPUSample> gpuSamples;
        std::unordered_map<std::string, TimerData> timers;
        std::vector<std::string> activeSections;
        bool isProfiling;
        bool isRealTimeEnabled;
        std::chrono::milliseconds profilingInterval;
    };

    // Constructor and destructor
    PerformanceProfiler();
    ~PerformanceProfiler();

    // Initialize profiler
    bool initialize(std::shared_ptr<Config> config = nullptr);
    void shutdown();

    // Start/stop profiling for a section
    void startProfile(const std::string& sectionName);
    void stopProfile(const std::string& sectionName);
    void pauseProfile(const std::string& sectionName);
    void resumeProfile(const std::string& sectionName);

    // Check if a section is being profiled
    bool isProfiling(const std::string& sectionName) const;

    // Get profile data
    const TimerData& getTimerData(const std::string& sectionName) const;
    std::vector<std::string> getAllSectionNames() const;

    // Real-time monitoring
    void startRealTimeMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoringActive() const;

    // Get real-time statistics
    ProfileStats getProfileStats() const;
    MemorySample getCurrentMemorySample() const;
    CPUSample getCurrentCPUSample() const;
    IOSample getCurrentIOSample() const;
    GPUSample getCurrentGPUSample() const;

    // Memory profiling
    void trackMemoryAllocation(size_t size);
    void trackMemoryDeallocation(size_t size);
    uint64_t getCurrentMemoryUsage() const;
    uint64_t getPeakMemoryUsage() const;
    double getMemoryAllocationRate() const;

    // CPU profiling
    void updateCPUUsage();
    double getCurrentCPUUsage() const;
    uint32_t getActiveThreadCount() const;

    // I/O profiling
    void trackReadOperation(size_t bytes);
    void trackWriteOperation(size_t bytes);
    size_t getTotalReadBytes() const;
    size_t getTotalWriteBytes() const;
    double getAverageReadSpeedMBps() const;
    double getAverageWriteSpeedMBps() const;

    // GPU profiling (if available)
    void updateGPUUsage();
    double getGPUUsagePercent() const;
    uint64_t getGPUMemoryUsedMB() const;

    // Control methods
    void reset();
    void clearSection(const std::string& sectionName);
    void clearAllSections();

    // Configuration
    void setProfilingInterval(std::chrono::milliseconds interval);
    void setDetailedTiming(bool enable);
    void setMemoryTracking(bool enable);
    void setCPUTracking(bool enable);
    void setIOTracking(bool enable);
    void setGPUTracking(bool enable);

    // Export functionality
    bool exportProfile(const std::string& filename, const std::string& format = "csv") const;
    bool exportToJSON(const std::string& filename) const;
    bool exportToSQLite(const std::string& filename) const;

    // Analysis helpers
    std::vector<std::string> getBottleneckSections(double threshold = 0.2) const;
    std::vector<std::string> getTopSlowSections(size_t count = 10) const;
    double calculateSectionTimePercent(const std::string& sectionName) const;
    std::string getProfileSummary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void updateRealTimeStats();
    void cleanupOldSamples();
};

} // namespace nlm

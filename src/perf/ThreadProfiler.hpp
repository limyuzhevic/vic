#pragma once

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <map>
#include <algorithm>

namespace nlm {

class ThreadProfiler : public PerformanceProfiler {
private:
    struct ThreadSnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
        std::string threadName;
        double cpuUsage;
        size_t stackSize;
        size_t memoryUsage;
        double activeTime;
        double waitTime;
        double cpuWaitTime;
        bool isActive;
    };
    
    std::vector<ThreadSnapshot> threadSnapshots;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    bool profilingActive;
    double samplingInterval;
    std::mutex dataMutex;
    std::atomic<bool> stopFlag;
    std::thread samplingThread;
    
    // Thread tracking
    std::map<std::thread::id, std::string> threadIds;
    std::map<std::thread::id, ThreadSnapshot> currentThreadData;
    std::mutex threadMutex;
    
    // Performance analysis
    struct ThreadPerformance {
        std::thread::id threadId;
        std::string threadName;
        double utilization;
        double throughput;
        double latency;
        size_t workItems;
        double averageWorkTime;
        double peakWorkTime;
    };
    
    std::vector<ThreadPerformance> threadPerformance;
    
    // Platform-specific thread monitoring
    void collectThreadData();
    void samplingFunction();
    void trackCurrentThread();
    std::string getCurrentThreadName() const;
    
public:
    ThreadProfiler();
    ~ThreadProfiler() override;
    
    // Profiling control
    void startProfiling() override;
    void stopProfiling() override;
    bool isProfiling() const override;
    
    // Performance metrics collection
    double getCpuUsage() const override;
    size_t getMemoryUsage() const override;
    double getThreadUsage() const override;
    double getIoUsage() const override;
    
    // Profiling data
    PerformanceMetrics getCurrentMetrics() const override;
    std::vector<PerformanceMetrics> getHistoricalData() const override;
    
    // Configuration
    void setSamplingInterval(double interval) override;
    double getSamplingInterval() const override;
    
    // Thread-specific profiling
    void startCpuProfiling() override;
    void stopCpuProfiling() override;
    void startMemoryProfiling() override;
    void stopMemoryProfiling() override;
    void startThreadProfiling() override;
    void stopThreadProfiling() override;
    void startIoProfiling() override;
    void stopIoProfiling() override;
    
    // Thread-specific methods
    size_t getActiveThreadCount() const;
    size_t getTotalThreadCount() const;
    double getThreadUtilization() const;
    double getAverageThreadUtilization() const;
    double getPeakThreadUtilization() const;
    std::vector<ThreadPerformance> getThreadPerformance() const;
    std::map<std::thread::id, std::string> getThreadIds() const;
    double calculateThreadScalability(int threadCount) const;
    void analyzeThreadContention() const;
    void identifyThreadBottlenecks() const;
    
    // Thread performance metrics
    double getAverageThreadLifeTime() const;
    double getThreadContextSwitchRate() const;
    size_t getThreadLockContention() const;
    std::vector<double> getThreadWaitTimes() const;
    std::vector<double> getThreadCpuTimes() const;
};

} // namespace nlm
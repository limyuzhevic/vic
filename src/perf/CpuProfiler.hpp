#pragma once

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

class CpuProfiler : public PerformanceProfiler {
private:
    struct ThreadCpuData {
        std::chrono::high_resolution_clock::time_point lastTime;
        double cpuUsage;
        double userTime;
        double systemTime;
        double totalCpuTime;
        double totalUserTime;
        double totalSystemTime;
        bool active;
    };
    
    std::vector<ThreadCpuData> threadData;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    bool profilingActive;
    double samplingInterval;
    std::mutex dataMutex;
    std::atomic<bool> stopFlag;
    
    // Raw CPU time data
    struct CpuSnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        double totalCpuTime;
        double idleTime;
        double userTime;
        double systemTime;
    };
    
    std::vector<CpuSnapshot> cpuSnapshots;
    std::thread samplingThread;
    
    // Platform-specific CPU monitoring
    double getSystemCpuUsage();
    void collectCpuData();
    void samplingFunction();
    
public:
    CpuProfiler();
    ~CpuProfiler() override;
    
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
    
    // CPU-specific profiling
    void startCpuProfiling() override;
    void stopCpuProfiling() override;
    void startMemoryProfiling() override;
    void stopMemoryProfiling() override;
    void startThreadProfiling() override;
    void stopThreadProfiling() override;
    void startIoProfiling() override;
    void stopIoProfiling() override;
    
    // CPU-specific methods
    double getAverageCpuUsage() const;
    double getPeakCpuUsage() const;
    std::vector<double> getPerCoreCpuUsage() const;
    double getCpuSteadyState() const;
    
    // Simulation step timing
    void startSimulationStep();
    void endSimulationStep();
    double getAverageStepTime() const;
    double getPeakStepTime() const;
    std::vector<double> getStepTimes() const;
};

} // namespace nlm
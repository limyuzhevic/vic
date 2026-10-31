#pragma once

#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <iomanip>

namespace nlm {

/**
 * Simple performance monitoring tool for NLM demos
 */
class SimplePerformanceMonitor {
public:
    SimplePerformanceMonitor();
    ~SimplePerformanceMonitor();
    
    void start();
    void stop();
    
    void update(const std::shared_ptr<class Brain>& brain, size_t stepsCompleted);
    
    void setReportInterval(int seconds);
    bool isRunning() const;
    
private:
    void monitorLoop();
    void reportProgress();
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastReportTime;
    std::chrono::seconds reportInterval;
    
    size_t stepCount;
    size_t totalNeurons;
    size_t totalSynapses;
    float maxFiringRate;
    float minFiringRate;
    float avgFiringRate;
    size_t totalSpikes;
    
    std::thread monitorThread;
    bool monitoring;
    std::mutex mutex;
};

/**
 * Simple performance monitoring tool for Phase 6 demo
 */
class Phase6PerformanceMonitor {
public:
    Phase6PerformanceMonitor();
    ~Phase6PerformanceMonitor();
    
    void update(const struct Phase6IntegrationResult& result, size_t stepsCompleted);
    
    bool isRunning() const;
    
private:
    void reportProgress(const struct Phase6IntegrationResult& result);
    
    std::chrono::high_resolution_clock::time_point startTime;
    size_t stepCount;
    
    int memoryWorkingMemory;
    int memoryEpisodicMemory;
    float neuromodulationLevel;
    float predictionAccuracy;
    float developmentProgress;
};

} // namespace nlm
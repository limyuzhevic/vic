// CPU Profiler Implementation
//
// This file implements the CPU profiler with detailed CPU usage analysis.

#pragma once

#include "CPUProfiler.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace nlm {

class CPUProfiler : public ProfilerBase, public nlm::CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler() override;
    
    bool initialize(const Config& config) override;
    void shutdown() override;
    bool isAvailable() const override;
    std::string getName() const override;
    
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void recordSample(const std::string& sampleName,
                     const std::map<std::string, double>& metrics,
                     const std::map<std::string, std::string>& metadata = {}) override;
    
    std::vector<std::shared_ptr<ProfileData>> getSamples() const override;
    std::vector<std::shared_ptr<ProfileData>> getSamplesByTag(const std::string& tag) const override;
    
    void addTag(const std::string& tag) override;
    void removeTag(const std::string& tag) override;
    std::vector<std::string> getTags() const override;
    
    double getTotalDuration() const override;
    size_t getSampleCount() const override;
    
    // CPU Profiler specific methods
    void setCPUCores(const std::vector<int>& cores) override;
    std::vector<int> getCPUCores() const override;
    
    void setSamplingRate(double rate) override;
    double getSamplingRate() const override;
    
    void enableBranchPrediction(bool enable) override;
    bool isBranchPredictionEnabled() const override;
    
    void enableCacheAnalysis(bool enable) override;
    bool isCacheAnalysisEnabled() const override;
    
    void enableContextSwitchAnalysis(bool enable);
    bool isContextSwitchAnalysisEnabled() const;
    
    void enableThreadProfiling(bool enable);
    bool isThreadProfilingEnabled() const;
    
    // Flame graph generation
    void generateFlameGraph(const std::string& outputFile) const;
    void generateCallGraph(const std::string& outputFile) const;
    
    // Performance metrics
    double getCPUUtilization() const;
    double getUserCPUTime() const;
    double getSystemCPUTime() const;
    double getIdleCPUTime() const;
    std::vector<double> getPerCoreUtilization() const;
    
    // Branch prediction analysis
    void analyzeBranchPrediction() const;
    
    // Cache performance analysis
    void analyzeCachePerformance() const;
    
    // Context switching analysis
    void analyzeContextSwitches() const;
    
    // Thread profiling
    void profileThreads() const;
    
    // Detailed profiling
    void enableDetailedProfiling(bool enable);
    bool isDetailedProfilingEnabled() const;
    
private:
    struct SampleData {
        std::string sampleName;
        double timestamp;
        std::map<std::string, double> metrics;
        std::map<std::string, std::string> metadata;
        std::vector<std::string> tags;
    };
    
    mutable std::mutex mutex_;
    std::atomic<bool> isRunning_;
    std::atomic<bool> isInitialized_;
    
    std::vector<SampleData> samples_;
    std::vector<std::string> tags_;
    std::thread sampleThread_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point lastSampleTime_;
    
    double samplingRate_;
    std::vector<int> cpuCores_;
    
    bool branchPredictionEnabled_;
    bool cacheAnalysisEnabled_;
    bool contextSwitchAnalysisEnabled_;
    bool threadProfilingEnabled_;
    bool detailedProfilingEnabled_;
    
    // Thread-safe sampling
    void sampleCPUUsage();
    void updatePerCoreMetrics();
    void analyzeBranchPrediction() const;
    void analyzeCachePerformance() const;
    void analyzeContextSwitches() const;
    void profileThreads() const;
    
    // Helper methods
    double getCurrentCPUTime() const;
    double getCurrentSystemCPUTime() const;
    std::vector<double> getCurrentPerCoreUtilization() const;
    std::unordered_map<std::string, double> getCurrentBranchMetrics() const;
    std::unordered_map<std::string, double> getCurrentCacheMetrics() const;
    std::unordered_map<std::string, double> getCurrentContextSwitchMetrics() const;
    std::unordered_map<std::string, double> getCurrentMetrics() const;
};

} // namespace nlm

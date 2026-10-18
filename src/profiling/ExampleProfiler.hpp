// Example Profiling Tool Implementation
//
// This file provides an example of how profiling tools should be implemented.
// This is a simple test profiler to demonstrate the pattern.

#pragma once

#include "ProfilerBase.hpp"
#include "ProfilingFramework.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <algorithm>

namespace nlm {

class ExampleProfiler : public ProfilerBase {
public:
    ExampleProfiler();
    ~ExampleProfiler() override;
    
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
    std::thread samplingThread_;
    
    std::chrono::steady_clock::time_point startTime_;
    double samplingRate_;
    
    // Thread-safe sampling
    void sampleData();
    std::unordered_map<std::string, double> getCurrentMetrics() const;
    
    // Helper methods
    double getCurrentTimestamp() const;
};

} // namespace nlm

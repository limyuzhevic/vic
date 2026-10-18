#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace nlm {

struct ProfilerConfig {
    std::string name;
    bool enabled;
    double samplingInterval; // seconds
    bool trackMemory;
    bool trackCPU;
    bool trackIO;
    bool trackEvents;
    size_t maxSamples;
    std::string outputFormat; // text, json, csv, html
    std::string outputFile;
    bool realTimeMonitoring;
};

class Profiler {
public:
    Profiler();
    explicit Profiler(const ProfilerConfig& config);
    virtual ~Profiler();
    
    // Configuration
    void configure(const ProfilerConfig& config);
    ProfilerConfig getConfig() const { return config; }
    
    // Control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Event profiling
    struct PerformanceMetrics {
        std::string operationName;
        std::chrono::duration<double> duration;
        size_t memoryAllocated;
        size_t memoryDeallocated;
        double cpuTime;
        size_t ioBytes;
        std::chrono::system_clock::time_point timestamp;
        std::string threadId;
    };

    void profileStep(int step, const PerformanceMetrics& metrics);
    void profileEvent(const std::string& eventName, const std::chrono::duration<double>& duration);
    void profileMemory(size_t size, bool allocation);
    void profileCPU(double time);
    void profileIO(size_t bytes, bool isRead);
    
    // Data access
    struct ProfileData {
        std::vector<PerformanceMetrics> metrics;
        std::unordered_map<std::string, size_t> eventCounts;
        std::unordered_map<std::string, std::chrono::duration<double>> totalTimes;
        std::vector<size_t> memorySamples;
        std::vector<double> cpuSamples;
        size_t totalStepsProfiled;
    };

    ProfileData getProfileData() const;
    PerformanceMetrics getLastMetric() const;
    double getAverageStepTime() const;
    double getMedianStepTime() const;
    
    // Analysis
    std::vector<std::string> identifySlowOperations(double threshold) const;
    std::vector<std::string> identifyMemoryLeaks() const;
    std::vector<std::string> getPerformanceBottlenecks() const;
    std::string generatePerformanceAnalysis() const;
    
    // Output
    void exportProfile(const std::string& filename) const;
    void generateTextReport(std::ostream& output) const;
    void generateJSONReport(std::ostream& output) const;
    void generateCSVReport(std::ostream& output) const;
    void generateHTMLReport(std::ostream& output) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ProfilerConfig config;
    bool isProfiling_;
    mutable std::mutex mutex_;
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "../core/Logger/Logger.hpp"
#include "DebugConfig.hpp"

namespace nlm {

// Performance profile data
struct ProfileData {
    std::chrono::steady_clock::time_point timestamp;
    std::string operation;
    double duration;
    double cpuUsage;
    double memoryUsage;
    size_t memoryAllocated;
    size_t memoryFreed;
    std::map<std::string, double> metrics;
    std::string threadId;
    std::string callStack;
};

// Performance bottleneck information
struct Bottleneck {
    std::string id;
    std::string description;
    std::chrono::steady_clock::time_point detectedTime;
    double impactScore;
    std::string location; // file:line or function
    std::vector<ProfileData> relatedProfiles;
    std::string recommendation;
};

// Performance profiling configuration
struct ProfilerConfig {
    bool enableSimulationProfiling;
    bool enableMemoryProfiling;
    bool enableNeuralEventTimingAnalysis;
    bool enableBottleneckDetection;
    bool enableDetailedTiming;
    int profileSamplingInterval; // milliseconds
    size_t maxProfileEntries;
    double bottleneckThreshold;
    std::vector<std::string> profileMetrics;
    std::vector<std::string> ignoreFunctions;
    bool enableRealTimeAnalysis;
    bool enableAutomatedOptimizationSuggestions;
};

// Performance profiler interface
class IProfiler {
public:
    virtual ~IProfiler() = default;
    virtual void initialize(std::shared_ptr<DebugConfig> config) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void update() = 0;
    virtual void beginProfile(const std::string& operation) = 0;
    virtual void endProfile(const std::string& operation) = 0;
    virtual void profileMemoryAllocation(size_t size, const std::string& context) = 0;
    virtual void profileMemoryDeallocation(size_t size, const std::string& context) = 0;
    virtual std::vector<ProfileData> getProfileData(const std::string& operation, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) const = 0;
    virtual std::vector<Bottleneck> getBottlenecks() const = 0;
    virtual std::map<std::string, double> getPerformanceMetrics() const = 0;
    virtual std::string getPerformanceReport() const = 0;
    virtual void clearProfileData() = 0;
};

// Performance profiler implementation
class Profiler : public IProfiler {
public:
    Profiler();
    ~Profiler() override;
    
    // Disable copying, enable moving
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    Profiler(Profiler&&) noexcept;
    Profiler& operator=(Profiler&&) noexcept;
    
    void initialize(std::shared_ptr<DebugConfig> config) override;
    void start() override;
    void stop() override;
    void update() override;
    void beginProfile(const std::string& operation) override;
    void endProfile(const std::string& operation) override;
    void profileMemoryAllocation(size_t size, const std::string& context) override;
    void profileMemoryDeallocation(size_t size, const std::string& context) override;
    std::vector<ProfileData> getProfileData(const std::string& operation, const std::chrono::steady_clock::time_point& startTime, const std::chrono::steady_clock::time_point& endTime) const override;
    std::vector<Bottleneck> getBottlenecks() const override;
    std::map<std::string, double> getPerformanceMetrics() const override;
    std::string getPerformanceReport() const override;
    void clearProfileData() override;
    
    // Bottleneck detection and analysis
    void analyzeBottlenecks();
    void detectPerformanceIssues();
    void analyzeNeuralEventTiming();
    
    // Optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    
    // Performance statistics
    size_t getTotalProfileCount() const;
    double getAverageOperationDuration(const std::string& operation) const;
    double getPeakMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

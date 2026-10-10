#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>

namespace nlm {

// Function profile for performance analysis
struct FunctionProfile {
    std::string functionName;
    std::string fileName;
    int lineNumber;
    std::string moduleName;
    size_t callCount;
    double totalTime;
    double averageTime;
    double minTime;
    double maxTime;
    double totalMemory;
    double peakMemory;
    double cpuTime;
    Timestamp lastCalled;
    Timestamp firstCalled;
    std::string category; // "cpu", "memory", "io", "neural"
    std::map<std::string, double> customMetrics;
};

// Performance profile data
struct PerformanceProfile {
    std::string profileId;
    Timestamp startTime;
    Timestamp endTime;
    double totalTime;
    size_t totalMemoryAllocated;
    size_t totalMemoryDeallocated;
    double peakMemoryUsage;
    size_t totalFunctionCalls;
    std::map<std::string, FunctionProfile> functionProfiles;
    std::vector<std::pair<std::string, double>> systemMetrics; // "cpu_usage", "memory_usage", etc.
    std::map<std::string, std::vector<double>> timelineData; // "time", "cpu", "memory"
    std::vector<std::string> bottlenecks; // Detected bottlenecks
    std::vector<std::string> recommendations; // Optimization recommendations
};

// PerformanceProfiler - Performance profiling
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Profile control
    void startProfiling(const std::string& profileId = "default");
    void stopProfiling();
    bool isProfiling() const;
    void resetProfile();
    
    // Function profiling
    void startFunctionProfile(const std::string& functionName,
                              const std::string& fileName = "",
                              int lineNumber = 0,
                              const std::string& moduleName = "");
    void stopFunctionProfile();
    void recordFunctionTime(double time);
    void recordFunctionMemory(double memory);
    void recordFunctionCpuTime(double cpuTime);
    void recordCustomMetric(const std::string& name, double value);
    
    // System metrics profiling
    void recordSystemMetric(const std::string& name, double value);
    void recordTimelinePoint(const std::string& name, double value);
    
    // Profile analysis
    FunctionProfile getFunctionProfile(const std::string& functionName) const;
    PerformanceProfile getCurrentProfile() const;
    std::vector<std::string> getTopFunctions(size_t count = 10) const;
    std::vector<std::string> getBottlenecks() const;
    std::vector<std::string> getRecommendations() const;
    
    // Performance analysis
    double getAverageFunctionTime(const std::string& functionName) const;
    double getTotalTime(const std::string& functionName) const;
    double getTotalTime() const;
    size_t getFunctionCallCount(const std::string& functionName) const;
    size_t getTotalFunctionCalls() const;
    size_t getTotalMemoryAllocated() const;
    size_t getTotalMemoryDeallocated() const;
    double getPeakMemoryUsage() const;
    double getCpuUsage() const;
    double getMemoryUsage() const;
    
    // Hotspot detection
    std::vector<std::string> identifyHotspots() const;
    std::vector<std::string> identifyBottlenecks() const;
    std::vector<std::string> identifyMemoryBottlenecks() const;
    std::vector<std::string> identifyCpuBottlenecks() const;
    
    // Comparative analysis
    void compareWithProfile(const PerformanceProfile& otherProfile) const;
    std::string generateProfileReport() const;
    std::string generateHotspotReport() const;
    std::string generateMemoryReport() const;
    std::string generateCpuReport() const;
    
    // Profile export/import
    bool exportProfile(const std::string& filepath, const std::string& format = "json") const;
    bool importProfile(const std::string& filepath, const std::string& format = "json");
    
    // Profile management
    void saveProfile(const std::string& profileId);
    bool loadProfile(const std::string& profileId);
    std::vector<std::string> getProfileIds() const;
    void removeProfile(const std::string& profileId);
    void clearProfiles();
    
    // Control
    void enable(bool enable);
    bool isEnabled() const;
    void setSamplingRate(double rate); // Hz
    void setProfileThreshold(double threshold);
    void setMemoryTracking(bool enable);
    void setCpuTracking(bool enable);
    
    // Analysis
    std::map<std::string, double> analyzePerformanceTrends() const;
    std::vector<std::pair<std::string, double>> analyzeMemoryUsageTrends() const;
    std::vector<std::pair<std::string, double>> analyzeCpuUsageTrends() const;
    std::map<std::string, double> getPerformanceScore() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
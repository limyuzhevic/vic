#pragma once

#include "BuildProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

// Build performance metrics
struct BuildPerformance {
    std::string buildTarget;
    std::string buildType; // Debug, Release, etc.
    std::chrono::nanoseconds totalBuildTime;
    std::chrono::nanoseconds compileTime;
    std::chrono::nanoseconds linkTime;
    std::chrono::nanoseconds libraryTime;
    std::chrono::nanoseconds resourceTime;
    size_t sourceFiles;
    size_t headerFiles;
    size_t generatedFiles;
    double buildSpeed; // files/second
    double compilationSpeed; // files/second
    std::chrono::system_clock::time_point buildTime;
    std::string buildCommand;
    std::string compiler;
    std::string platform;
    
    BuildPerformance(const std::string& target, const std::string& type)
        : buildTarget(target), buildType(type), totalBuildTime(0), compileTime(0),
          linkTime(0), libraryTime(0), resourceTime(0), sourceFiles(0), headerFiles(0),
          generatedFiles(0), buildSpeed(0.0), compilationSpeed(0.0),
          buildTime(std::chrono::system_clock::now()) {}
};

// Build step performance metrics
struct BuildStepPerformance {
    std::string stepName;
    std::string dependency;
    std::chrono::nanoseconds stepTime;
    std::chrono::nanoseconds dependencyTime;
    bool stepSuccess;
    std::string errorMessage;
    std::chrono::system_clock::time_point timestamp;
    
    BuildStepPerformance(const std::string& name, const std::string& dep)
        : stepName(name), dependency(dep), stepTime(0), dependencyTime(0),
          stepSuccess(true), timestamp(std::chrono::system_clock::now()) {}
};

// Build performance profiling configuration
struct BuildProfilingConfig {
    bool enableBuildProfiling;           // Enable build performance profiling
    bool enableBuildStepProfiling;       // Enable build step profiling
    bool enableBuildResourceProfiling;    // Enable build resource profiling
    bool enableBuildOptimization;        // Enable build optimization suggestions
    size_t maxBuildProfiles;              // Maximum number of build profiles
    double buildTimeThreshold;            // Build time threshold (seconds)
    double compileTimeThreshold;         // Compilation time threshold (seconds)
    double linkTimeThreshold;             // Linking time threshold (seconds)
    bool enableRealTimeBuildMonitoring;   // Enable real-time build monitoring
    size_t profilingIntervalMs;           // Profiling interval in milliseconds
    
    BuildProfilingConfig()
        : enableBuildProfiling(true), enableBuildStepProfiling(true),
          enableBuildResourceProfiling(true), enableBuildOptimization(true),
          maxBuildProfiles(100), buildTimeThreshold(300.0),  // 5 minutes
          compileTimeThreshold(180.0), linkTimeThreshold(60.0),
          enableRealTimeBuildMonitoring(true), profilingIntervalMs(1000) {}
};

// Build profiler implementation
class BuildProfiler : public IBuildProfiler {
private:
    BuildProfilingConfig config_;
    std::atomic<bool> isRunning_{false};
    std::thread monitoringThread_;
    
    // Build performance tracking
    std::unordered_map<std::string, BuildPerformance> buildPerformances_;
    mutable std::mutex buildMutex_;
    
    // Build step performance tracking
    std::unordered_map<std::string, BuildStepPerformance> buildStepPerformances_;
    mutable std::mutex stepMutex_;
    
    // Current build metrics
    size_t totalBuilds_{0};
    size_t successfulBuilds_{0};
    double averageBuildTime_{0.0};
    double averageCompileTime_{0.0};
    double averageLinkTime_{0.0};
    mutable std::mutex metricsMutex_;
    
    // Monitoring state
    std::chrono::system_clock::time_point lastMonitoringTime_;
    size_t monitoringCount_ = 0;
    
public:
    BuildProfiler();
    ~BuildProfiler();
    
    // IBuildProfiler implementation
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void recordBuildPerformance(const std::string& target, const std::string& type,
                               const std::chrono::nanoseconds& totalTime,
                               const std::chrono::nanoseconds& compileTime,
                               const std::chrono::nanoseconds& linkTime,
                               const std::chrono::nanoseconds& libraryTime,
                               const std::chrono::nanoseconds& resourceTime,
                               size_t sourceFiles, size_t headerFiles, size_t generatedFiles,
                               const std::string& buildCommand, const std::string& compiler,
                               const std::string& platform) override;
    void recordBuildStepPerformance(const std::string& stepName, const std::string& dependency,
                                   const std::chrono::nanoseconds& stepTime,
                                   const std::chrono::nanoseconds& dependencyTime,
                                   bool stepSuccess, const std::string& errorMessage) override;
    
    std::vector<BuildPerformance> getBuildPerformances() const override;
    std::vector<BuildStepPerformance> getBuildStepPerformances() const override;
    
    std::string generateHtmlReport() const override;
    std::string generateJsonReport() const override;
    std::string generateTextReport() const override;
    
    // Configuration management
    void setConfig(const BuildProfilingConfig& config);
    BuildProfilingConfig getConfig() const;
    
    // Build system monitoring
    void updateBuildMetrics(size_t totalBuilds, size_t successfulBuilds);
    
    // Performance analysis
    void analyzeBuildPerformance(BuildPerformance& performance) const;
    void analyzeBuildStepPerformance(BuildStepPerformance& performance) const;
    
    // Report generation helpers
    std::string getCurrentTimestamp() const;
    double calculateBuildSpeed(BuildPerformance& performance) const;
    double calculateCompileSpeed(BuildPerformance& performance) const;
};

// Build system metrics aggregation
struct BuildSystemMetrics {
    size_t totalBuilds;
    size_t successfulBuilds;
    double averageBuildTime;
    double averageCompileTime;
    double averageLinkTime;
    double buildSuccessRate;
    std::chrono::system_clock::time_point timestamp;
};
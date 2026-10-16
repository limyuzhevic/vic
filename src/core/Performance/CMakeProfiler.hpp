#pragma once

#include "CMakeProfiler.hpp"
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

// CMake build performance metrics
struct CMakeBuildPerformance {
    std::string buildTarget;
    std::string cmakeGenerator;
    std::chrono::nanoseconds totalCMakeTime;
    std::chrono::nanoseconds configureTime;
    std::chrono::nanoseconds generateTime;
    std::chrono::nanoseconds buildTime;
    std::chrono::nanoseconds installTime;
    size_t sourceFiles;
    size_t headerFiles;
    size_t buildFiles;
    size_t generatedFiles;
    std::string cmakeCommand;
    std::string buildCommand;
    std::string sourceDirectory;
    std::string buildDirectory;
    double configureSpeed; // files/second
    double buildSpeed; // files/second
    std::chrono::system_clock::time_point buildTime;
    bool success;
    std::string errorMessage;
    
    CMakeBuildPerformance(const std::string& target)
        : buildTarget(target), totalCMakeTime(0), configureTime(0), generateTime(0),
          buildTime(0), installTime(0), sourceFiles(0), headerFiles(0),
          buildFiles(0), generatedFiles(0), configureSpeed(0.0), buildSpeed(0.0),
          buildTime(std::chrono::system_clock::now()), success(false) {}
};

// CMake step performance metrics
struct CMakeStepPerformance {
    std::string stepName;
    std::string dependency;
    std::chrono::nanoseconds stepTime;
    std::chrono::nanoseconds dependencyTime;
    bool stepSuccess;
    std::string output;
    std::chrono::system_clock::time_point timestamp;
    
    CMakeStepPerformance(const std::string& name, const std::string& dep)
        : stepName(name), dependency(dep), stepTime(0), dependencyTime(0),
          stepSuccess(true), timestamp(std::chrono::system_clock::now()) {}
};

// CMake performance profiling configuration
struct CMakeProfilingConfig {
    bool enableCMakeProfiling;           // Enable CMake build performance profiling
    bool enableCMakeStepProfiling;       // Enable CMake step profiling
    bool enableCMakeOptimization;        // Enable CMake optimization suggestions
    bool enableBuildDirectoryProfiling;   // Enable build directory profiling
    size_t maxCMakeProfiles;              // Maximum number of CMake profiles
    double configureTimeThreshold;       // Configure time threshold (seconds)
    double generateTimeThreshold;        // Generate time threshold (seconds)
    double buildTimeThreshold;            // Build time threshold (seconds)
    bool enableRealTimeCMakeMonitoring;   // Enable real-time CMake monitoring
    size_t profilingIntervalMs;           // Profiling interval in milliseconds
    
    CMakeProfilingConfig()
        : enableCMakeProfiling(true), enableCMakeStepProfiling(true),
          enableCMakeOptimization(true), enableBuildDirectoryProfiling(true),
          maxCMakeProfiles(100), configureTimeThreshold(60.0),  // 1 minute
          generateTimeThreshold(120.0), buildTimeThreshold(300.0),  // 5 minutes
          enableRealTimeCMakeMonitoring(true), profilingIntervalMs(1000) {}
};

// CMake profiler implementation
class CMakeProfiler : public ICMakeProfiler {
private:
    CMakeProfilingConfig config_;
    std::atomic<bool> isRunning_{false};
    std::thread monitoringThread_;
    
    // CMake performance tracking
    std::unordered_map<std::string, CMakeBuildPerformance> cmakeBuildPerformances_;
    mutable std::mutex buildMutex_;
    
    // CMake step performance tracking
    std::unordered_map<std::string, CMakeStepPerformance> cmakeStepPerformances_;
    mutable std::mutex stepMutex_;
    
    // Current CMake metrics
    size_t totalCMakeBuilds_{0};
    size_t successfulCMakeBuilds_{0};
    double averageConfigureTime_{0.0};
    double averageGenerateTime_{0.0};
    double averageBuildTime_{0.0};
    mutable std::mutex metricsMutex_;
    
    // Monitoring state
    std::chrono::system_clock::time_point lastMonitoringTime_;
    size_t monitoringCount_ = 0;
    
public:
    CMakeProfiler();
    ~CMakeProfiler();
    
    // ICMakeProfiler implementation
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void recordCMakeBuildPerformance(const std::string& target, const std::string& generator,
                                    const std::chrono::nanoseconds& totalTime,
                                    const std::chrono::nanoseconds& configureTime,
                                    const std::chrono::nanoseconds& generateTime,
                                    const std::chrono::nanoseconds& buildTime,
                                    const std::chrono::nanoseconds& installTime,
                                    size_t sourceFiles, size_t headerFiles, size_t buildFiles,
                                    size_t generatedFiles, const std::string& cmakeCommand,
                                    const std::string& buildCommand, const std::string& sourceDir,
                                    const std::string& buildDir) override;
    void recordCMakeStepPerformance(const std::string& stepName, const std::string& dependency,
                                   const std::chrono::nanoseconds& stepTime,
                                   const std::chrono::nanoseconds& dependencyTime,
                                   bool stepSuccess, const std::string& output) override;
    
    std::vector<CMakeBuildPerformance> getCMakeBuildPerformances() const override;
    std::vector<CMakeStepPerformance> getCMakeStepPerformances() const override;
    
    std::string generateHtmlReport() const override;
    std::string generateJsonReport() const override;
    std::string generateTextReport() const override;
    
    // Configuration management
    void setConfig(const CMakeProfilingConfig& config);
    CMakeProfilingConfig getConfig() const;
    
    // CMake system monitoring
    void updateCMakeMetrics(size_t totalBuilds, size_t successfulBuilds);
    
    // Performance analysis
    void analyzeCMakeBuildPerformance(CMakeBuildPerformance& performance) const;
    void analyzeCMakeStepPerformance(CMakeStepPerformance& performance) const;
    
    // Report generation helpers
    std::string getCurrentTimestamp() const;
    double calculateConfigureSpeed(CMakeBuildPerformance& performance) const;
    double calculateBuildSpeed(CMakeBuildPerformance& performance) const;
};

// CMake system metrics aggregation
struct CMakeSystemMetrics {
    size_t totalCMakeBuilds;
    size_t successfulCMakeBuilds;
    double averageConfigureTime;
    double averageGenerateTime;
    double averageBuildTime;
    double configureSuccessRate;
    std::chrono::system_clock::time_point timestamp;
};
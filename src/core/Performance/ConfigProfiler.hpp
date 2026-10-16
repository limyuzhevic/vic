#pragma once

#include "ConfigProfiler.hpp"
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

// Configuration loading performance metrics
struct ConfigLoadingPerformance {
    std::string configFile;
    std::chrono::nanoseconds totalLoadTime;
    std::chrono::nanoseconds parseTime;
    std::chrono::nanoseconds validationTime;
    std::chrono::nanoseconds applyTime;
    size_t fileSize;
    size_t keyCount;
    double loadSpeed; // MB/s
    std::chrono::system_clock::time_point loadTime;
    std::string format; // JSON, YAML, or .cfg
    
    ConfigLoadingPerformance(const std::string& file) 
        : configFile(file), totalLoadTime(0), parseTime(0), validationTime(0), 
          applyTime(0), fileSize(0), keyCount(0), loadSpeed(0.0), 
          format("Unknown"), loadTime(std::chrono::system_clock::now()) {}
};

// Configuration validation performance metrics
struct ConfigValidationPerformance {
    std::string validationType;
    std::chrono::nanoseconds validationTime;
    std::chrono::nanoseconds errorDetectionTime;
    std::chrono::nanoseconds errorCorrectionTime;
    size_t validatedKeys;
    size_t errorCount;
    size_t warningCount;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigValidationPerformance(const std::string& type)
        : validationType(type), validationTime(0), errorDetectionTime(0),
          errorCorrectionTime(0), validatedKeys(0), errorCount(0),
          warningCount(0), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration performance profiling configuration
struct ConfigProfilingConfig {
    bool enableConfigLoadingProfiling;      // Enable configuration loading profiling
    bool enableConfigValidationProfiling;   // Enable configuration validation profiling
    bool enableConfigChangeProfiling;       // Enable configuration change profiling
    bool enablePerformanceTracking;         // Enable performance tracking
    size_t maxConfigProfiles;               // Maximum number of config profiles
    double loadingSpeedThreshold;           // Loading speed threshold (MB/s)
    double validationTimeThreshold;         // Validation time threshold (ms)
    size_t configChangeThreshold;           // Configuration change threshold
    bool enableRealTimeMonitoring;           // Enable real-time config monitoring
    size_t profilingIntervalMs;              // Profiling interval in milliseconds
    
    ConfigProfilingConfig()
        : enableConfigLoadingProfiling(true), enableConfigValidationProfiling(true),
          enableConfigChangeProfiling(true), enablePerformanceTracking(true),
          maxConfigProfiles(1000), loadingSpeedThreshold(10.0),  // 10 MB/s
          validationTimeThreshold(100.0), configChangeThreshold(10),
          enableRealTimeMonitoring(true), profilingIntervalMs(1000) {}
};

// Config performance profiler implementation
class ConfigProfiler : public IConfigProfiler {
private:
    ConfigProfilingConfig config_;
    std::atomic<bool> isRunning_{false};
    std::thread monitoringThread_;
    
    // Config loading performance tracking
    std::unordered_map<std::string, ConfigLoadingPerformance> configLoadingPerformances_;
    mutable std::mutex loadingMutex_;
    
    // Config validation performance tracking
    std::unordered_map<std::string, ConfigValidationPerformance> configValidationPerformances_;
    mutable std::mutex validationMutex_;
    
    // Current config metrics
    size_t totalConfigFiles_{0};
    size_t totalConfigChanges_{0};
    double averageLoadSpeed_{0.0};
    double averageValidationTime_{0.0};
    mutable std::mutex metricsMutex_;
    
    // Monitoring state
    std::chrono::system_clock::time_point lastMonitoringTime_;
    size_t monitoringCount_ = 0;
    
public:
    ConfigProfiler();
    ~ConfigProfiler();
    
    // IConfigProfiler implementation
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void recordConfigLoading(const std::string& configFile, const std::chrono::nanoseconds& loadTime,
                            const std::chrono::nanoseconds& parseTime, const std::chrono::nanoseconds& validationTime,
                            const std::chrono::nanoseconds& applyTime, size_t fileSize, size_t keyCount,
                            const std::string& format) override;
    void recordConfigValidation(const std::string& validationType, const std::chrono::nanoseconds& validationTime,
                               const std::chrono::nanoseconds& errorDetectionTime, const std::chrono::nanoseconds& errorCorrectionTime,
                               size_t validatedKeys, size_t errorCount, size_t warningCount) override;
    
    std::vector<ConfigLoadingPerformance> getConfigLoadingPerformances() const override;
    std::vector<ConfigValidationPerformance> getConfigValidationPerformances() const override;
    
    std::string generateHtmlReport() const override;
    std::string generateJsonReport() const override;
    std::string generateTextReport() const override;
    
    // Configuration management
    void setConfig(const ConfigProfilingConfig& config);
    ConfigProfilingConfig getConfig() const;
    
    // Configuration system monitoring
    void updateConfigMetrics(size_t totalFiles, size_t totalChanges);
    
    // Performance analysis
    void analyzeConfigLoading(ConfigLoadingPerformance& performance) const;
    void analyzeConfigValidation(ConfigValidationPerformance& performance) const;
    
    // Report generation helpers
    std::string getCurrentTimestamp() const;
    double calculateLoadingSpeed(ConfigLoadingPerformance& performance) const;
    double calculateValidationEfficiency(ConfigValidationPerformance& performance) const;
};

// Config system metrics aggregation
struct ConfigSystemMetrics {
    size_t totalConfigFiles;
    size_t totalConfigChanges;
    double averageLoadSpeed;
    double averageValidationTime;
    double configChangeFrequency;
    std::chrono::system_clock::time_point timestamp;
};
// Copyright (c) 2026 NLM Project
// Logger configuration and management

#pragma once

#include "LoggerManager.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <chrono>
#include <thread>
#include <atomic>
#include <algorithm>

namespace nlm {

struct LogRotationInfo {
    std::string basePath;
    size_t maxFileSize;
    int maxFiles;
    std::chrono::system_clock::time_point lastRotation;
};

class LoggerManager {
public:
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }
    
    // Configuration
    void configure(const std::string& name, LogLevel level = LogLevel::Info,
                   const std::string& format = "json",
                   const std::string& outputPath = "",
                   bool async = true,
                   size_t queueSize = 10000);
    
    void setGlobalConfig(const std::string& format = "json",
                        bool async = true,
                        size_t queueSize = 10000);
    
    // Log level hierarchies and filtering
    void setMinimumLogLevel(LogLevel level);
    void addLogLevelFilter(const std::string& module, LogLevel level);
    void removeLogLevelFilter(const std::string& module);
    bool isLogLevelEnabled(const std::string& module, LogLevel level) const;
    
    // Performance logging thresholds
    void setPerformanceThreshold(const std::string& metric, double threshold);
    void setMemoryThreshold(size_t threshold);
    void setCpuThreshold(double threshold);
    
    // Async logging settings
    void setAsyncEnabled(bool enabled);
    void setQueueSize(size_t size);
    void flushAllLogs();
    
    // File rotation and management
    void setLogFileRotation(const std::string& basePath, size_t maxFileSize, int maxFiles);
    void enableLogRotation(bool enable);
    void cleanupOldLogFiles();
    
    // Correlation ID management
    uint64_t generateCorrelationId();
    std::string generateTraceId();
    std::string generateSpanId();
    
    // System monitoring
    void startSystemMonitoring();
    void stopSystemMonitoring();
    void updateSystemMetrics();
    
    // Debug console integration
    void setDebugConsole(std::shared_ptr<IDebugConsole> console);
    
    // Performance profiling
    void startProfiling(const std::string& name);
    void stopProfiling(const std::string& name);
    double getLastMeasurement(const std::string& name) const;
    
    // Global logger management
    std::shared_ptr<ILogger> getLogger(const std::string& name = "global");
    void addLogger(std::shared_ptr<ILogger> logger);
    void removeLogger(const std::string& name);
    void clearLoggers();
    
    // Utility functions
    std::string getCurrentTimestamp() const;
    std::string getCurrentProcessId() const;
    uint64_t getCurrentThreadId() const;
    double getCurrentMemoryUsage() const;
    double getCurrentCpuUsage() const;
    
private:
    LoggerManager();
    ~LoggerManager();
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
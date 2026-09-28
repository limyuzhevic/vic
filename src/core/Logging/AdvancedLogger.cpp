// Copyright (c) 2026 NLM Project
// JSON-based logger implementation with structured logging

#pragma once

#include "AdvancedLogger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <ctime>

namespace nlm {

class JsonLogger : public ILogger {
public:
    JsonLogger();
    explicit JsonLogger(LogLevel level);
    ~JsonLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    void setOutputFile(const std::string& filepath, bool rotate = false) override;
    void setMaxFileSize(size_t maxBytes) override;
    void setMaxFiles(int maxCount) override;
    void setTrackPerformance(bool track) override;
    void setTrackMemory(bool track) override;
    void setTrackCpu(bool track) override;
    uint64_t getNextCorrelationId() override;
    void setCorrelationId(uint64_t id) override;
    
    // JSON file output
    void writeToJsonFile(const LogEntry& entry);
    
    // File management for rotation
    void rotateFiles();
    void cleanupOldFiles();
    
    // Performance tracking
    void updatePerformanceMetrics(LogEntry& entry);
    void updateMemoryUsage(LogEntry& entry);
    void updateCpuUsage(LogEntry& entry);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    json logEntryToJson(const LogEntry& entry) const;
    std::string generateTimestamp() const;
    std::string getCurrentProcessId() const;
};

// Async logger for performance
class AsyncLogger : public ILogger {
public:
    AsyncLogger(size_t queueSize = 10000);
    ~AsyncLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    void setOutputFile(const std::string& filepath, bool rotate = false) override;
    void setMaxFileSize(size_t maxBytes) override;
    void setMaxFiles(int maxCount) override;
    void setTrackPerformance(bool track) override;
    void setTrackMemory(bool track) override;
    void setTrackCpu(bool track) override;
    uint64_t getNextCorrelationId() override;
    void setCorrelationId(uint64_t id) override;
    
    // Wait for all queued logs to be processed
    void flush();
    
    // Background thread management
    void stop();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Queue for async logging
    std::vector<LogEntry> logQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
    std::atomic<bool> running{true};
    std::thread workerThread;
    
    void workerFunction();
};

// Structured logger for domain-specific events
class StructuredLogger : public ILogger {
public:
    StructuredLogger();
    ~StructuredLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    void setOutputFile(const std::string& filepath, bool rotate = false) override;
    void setMaxFileSize(size_t maxBytes) override;
    void setMaxFiles(int maxCount) override;
    void setTrackPerformance(bool track) override;
    void setTrackMemory(bool track) override;
    void setTrackCpu(bool track) override;
    uint64_t getNextCorrelationId() override;
    void setCorrelationId(uint64_t id) override;
    
    // Domain-specific logging methods
    void logEvent(const StructuredLogEntry& event);
    void logNeuronEvent(NeuronId neuronId, const std::string& eventType, const json& data);
    void logSynapseEvent(SynapseId synapseId, const std::string& eventType, const json& data);
    void logMemoryEvent(const std::string& eventType, uint64_t bytes, const std::string& unit);
    void logPerformanceEvent(const std::string& eventType, double value, const std::string& unit);
    void logNetworkEvent(const std::string& eventType, uint64_t bytes, const std::string& direction);
    void logDiskEvent(const std::string& eventType, uint64_t bytes, const std::string& operation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
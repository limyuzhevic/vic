// Copyright (c) 2026 NLM Project
// Enhanced Logger with JSON output and structured logging

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Advanced log severity levels with numeric values for filtering
enum class LogLevel {
    Trace = 0,      // More detailed than Debug
    Debug = 1,      // Detailed information for debugging
    Info = 2,       // General information
    Notice = 3,     // Important events
    Warning = 4,    // Warning conditions
    Error = 5,      // Error conditions
    Critical = 6,   // Critical conditions
    Alert = 7,      // Action must be taken immediately
    Emergency = 8   // System unusable
};

// Enhanced log entry with structured data
struct LogEntry {
    uint64_t correlationId = 0;              // For tracing requests across systems
    LogLevel level;
    std::string message;
    std::string file;
    int line = 0;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    uint64_t threadId = 0;
    double cpuUsage = 0.0;                   // CPU usage at time of logging
    uint64_t memoryUsage = 0;                // Memory usage in bytes
    std::string processId;                   // Process identifier
    
    // Additional structured data for rich context
    std::vector<std::pair<std::string, std::string>> tags;
    std::vector<std::pair<std::string, double>> metrics;
    
    // Performance metrics
    std::chrono::nanoseconds processingTime;
    std::chrono::nanoseconds elapsedSinceLast;
    
    // Custom fields for domain-specific data
    std::string module;                      // Which module generated this log
    std::string severity;                    // Human-readable severity
    
    // Performance and resource tracking
    double memoryPercent = 0.0;
    uint64_t networkBytes = 0;               // Network I/O if applicable
    uint64_t diskBytes = 0;                  // Disk I/O if applicable
    
    // For performance tracing
    std::string traceId;
    std::string spanId;
    bool isPerformanceEvent = false;
    
    LogEntry& addTag(const std::string& key, const std::string& value) {
        tags.emplace_back(key, value);
        return *this;
    }
    
    LogEntry& addMetric(const std::string& key, double value) {
        metrics.emplace_back(key, value);
        return *this;
    }
};

// JSON serialization for LogEntry
inline void to_json(json& j, const LogEntry& entry) {
    j = json{
        {"correlation_id", entry.correlationId},
        {"level", static_cast<int>(entry.level)},
        {"message", entry.message},
        {"file", entry.file},
        {"line", entry.line},
        {"function", entry.function},
        {"timestamp", std::chrono::system_clock::to_time_t(entry.timestamp)},
        {"thread_id", entry.threadId},
        {"cpu_usage", entry.cpuUsage},
        {"memory_usage", entry.memoryUsage},
        {"process_id", entry.processId},
        {"tags", json::array()},  // Will be filled by manual processing
        {"metrics", json::array()}, // Will be filled by manual processing
        {"processing_time_ns", entry.processingTime.count()},
        {"elapsed_since_last_ns", entry.elapsedSinceLast.count()},
        {"module", entry.module},
        {"severity", entry.severity},
        {"memory_percent", entry.memoryPercent},
        {"network_bytes", entry.networkBytes},
        {"disk_bytes", entry.diskBytes},
        {"trace_id", entry.traceId},
        {"span_id", entry.spanId},
        {"is_performance_event", entry.isPerformanceEvent}
    };
    
    // Add tags and metrics manually
    if (!entry.tags.empty()) {
        json tags_json = json::array();
        for (const auto& tag : entry.tags) {
            tags_json.push_back(json::object({{"key", tag.first}, {"value", tag.second}}));
        }
        j["tags"] = tags_json;
    }
    
    if (!entry.metrics.empty()) {
        json metrics_json = json::array();
        for (const auto& metric : entry.metrics) {
            metrics_json.push_back(json::object({{"key", metric.first}, {"value", metric.second}}));
        }
        j["metrics"] = metrics_json;
    }
}

// Enhanced ILogger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0; // "text", "json", "structured"
    virtual void setOutputFile(const std::string& filepath, bool rotate = false) = 0;
    virtual void setMaxFileSize(size_t maxBytes) = 0;
    virtual void setMaxFiles(int maxCount) = 0;
    
    // Performance and memory tracking
    virtual void setTrackPerformance(bool track) = 0;
    virtual void setTrackMemory(bool track) = 0;
    virtual void setTrackCpu(bool track) = 0;
    
    // Correlation ID support
    virtual uint64_t getNextCorrelationId() = 0;
    virtual void setCorrelationId(uint64_t id) = 0;
};

// Structured log entry for domain-specific data
struct StructuredLogEntry {
    std::string eventType;                   // e.g., "neuron_created", "synaptic_connection"
    std::string module;                      // Which module generated this
    uint64_t entityId = 0;                   // ID of entity being logged
    std::string entityType;                  // Type of entity
    json data;                               // Domain-specific data
    double value = 0.0;                      // Single numeric value if applicable
    std::string unit;                        // Unit for the value (e.g., "ms", "Hz", "MB")
    std::chrono::system_clock::time_point timestamp;
};
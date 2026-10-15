#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <json/json.h> // For structured logging
#include "../Error/ErrorBase.hpp"
#include "../Error/ErrorManager.hpp"

namespace nlm {

// Enhanced Log severity levels with error severity integration
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Structured log entry for enhanced logging
struct StructuredLogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string component;
    std::string operation;
    Json::Value data;
    std::chrono::milliseconds executionTime;
    size_t memoryUsage;
    std::string correlationId;
    std::string parentLogId;
    std::string errorId;
};

// Performance log structure
struct PerformanceLogEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string metricName;
    double value;
    std::string unit;
    std::string component;
    std::map<std::string, std::string> tags;
};

// Enhanced Log entry structure with metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string component;
    std::string operation;
    Json::Value data;
    std::chrono::milliseconds executionTime;
    size_t memoryUsage;
    std::string correlationId;
    std::string errorId;
};

// Event types for system events
enum class EventType {
    StateChange,
    ConfigurationUpdate,
    SystemStartup,
    SystemShutdown,
    ErrorOccurred,
    RecoveryCompleted,
    PerformanceMetric,
    MemoryAllocation,
    MemoryDeallocation,
    PluginLoaded,
    PluginUnloaded,
    UserAction,
    SecurityEvent,
    Custom
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

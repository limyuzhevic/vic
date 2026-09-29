#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>
#include <atomic>
#include <thread>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;

namespace nlm {

// Enhanced Log severity levels (including FATAL)
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Structured log entry with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    // Additional metadata for structured logging
    std::string correlationId;
    std::string component;
    std::map<std::string, std::string> metadata;
    std::string threadId;
    uint64_t sequenceNumber;
    
    // JSON serialization
    json toJson() const {
        json j;
        j["level"] = static_cast<int>(level);
        j["message"] = message;
        j["file"] = file;
        j["line"] = line;
        j["function"] = function;
        j["timestamp"] = std::chrono::system_clock::to_time_t(timestamp);
        j["correlationId"] = correlationId;
        j["component"] = component;
        j["metadata"] = metadata;
        j["threadId"] = threadId;
        j["sequenceNumber"] = sequenceNumber;
        return j;
    }
    
    // JSON deserialization
    static LogEntry fromJson(const json& j) {
        LogEntry entry;
        entry.level = static_cast<LogLevel>(j.value("level", 1));
        entry.message = j.value("message", "");
        entry.file = j.value("file", "");
        entry.line = j.value("line", 0);
        entry.function = j.value("function", "");
        if (j.contains("timestamp")) {
            auto time_t_val = j["timestamp"].get<time_t>();
            entry.timestamp = std::chrono::system_clock::from_time_t(time_t_val);
        }
        entry.correlationId = j.value("correlationId", "");
        entry.component = j.value("component", "");
        entry.metadata = j.value("metadata", std::map<std::string, std::string>{});
        entry.threadId = j.value("threadId", "");
        entry.sequenceNumber = j.value("sequenceNumber", 0ULL);
        return entry;
    }
};

// Enhanced ILogger interface with structured logging support
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // New structured logging methods
    virtual void logJson(const json& entry) = 0;
    virtual void flush() = 0;
};

// Enhanced ConsoleLogger with structured logging support
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Structured logging methods
    void logJson(const json& entry) override;
    void flush() override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Correlation ID management
    void setCorrelationId(const std::string& correlationId);
    std::string getCorrelationId() const;
    
    // Sequence number management
    uint64_t getNextSequenceNumber();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void logStructured(const LogEntry& entry);
};

// Enhanced structured logger implementation
class StructuredLogger : public ILogger {
public:
    StructuredLogger(const std::string& filepath, bool append = true);
    ~StructuredLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Structured logging methods
    void logJson(const json& entry) override;
    void flush() override;
    
    // File operations
    void close();
    bool isOpen() const;
    
private:
    std::ofstream fileOut;
    std::mutex mutex;
    LogLevel level = LogLevel::Info;
    std::string filepath;
    bool appendMode;
    uint64_t sequenceNumber = 0;
    std::chrono::system_clock::time_point lastFlushTime;
};

// Main Logger class with enhanced features
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
             const std::string& function = "", 
             const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods with metadata support
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::map<std::string, std::string>& metadata = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Enhanced logging methods
    void logStructured(const LogEntry& entry);
    void logJson(const json& entry);
    
    // Correlation ID management
    void setCorrelationId(const std::string& correlationId);
    std::string getCorrelationId() const;
    
    // Component name management
    void setComponent(const std::string& component);
    std::string getComponent() const;
    
    // Performance metrics
    struct PerformanceMetrics {
        uint64_t totalLogs;
        uint64_t logsByLevel[6]; // Debug, Info, Warning, Error, Critical, Fatal
        double logsPerSecond;
        std::chrono::system_clock::time_point lastResetTime;
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

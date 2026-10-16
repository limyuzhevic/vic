#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sys/types.h>
#include <sys/sysinfo.h>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure for structured logging
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Structured data for JSON logging
    nlohmann::json structuredData;
    
    // Performance timing
    std::chrono::duration<double> duration; // 0 if not timing
    bool isTimed; // Whether this log entry includes timing
    
    LogEntry() 
        : level(LogLevel::Info), line(0), isTimed(false), duration(0.0) {}
    
    LogEntry(LogLevel lvl, const std::string& msg, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), 
          function(function), timestamp(std::chrono::system_clock::now()),
          isTimed(false), duration(0.0) {}
};

// Performance metrics structure
struct PerformanceMetrics {
    std::string operation;
    std::chrono::duration<double> duration;
    size_t memoryUsage; // in bytes
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    PerformanceMetrics(const std::string& op, std::chrono::duration<double> dur)
        : operation(op), duration(dur), memoryUsage(0), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Enhanced logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Structured logging methods
    virtual void logStructured(LogLevel level, const std::string& message,
                               const nlohmann::json& data,
                               const std::string& file = "", int line = 0,
                               const std::string& function = "");
    
    // Performance logging
    virtual void logPerformance(const PerformanceMetrics& metrics);
    
    // Performance timing
    virtual std::unique_ptr<class ScopedTimer> startTiming(const std::string& operation);
};

// Performance timer for automatic timing
class ScopedTimer {
public:
    ScopedTimer(const std::string& operation, ILogger* logger = nullptr)
        : operation(operation), logger(logger), start(std::chrono::high_resolution_clock::now()) {}
    
    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - start).count();
        
        if (logger) {
            PerformanceMetrics metrics(operation, std::chrono::duration<double>(duration));
            logger->logPerformance(metrics);
        }
    }
    
    double getElapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
private:
    std::string operation;
    ILogger* logger;
    std::chrono::high_resolution_clock::time_point start;
};

// Enhanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message,
                      const nlohmann::json& data,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "") override;
    
    // Performance logging
    void logPerformance(const PerformanceMetrics& metrics) override;
    
    // Performance timing
    std::unique_ptr<ScopedTimer> startTiming(const std::string& operation) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatStructuredLog(const LogEntry& entry) const;
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
    
    // Structured logging convenience methods
    void logStructured(LogLevel level, const std::string& message,
                      const nlohmann::json& data,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Performance logging
    void logPerformance(const PerformanceMetrics& metrics);
    
    // Performance timing
    std::unique_ptr<ScopedTimer> startTiming(const std::string& operation);
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance monitoring helpers
    static double getSystemMemoryUsage();
    static std::string getCurrentTimestamp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Structured data support
    LogStream& operator<<(const nlohmann::json& json) {
        structuredData_ = json;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    nlohmann::json structuredData_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
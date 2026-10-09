#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <shared_mutex>
#include <optional>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
};

// Performance metrics for log operations
struct LogPerformanceMetrics {
    std::atomic<size_t> debugCount{0};
    std::atomic<size_t> infoCount{0};
    std::atomic<size_t> warningCount{0};
    std::atomic<size_t> errorCount{0};
    std::atomic<size_t> criticalCount{0};
    std::atomic<double> averageLatency{0.0};
    std::atomic<size_t> totalBytesWritten{0};
    std::chrono::steady_clock::time_point lastUpdate;
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Performance monitoring interface
    virtual bool isPerformanceMonitoringEnabled() const { return false; }
    virtual void enablePerformanceMonitoring(bool enable) {}
    virtual LogPerformanceMetrics getPerformanceMetrics() const { return LogPerformanceMetrics{}; }
    virtual void resetPerformanceMetrics() {}
    virtual std::unordered_map<std::string, double> getPerformanceAnalysis() const { return {}; }
    
    // Logging category support
    virtual bool isCategoryEnabled(const std::string& category) const { return false; }
    virtual void enableCategory(const std::string& category, bool enable) {}
};

// Console logger implementation
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
    
    // Performance monitoring interface
    bool isPerformanceMonitoringEnabled() const override;
    void enablePerformanceMonitoring(bool enable) override;
    LogPerformanceMetrics getPerformanceMetrics() const override;
    void resetPerformanceMetrics() override;
    std::unordered_map<std::string, double> getPerformanceAnalysis() const override;
    
    // Logging category support
    bool isCategoryEnabled(const std::string& category) const override;
    void enableCategory(const std::string& category, bool enable) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    // Performance timing for individual log operations
    std::chrono::steady_clock::time_point measureLogTime(std::chrono::steady_clock::time_point start) const;
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
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    LogPerformanceMetrics getGlobalPerformanceMetrics() const;
    void resetGlobalPerformanceMetrics();
    std::unordered_map<std::string, double> getGlobalPerformanceAnalysis() const;
    
    // Logging category support
    void enableCategory(const std::string& category, bool enable);
    bool isCategoryEnabled(const std::string& category) const;
    
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
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Performance timing helper
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& operationName = "");
    ~PerformanceTimer();
    
    void setOperationName(const std::string& name);
    const std::string& getOperationName() const;
    
    // Get the measured duration in microseconds
    long long getDurationMicroseconds() const;
    
    // Get the measured duration in seconds
    double getDurationSeconds() const;
    
    // Reset timer
    void reset();
    
private:
    std::chrono::steady_clock::time_point startTime;
    std::string operationName;
};

} // namespace nlm

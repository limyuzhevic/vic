#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
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
    std::unordered_map<std::string, std::string> metadata; // For structured logging
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual std::string getFormat() const = 0;
    virtual void setFilter(const std::function<bool(const LogEntry&)>& filter) = 0;
    virtual void clearFilter() = 0;
};

// Advanced console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    std::string getFormat() const override;
    void setFilter(const std::function<bool(const LogEntry&)>& filter) override;
    void clearFilter() override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // File output management
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    bool isFileOutputOpen() const;
    
    // Log rotation
    void setMaxFileSize(size_t maxSizeBytes);
    void setFileRotationCount(int rotationCount);
    size_t getMaxFileSize() const;
    int getFileRotationCount() const;
    
    // Performance timing
    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    std::chrono::milliseconds getLastDuration(const std::string& operation) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
    bool shouldLog(const LogEntry& entry) const;
};

// Log aggregator for batch processing
class LogAggregator {
public:
    LogAggregator();
    ~LogAggregator();
    
    void addEntry(const LogEntry& entry);
    void processEntries();
    void setBatchSize(size_t batchSize);
    void setFlushInterval(std::chrono::milliseconds interval);
    size_t getEntryCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    void addConsoleLogger(LogLevel level = LogLevel::Info);
    void addFileLogger(const std::string& filepath, LogLevel level = LogLevel::Info);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance timing
    void startTimer(const std::string& operation, const std::string& category = "");
    void stopTimer(const std::string& operation);
    std::chrono::milliseconds getLastDuration(const std::string& operation) const;
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration integration
    void configureFromConfig(const class Config& config);
    
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

// Performance timing macro
#define NLM_TIME_OPERATION(operation) \
    nlm::ScopedTimer timer_##operation(#operation); \
    nlm::Logger::getGlobal().startTimer(#operation, "performance"); \
    timer_##operation.start(); \
    [ & ]()

// Exception for log-related errors
class LogException : public std::runtime_error {
public:
    explicit LogException(const std::string& message) : std::runtime_error(message) {}
};

// Scoped timer for performance measurement
class ScopedTimer {
public:
    ScopedTimer(const std::string& operation);
    ~ScopedTimer();
    void start();
    void stop();
    std::chrono::milliseconds duration() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

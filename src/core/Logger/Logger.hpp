#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <optional>
#include <filesystem>

namespace nlm {

// Log severity levels with string conversion
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Configuration for logging behavior
struct LoggerConfig {
    LogLevel defaultLevel = LogLevel::Info;
    bool useColors = true;
    bool asyncLogging = false;
    size_t maxFileSize = 10 * 1024 * 1024; // 10MB
    size_t maxFileCount = 5;
    bool logToConsole = true;
    bool logToFile = false;
    std::string logFilePath;
    bool enableTimestamps = true;
    bool enableThreadIds = false;
    std::string logPattern = "[%level] [%timestamp] %message";
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    
    LogEntry() : level(LogLevel::Info), line(0), threadId(std::this_thread::get_id()) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& file = "", 
             int line = 0, const std::string& function = "")
        : level(lvl), message(msg), file(file), line(line), function(function), 
          timestamp(std::chrono::system_clock::now()), threadId(std::this_thread::get_id()) {}
};

// Enhanced logger interface with error reporting
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual bool log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
    virtual std::string getName() const = 0;
    virtual bool configure(const LoggerConfig& config) = 0;
    virtual LoggerConfig getConfiguration() const = 0;
    
    // Error handling
    virtual std::vector<std::string> getLastErrors() const { return {}; }
    virtual void clearErrors() {}
    virtual bool hasErrors() const { return false; }
};

// Exception type for logger errors
class LoggerError : public std::runtime_error {
public:
    LoggerError(const std::string& message, const std::string& source = "")
        : std::runtime_error("Logger Error: " + message + (source.empty() ? "" : " [Source: " + source + "]")) {}
};

// Console logger implementation with enhanced error handling
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(const LoggerConfig& config);
    ~ConsoleLogger() override;
    
    // ILogger interface
    bool log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    std::string getName() const override { return "ConsoleLogger"; }
    bool configure(const LoggerConfig& config) override;
    LoggerConfig getConfiguration() const override;
    
    // Console-specific features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // File output with error handling
    bool setOutputFile(const std::string& filepath, bool append = true);
    void closeOutputFile();
    bool isOutputFileOpen() const;
    std::string getOutputFilePath() const;
    
    // Error handling
    std::vector<std::string> getLastErrors() const override;
    void clearErrors() override;
    bool hasErrors() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatMessage(const LogEntry& entry) const;
    bool writeToFile(const std::string& message);
    void logError(const std::string& error, const std::string& operation = "");
};

// Formatted logger implementation
class FormattedLogger : public ILogger {
public:
    FormattedLogger(const std::string& name);
    ~FormattedLogger() override;
    
    // ILogger interface
    bool log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    bool isEnabled(LogLevel level) const override;
    std::string getName() const override;
    bool configure(const LoggerConfig& config) override;
    LoggerConfig getConfiguration() const override;
    
    // Format configuration
    void setLogPattern(const std::string& pattern);
    std::string getLogPattern() const;
    
    // Filter configuration
    void addLevelFilter(LogLevel level, bool include);
    void setRegexFilter(const std::string& pattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    bool matchesFilters(const LogEntry& entry) const;
    std::string applyPattern(const LogEntry& entry) const;
};

// Main Logger class with enhanced thread safety and error handling
class Logger {
public:
    Logger();
    explicit Logger(const LoggerConfig& config);
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Logger management
    bool addLogger(std::shared_ptr<ILogger> logger);
    bool removeLogger(const std::string& name);
    bool removeLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    
    // Global logger configuration
    bool configureGlobal(const LoggerConfig& config);
    LoggerConfig getGlobalConfiguration() const;
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Logging functions with error checking
    bool log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Enhanced logging with error checking
    bool log(const LogEntry& entry);
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Global logger instance management
    static Logger& getGlobal();
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Error handling
    std::vector<std::string> getGlobalErrors() const;
    void clearGlobalErrors();
    bool hasGlobalErrors() const;
    
    // Configuration persistence
    bool saveConfiguration(const std::string& filepath) const;
    bool loadConfiguration(const std::string& filepath);
    
    // Statistics and diagnostics
    struct LoggerStats {
        size_t totalLogs = 0;
        size_t errorLogs = 0;
        size_t warningLogs = 0;
        size_t infoLogs = 0;
        size_t debugLogs = 0;
        size_t criticalLogs = 0;
        size_t droppedLogs = 0;
        size_t fileWriteErrors = 0;
        std::chrono::system_clock::time_point startTime;
    };
    
    LoggerStats getStatistics() const;
    void resetStatistics();
    
    // Advanced features
    void enableStructuredLogging(bool enable);
    void setMaxLogSize(size_t bytes);
    bool rotateLogFiles();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void logToAllLoggers(const LogEntry& entry, std::vector<std::string>& errors);
    bool validateLoggerConfig(const LoggerConfig& config) const;
};

// Enhanced convenience macros with error checking
#define NLM_LOG_CHECK(logger) \
    if (!(logger).isEnabled(nlm::LogLevel::Debug)) ; else \

#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with error checking
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function, Logger& logger = Logger::getGlobal());
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        if (oss_) {
            oss_ << value;
        }
        return *this;
    }
    
    LogStream& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (oss_) {
            *oss_ << manip;
        }
        return *this;
    }
    
    // Check for stream errors
    bool good() const { return oss_ && !oss_->fail(); }
    
private:
    LogLevel level_;
    std::unique_ptr<std::ostringstream> oss_;
    std::string file_;
    int line_;
    std::string function_;
    Logger* logger_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Specialized logging macros for different contexts
#define NLM_LOG_PERFORMANCE(msg) nlm::Logger::getGlobal().info("[PERF] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_MEMORY(msg) nlm::Logger::getGlobal().info("[MEMORY] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_NETWORK(msg) nlm::Logger::getGlobal().info("[NETWORK] " + msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_COMPUTATION(msg) nlm::Logger::getGlobal().info("[COMPUTATION] " + msg, __FILE__, __LINE__, __func__)

// Logging helper functions
namespace LogHelpers {
    // Performance logging
    void logPerformance(const std::string& operation, double duration, 
                       const std::string& details = "");
    
    // Memory logging
    void logMemoryAllocation(size_t bytes, const std::string& context = "");
    void logMemoryDeallocation(size_t bytes, const std::string& context = "");
    
    // Error context
    struct ErrorContext {
        std::string operation;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> stackTrace;
    };
    
    ErrorContext createErrorContext(const std::string& operation);
    std::string formatErrorContext(const ErrorContext& context);
    
    // Assertion logging
    void logAssertion(bool condition, const std::string& message,
                     const std::string& file, int line, const std::string& function);
}

} // namespace nlm

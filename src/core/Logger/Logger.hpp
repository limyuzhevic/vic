#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include "EnhancedLogger.hpp"

namespace nlm {

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include "EnhancedLogger.hpp"

namespace nlm {

// Enhanced logger system header
// Includes all production-ready logging features with backward compatibility

// Re-export enhanced logger types for ease of use
using EnhancedLogger = nlm::Logger;
using EnhancedLogLevel = nlm::LogLevel;
using EnhancedLogEntry = nlm::LogEntry;
using EnhancedILogger = nlm::ILogger;
using EnhancedLoggerFactory = nlm::LoggerFactory;
using EnhancedLoggerUtils = nlm::LoggerUtils;
using EnhancedLogMetrics = nlm::LogMetrics;
using EnhancedLogCategory = nlm::LogCategory;

// Convenience typedefs for backward compatibility
using LogEntry = EnhancedLogEntry;
using ILogger = EnhancedILogger;
using LogLevel = EnhancedLogLevel;

// Original ConsoleLogger (backward compatibility)
class ConsoleLogger : public EnhancedILogger {
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class (enhanced with backward compatibility)
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
    
    // Enhanced logger factory methods
    static std::unique_ptr<Logger> createEnhancedLogger();
    static std::unique_ptr<Logger> createProductionLogger(const nlm::LoggerConfig& config = nlm::LoggerConfig());
    
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

// Enhanced logging macros with category support
#define NLM_LOG_DEBUG_CAT(msg, cat) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_INFO_CAT(msg, cat) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_WARNING_CAT(msg, cat) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_ERROR_CAT(msg, cat) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, cat)
#define NLM_LOG_CRITICAL_CAT(msg, cat) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, cat)

// Performance logging macros
#define NLM_LOG_PERFORMANCE(op, duration) \
    nlm::Logger::getGlobal().logPerformance(op, duration, __FILE__, __LINE__, __func__)

// Exception logging macros
#define NLM_LOG_EXCEPTION(msg) \
    nlm::Logger::getGlobal().logException(msg, std::current_exception(), __FILE__, __LINE__, __func__)

// Structured logging helper
class LogStreamStructured {
public:
    LogStreamStructured(LogLevel level, const std::string& file, int line, const std::string& function,
                       const std::string& category = "");
    ~LogStreamStructured();
    
    template<typename T>
    LogStreamStructured& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    LogStreamStructured& operator<<(const std::pair<std::string, std::string>& field);
    LogStreamStructured& field(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string category_;
    std::unordered_map<std::string, std::string> fields_;
};

#define NLM_LOG_STREAM_STRUCT(level, category) nlm::LogStreamStructured(level, __FILE__, __LINE__, __func__, category)

// Factory methods for easy creation of different logger types
class LoggerFactory {
public:
    static std::unique_ptr<ILogger> createConsoleLogger(LogLevel level = LogLevel::Info, bool useColors = true);
    static std::unique_ptr<ILogger> createFileLogger(LogLevel level = LogLevel::Info,
                                                     const std::string& filepath = "",
                                                     size_t maxFileSize = 10 * 1024 * 1024,
                                                     int maxFiles = 5,
                                                     bool jsonFormat = false);
    static std::unique_ptr<ILogger> createQueueLogger(LogLevel level = LogLevel::Info,
                                                      size_t queueSize = 10000,
                                                      size_t workerThreads = 2,
                                                      std::chrono::milliseconds flushInterval = std::chrono::milliseconds(100));
    static std::unique_ptr<ILogger> createMetricsLogger(LogLevel level = LogLevel::Info,
                                                        const std::string& outputFile = "metrics.log");
    static std::unique_ptr<ILogger> createDebugLogger(LogLevel level = LogLevel::Info,
                                                      const std::string& outputFile = "debug.log");
    static std::unique_ptr<ILogger> createStructuredLogger(LogLevel level = LogLevel::Info,
                                                           const std::string& outputFile = "structured.log");
};

// Configuration management
class LoggerConfigManager {
public:
    static LoggerConfigManager& getInstance();
    
    void setConfig(const nlm::LoggerConfig& config);
    const nlm::LoggerConfig& getConfig() const;
    
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    
private:
    LoggerConfigManager() = default;
    ~LoggerConfigManager() = default;
    LoggerConfigManager(const LoggerConfigManager&) = delete;
    LoggerConfigManager& operator=(const LoggerConfigManager&) = delete;
    
    nlm::LoggerConfig config_;
    mutable std::mutex configMutex;
};

} // namespace nlm


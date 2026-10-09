#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <variant>
#include <functional>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <deque>
#include <algorithm>
#include <filesystem>

// Platform-specific headers
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace nlm {

// Enhanced LogLevel enum with additional levels
enum class LogLevel {
    Trace = -1,
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Alert = 5,
    Emergency = 6,
    Off = 999
};

// Log entry with enhanced metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string processId;
    std::string module;
    uint64_t correlationId;
    
    // Performance metrics
    std::chrono::microseconds executionTime;
    uint64_t memoryUsage;
    
    // Structured data
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, double> numericFields;
    
    // Custom context
    std::unordered_map<std::string, std::string> context;
    
    // Exception information (if any)
    std::string exceptionType;
    std::string exceptionMessage;
    std::string stackTrace;
};

// Forward declarations
class Logger;

// Enterprise logging system (forward declared for compatibility)
namespace Enterprise {
    class CorrelationManager;
    class MemoryTracker;
    class PerformanceLogger;
}

// Original Logger interface for backward compatibility
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Original ConsoleLogger implementation
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Main Logger class (original implementation extended)
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink (backward compatibility)
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
    
    // Enterprise logging methods
    void setEnterpriseMode(bool enabled);
    bool isEnterpriseMode() const;
    
    // Performance logging with timing
    Enterprise::PerformanceLogger startPerformanceLog(const std::string& module);
    
    // Memory tracking
    void logMemoryStats(const std::string& context = "");
    
    // System health check
    bool performHealthCheck();
    
    // Correlation ID management
    void startCorrelation(const std::string& context);
    void endCorrelation();
    uint64_t getCurrentCorrelationId() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros (original)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper (original)
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

// Enterprise logging macros (new)
#define NLM_PERFORMANCE_LOG(module) nlm::Enterprise::PerformanceLogger performanceLogger(module)
#define NLM_PERFORMANCE_START(module, operation) performanceLogger.startTiming(operation)
#define NLM_PERFORMANCE_END(operation) performanceLogger.endTiming(operation)

#define NLM_CORRELATION_START(context) nlm::getCorrelationManager().startCorrelation(context)
#define NLM_CORRELATION_END() nlm::getCorrelationManager().endCorrelation()

#define NLM_HEALTH_CHECK() nlm::Logger::getGlobal().performHealthCheck()

#define NLM_LOG_WITH_FIELDS(level, message, ...) \
    do { \
        std::unordered_map<std::string, std::string> fields = {__VA_ARGS__}; \
        nlm::Logger::getGlobal().log(level, message, __FILE__, __LINE__, __func__, "", fields); \
    } while(0)

#define NLM_LOG_DEBUG_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Debug, message, __VA_ARGS__)
#define NLM_LOG_INFO_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Info, message, __VA_ARGS__)
#define NLM_LOG_WARNING_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Warning, message, __VA_ARGS__)
#define NLM_LOG_ERROR_WITH_FIELDS(message, ...) NLM_LOG_WITH_FIELDS(nlm::LogLevel::Error, message, __VA_ARGS__)

// Exception handling macros
#define NLM_THROW(category, severity, message) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        nlm::reportError(exc); \
        throw exc; \
    } while(0)

#define NLM_THROW_WITH_CONTEXT(category, severity, message, context) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        exc << context; \
        nlm::reportError(exc); \
        throw exc; \
    } while(0)

#define NLM_RECOVERABLE_THROW(category, severity, message) \
    do { \
        nlm::NLMException exc(message, category, severity, __FILE__, __LINE__, __func__); \
        if (exc.canRecover()) { \
            nlm::reportError(exc, true); \
            return exc; \
        } else { \
            nlm::reportError(exc); \
            throw exc; \
        } \
    } while(0)

// Global error handling functions
void reportError(const NLMException& exception, bool autoRecover = false);
void reportError(const std::string& message, ExceptionCategory category, 
                 ExceptionSeverity severity = ExceptionSeverity::ERROR);

// Global enterprise components
Enterprise::CorrelationManager& getCorrelationManager();
MemoryTracker& getMemoryTracker();

} // namespace nlm

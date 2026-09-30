#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <ctime>
#include <iomanip>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Error codes for structured error handling
enum class ErrorCode {
    SUCCESS = 0,
    UNKNOWN_ERROR = 1,
    CONFIGURATION_ERROR = 2,
    BRAIN_OPERATION_ERROR = 3,
    MEMORY_ERROR = 4,
    PLASTICITY_ERROR = 5,
    NEUROMODULATION_ERROR = 6,
    MEMORY_SYSTEM_ERROR = 7,
    PREDICTION_ERROR = 8,
    DEVELOPMENT_ERROR = 9,
    SIMULATION_ERROR = 10,
    FILE_IO_ERROR = 11,
    NETWORK_ERROR = 12,
    VALIDATION_ERROR = 13
};

// Enhanced log entry with additional metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::unordered_map<std::string, std::string> context;
    std::string operationId;
    double durationMs;
    
    LogEntry() : level(LogLevel::Info), line(0), durationMs(0.0) {
        timestamp = std::chrono::system_clock::now();
        threadId = "main";
        durationMs = 0.0;
    }
};

// Enhanced error structure
struct NLMError {
    ErrorCode code;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> context;
    std::shared_ptr<NLMError> cause;
    std::vector<std::string> stackTrace;
    
    NLMError() : code(ErrorCode::UNKNOWN_ERROR), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    NLMError(ErrorCode code, const std::string& message) 
        : code(code), message(message), line(0) {
        timestamp = std::chrono::system_clock::now();
    }
}; 

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void logError(const NLMError& error) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFilter(const std::function<bool(const LogEntry&)>& filter) = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
};

// Enhanced console logger with filtering and formatting
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void logError(const NLMError& error) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Filtering
    void setFilter(const std::function<bool(const LogEntry&)>& filter);
    bool isEnabled(LogLevel level) const override;
    
    // Formatted logging
    void logDebug(const std::string& format, ...);
    void logInfo(const std::string& format, ...);
    void logWarning(const std::string& format, ...);
    void logError(const std::string& format, ...);
    void logCritical(const std::string& format, ...);
    
    // Performance logging
    void logPerformance(const std::string& operation, double durationMs,
                       const std::unordered_map<std::string, std::string>& context = {});
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMessage(const LogEntry& entry) const;
    void logFormatted(const LogEntry& entry);
}; 

// Performance tracer for measuring operation durations
class PerformanceTracer {
public:
    PerformanceTracer(const std::string& operationName, 
                     const std::unordered_map<std::string, std::string>& context = {});
    ~PerformanceTracer();
    
    void setContext(const std::string& key, const std::string& value);
    double getDurationMs() const;
    
    // Static methods for convenience
    static void startTrace(const std::string& operationName,
                         const std::unordered_map<std::string, std::string>& context = {});
    static void endTrace(const std::string& operationName, double durationMs);
    static void logPerformance(const std::string& operationName,
                              double durationMs,
                              const std::unordered_map<std::string, std::string>& context = {});
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string operationName_;
    std::chrono::steady_clock::time_point startTime_;
    std::unordered_map<std::string, std::string> context_;
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
    
    // Enhanced logging functions
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    void logError(ErrorCode code, const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "", const std::string& operationId = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Performance logging
    void logPerformance(const std::string& operation, double durationMs,
                       const std::unordered_map<std::string, std::string>& context = {});
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Performance tracer utilities
    static PerformanceTracer trace(const std::string& operationName,
                                   const std::unordered_map<std::string, std::string>& context = {});
    
    // Error reporting utilities
    static void reportError(ErrorCode code, const std::string& message,
                           const std::string& file = "", int line = 0,
                           const std::string& function = "",
                           const std::shared_ptr<NLMError>& cause = nullptr,
                           const std::string& operationId = "");
    
    // Context management
    void setContext(const std::string& key, const std::string& value);
    std::string getContext(const std::string& key) const;
    void clearContext();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info with context
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced error macro with context
#define NLM_REPORT_ERROR(code, message, context) nlm::Logger::reportError(code, message, __FILE__, __LINE__, __func__, context)

// Stream-style logging helper with context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const std::unordered_map<std::string, std::string>& context = {});
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Context methods
    LogStream& withContext(const std::string& key, const std::string& value);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::unordered_map<std::string, std::string> context_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Error handling utilities
namespace ErrorUtils {
    std::string errorCodeToString(ErrorCode code);
    std::string errorCodeToString(const NLMError& error);
    std::string formatError(const NLMError& error, bool includeStackTrace = false);
    bool isRecoverable(ErrorCode code);
    std::string getErrorSuggestion(ErrorCode code);
}

} // namespace nlm
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <fstream>

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
    std::unordered_map<std::string, std::string> metadata;
};

// Logging statistics
struct LogStatistics {
    size_t debugCount;
    size_t infoCount;
    size_t warningCount;
    size_t errorCount;
    size_t criticalCount;
    double processingTimeMs;
    
    LogStatistics() : debugCount(0), infoCount(0), warningCount(0), 
                      errorCount(0), criticalCount(0), processingTimeMs(0.0) {}
};

// Error context for better debugging
struct ErrorContext {
    std::string errorCode;
    std::string component;
    std::string operation;
    std::string recoveryAction;
    bool isRecoverable;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext() : isRecoverable(false) {}
    ErrorContext(const std::string& code, const std::string& comp, const std::string& op)
        : errorCode(code), component(comp), operation(op), isRecoverable(false) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Advanced logging methods
    virtual void logWithContext(const LogEntry& entry, const ErrorContext& context = ErrorContext()) = 0;
    virtual void logErrorWithContext(const std::string& message, const ErrorContext& context) = 0;
    virtual void setLogCallback(std::function<void(const LogEntry&)> callback) = 0;
};

// Console logger implementation with enhanced features
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
    
    // Advanced features
    void setLogFormat(const std::string& format);
    void setTimestampFormat(const std::string& format);
    void setLogLevelColors(bool enable);
    void setIncludeMetadata(bool include);
    void setIncludeSourceLocation(bool include);
    
    // Statistics and monitoring
    LogStatistics getStatistics() const;
    void resetStatistics();
    
    // Error handling
    void logErrorWithRecovery(const std::string& message, const ErrorContext& context);
    bool hasRecentErrors() const;
    double getLastErrorTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    std::string formatMessage(const LogEntry& entry) const;
};

// Enhanced main Logger class
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
    
    // Global logging functions with error context
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const ErrorContext& context = ErrorContext());
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const ErrorContext& context = ErrorContext());
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const ErrorContext& context = ErrorContext());
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const ErrorContext& context = ErrorContext());
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const ErrorContext& context = ErrorContext());
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const ErrorContext& context = ErrorContext());
    
    // Enhanced error methods
    void logErrorWithRecovery(const std::string& message, const ErrorContext& context, const std::string& file = "", int line = 0, const std::string& function = "");
    void logWarningWithContext(const std::string& message, const ErrorContext& context, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Enhanced logging functions
    static void logWithContext(LogLevel level, const std::string& message, const ErrorContext& context);
    static void logErrorWithRecovery(const std::string& message, const ErrorContext& context);
    static void setLogCallback(std::function<void(const LogEntry&)> callback);
    
    // Statistics and monitoring
    static LogStatistics getGlobalStatistics();
    static void resetGlobalStatistics();
    static std::string getLogSummary();
    
    // Error recovery and analysis
    static bool hasRecentErrors(double timeWindowSeconds = 60.0);
    static std::vector<LogEntry> getRecentErrors(size_t limit = 10);
    static std::string analyzeErrorPattern(const std::vector<LogEntry>& entries);
    
    // Performance monitoring
    static void startPerformanceTimer(const std::string& operation);
    static void stopPerformanceTimer(const std::string& operation);
    static void logPerformance(const std::string& operation, double durationMs);
    
    // User-friendly methods
    static void logUserAction(const std::string& action, const std::string& details = "");
    static void logSystemStatus(const std::string& status, LogLevel level = LogLevel::Info);
    static void logProgress(const std::string& progress, const std::string& total = "");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced macros with error context
#define NLM_LOG_ERROR_WITH_RECOVERY(msg, errorCode, component, operation) \
    do { \
        nlm::ErrorContext ctx(errorCode, component, operation); \
        nlm::Logger::logErrorWithRecovery(msg, ctx, __FILE__, __LINE__, __func__); \
    } while(0)

#define NLM_LOG_WARNING_WITH_CONTEXT(msg, errorCode, component, operation) \
    do { \
        nlm::ErrorContext ctx(errorCode, component, operation); \
        nlm::Logger::getGlobal().logWarningWithContext(msg, ctx, __FILE__, __LINE__, __func__); \
    } while(0)

#define NLM_LOG_PERFORMANCE(operation, durationMs) nlm::Logger::logPerformance(operation, durationMs)

#define NLM_LOG_USER_ACTION(action, details) nlm::Logger::logUserAction(action, details)

#define NLM_LOG_PROGRESS(progress, total) nlm::Logger::logProgress(progress, total)

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
    
    // Enhanced support for error context
    LogStream& operator<<(const ErrorContext& context);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Error recovery macros
#define NLM_TRY OPERATION MACRO
#define NLM_CATCH(errorCode, component, operation) \
    do { \
        nlm::ErrorContext ctx(errorCode, component, operation); \
        // Error recovery logic would go here \
    } while(0)

} // namespace nlm

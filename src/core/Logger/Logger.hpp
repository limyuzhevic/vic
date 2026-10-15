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

// Enhanced log severity levels with error handling support
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Trace = -1  // For detailed debugging
};

// Error context for better debugging
struct ErrorContext {
    std::string errorCode;
    std::string module;
    std::string operation;
    std::unordered_map<std::string, std::string> metadata;
    
    ErrorContext() = default;
    ErrorContext(const std::string& code, const std::string& mod, const std::string& op)
        : errorCode(code), module(mod), operation(op) {}
    
    ErrorContext& addMeta(const std::string& key, const std::string& value) {
        metadata[key] = value;
        return *this;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "[Error: " << errorCode << "] " << module << "." << operation;
        if (!metadata.empty()) {
            oss << " | Metadata:";
            for (const auto& [key, value] : metadata) {
                oss << " " << key << "=" << value;
            }
        }
        return oss.str();
    }
};

// Enhanced Log entry structure with error context
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::optional<ErrorContext> errorContext;  // For error tracking
    bool hasErrorContext() const { return errorContext.has_value(); }
    
    LogEntry() : level(LogLevel::Info), line(0) {}
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& f = "", int ln = 0, const std::string& func = "")
        : level(lvl), message(msg), file(f), line(ln), function(func), 
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(level) << "] ";
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        oss << " " << message;
        if (!file.empty()) {
            oss << " | " << file;
            if (line > 0) oss << ":" << line;
        }
        if (!function.empty()) {
            oss << " | " << function;
        }
        if (errorContext.has_value()) {
            oss << " | " << errorContext->toString();
        }
        return oss.str();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Enhanced logging with error context
    virtual void logError(const std::string& errorCode, const std::string& message,
                         const std::string& module = "", const std::string& operation = "") {
        ErrorContext context(errorCode, module, operation);
        LogEntry entry(LogLevel::Error, message);
        entry.errorContext = context;
        log(entry);
    }
    
    // Performance monitoring support
    virtual void logPerformance(const std::string& operation, double duration, int64_t dataPoints = 1) {
        std::ostringstream oss;
        oss << "Performance: " << operation << " took " << duration << "ms for " << dataPoints << " data points";
        LogEntry entry(LogLevel::Info, oss.str());
        entry.errorContext = ErrorContext("PERF", "System", operation)
            .addMeta("duration_ms", std::to_string(duration))
            .addMeta("data_points", std::to_string(dataPoints));
        log(entry);
    }
};

// Enhanced Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enhanced features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // File logging with rotation
    void setOutputFile(const std::string& filepath, size_t maxSizeMB = 10, int maxFiles = 5);
    void closeOutputFile();
    
    // Error handling
    void logError(const std::string& errorCode, const std::string& message,
                 const std::string& module = "", const std::string& operation = "",
                 const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance logging
    void logPerformance(const std::string& operation, double duration, int64_t dataPoints = 1);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void writeToFile(const std::string& message);
    bool shouldRotate() const;
    void rotateFile();
};

// Main enhanced Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink with name for management
    void addLogger(std::shared_ptr<ILogger> logger, const std::string& name = "");
    
    // Remove specific logger by name
    bool removeLogger(const std::string& name);
    
    // Get logger by name
    std::shared_ptr<ILogger> getLogger(const std::string& name) const;
    
    // List all loggers
    std::vector<std::string> getLoggerNames() const;
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Set specific level for a logger
    void setLevel(const std::string& name, LogLevel level);
    
    // Global logging functions with enhanced parameters
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "", const ErrorContext& context = ErrorContext());
    
    // Enhanced error logging
    void logError(const std::string& errorCode, const std::string& message,
                  const std::string& module = "", const std::string& operation = "",
                  const std::string& file = "", int line = 0,
                  const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Performance logging
    void logPerformance(const std::string& operation, double duration, int64_t dataPoints = 1,
                       const std::string& file = "", int line = 0);
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled for a specific logger
    bool isEnabled(LogLevel level, const std::string& name = "global") const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Log error with context
    static void logSystemError(const std::string& errorCode, const std::string& message,
                               const std::string& module = "", const std::string& operation = "",
                               const std::string& file = "", int line = 0);
    
    // Log performance metrics
    static void logPerformanceMetric(const std::string& operation, double duration,
                                     int64_t dataPoints = 1, const std::string& file = "", int line = 0);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced convenience macros for error handling
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced error logging macro
#define NLM_LOG_ERROR_WITH_CONTEXT(errorCode, msg, module, operation) \
    nlm::Logger::getGlobal().logError(errorCode, msg, module, operation, __FILE__, __LINE__)

// Enhanced performance logging macro
#define NLM_LOG_PERFORMANCE(operation, duration, dataPoints) \
    nlm::Logger::getGlobal().logPerformance(operation, duration, dataPoints, __FILE__, __LINE__)

// Stream-style logging helper with error context
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
              const ErrorContext& context = ErrorContext());
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Helper for error context
    LogStream& operator<<(const ErrorContext& context) {
        context_ = context;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    ErrorContext context_;
};

// Enhanced stream logging macros
#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)
#define NLM_LOG_STREAM_WITH_CONTEXT(level, context) nlm::LogStream(level, __FILE__, __LINE__, __func__, context)

} // namespace nlm

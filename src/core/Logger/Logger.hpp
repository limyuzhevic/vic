#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>

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

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Enhanced logger interface
class IEnhancedLogger : public ILogger {
public:
    virtual ~IEnhancedLogger() = default;
    
    // Structured logging with context
    virtual void logStructured(const std::string& level, const std::string& message,
                              const std::vector<std::string>& context = {}) = 0;
    
    // Performance monitoring
    virtual void startOperation(const std::string& name) = 0;
    virtual void endOperation(const std::string& name) = 0;
    virtual void logMetric(const std::string& name, double value,
                         const std::string& unit = "ms") = 0;
    
    // Error tracking
    virtual void trackError(const std::string& errorCode, const std::string& message,
                          const std::string& context = "") = 0;
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Enhanced console logger
class EnhancedConsoleLogger : public IEnhancedLogger {
public:
    EnhancedConsoleLogger();
    explicit EnhancedConsoleLogger(LogLevel level);
    ~EnhancedConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // IEnhancedLogger overrides
    void logStructured(const std::string& level, const std::string& message,
                      const std::vector<std::string>& context) override;
    void startOperation(const std::string& name) override;
    void endOperation(const std::string& name) override;
    void logMetric(const std::string& name, double value,
                  const std::string& unit) override;
    void trackError(const std::string& errorCode, const std::string& message,
                   const std::string& context) override;
    
    // Enhanced features
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void setMaxFileSize(size_t maxSizeBytes);
    void setMaxFiles(int maxFiles);
    void rotateLogFile();
    void closeOutputFile();
    
    // Correlation ID management
    uint64_t generateCorrelationId();
    void setCorrelationId(uint64_t id);
    uint64_t getCurrentCorrelationId() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
    uint64_t getCurrentThreadId() const;
};

// Performance metrics collector
class PerformanceMetrics {
public:
    PerformanceMetrics();
    ~PerformanceMetrics();
    
    void recordMetric(const std::string& name, double value,
                     const std::string& unit = "ms", 
                     const std::string& category = "general");
    
    void recordMemoryUsage(size_t memoryBytes);
    void recordCpuUsage(double cpuPercent);
    
    std::vector<std::pair<std::string, double>> getMetrics() const;
    void clearMetrics();
    
    static PerformanceMetrics& getGlobalMetrics();
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced logger with performance monitoring
class EnhancedLogger : public Logger {
public:
    EnhancedLogger();
    ~EnhancedLogger() override;
    
    // Performance monitoring
    void startOperation(const std::string& name);
    void endOperation(const std::string& name);
    void logPerformance(const std::string& name, double duration,
                       const std::string& unit = "ms");
    
    // Structured logging
    void logStructured(LogLevel level, const std::string& message,
                      const std::vector<std::string>& context = {});
    
    // Error tracking
    void trackError(const std::string& errorCode, const std::string& message,
                   const std::string& context = "");
    
    // Performance metrics access
    static PerformanceMetrics& getMetrics();
};

// Error handling framework
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

enum class ErrorCategory {
    None = 0,
    Configuration = 1,
    Validation = 2,
    Memory = 3,
    Plasticity = 4,
    Development = 5,
    Neuromodulation = 6,
    Perception = 7,
    Action = 8,
    Environment = 9,
    Experiment = 10,
    Performance = 11,
    System = 12
};

class NLMError {
public:
    NLMError() = default;
    NLMError(ErrorCategory category, ErrorSeverity severity,
            const std::string& message, const std::string& file = "",
            int line = 0, const std::string& function = "",
            const std::string& errorCode = "", uint64_t correlationId = 0);
    
    virtual ~NLMError() = default;
    
    virtual void log() const;
    virtual bool isRecoverable() const;
    virtual std::string getDescription() const;
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getMessage() const { return message_; }
    const std::string& getErrorCode() const { return errorCode_; }
    uint64_t getCorrelationId() const { return correlationId_; }
    
protected:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    std::string errorCode_;
    uint64_t correlationId_;
    std::chrono::system_clock::time_point timestamp_;
};

// Specific error types
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message, 
                      const std::string& file = "", int line = 0,
                      const std::string& function = "",
                      const std::string& errorCode = "",
                      uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   const std::string& file = "", int line = 0,
                   const std::string& function = "",
                   const std::string& errorCode = "",
                   uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "",
               const std::string& errorCode = "",
               uint64_t correlationId = 0);
    
    void log() const override;
    bool isRecoverable() const override;
    std::string getDescription() const override;
};

// Error handling utilities
class ErrorHandler {
public:
    static void logError(const std::shared_ptr<NLMError> error);
    static void logError(ErrorCategory category, ErrorSeverity severity,
                        const std::string& message,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "",
                        const std::string& errorCode = "");
    
    static bool isRecoverable(const std::shared_ptr<NLMError> error);
    static std::string getFormattedError(const std::shared_ptr<NLMError> error);
    
    static void setErrorReportingCallback(std::function<void(const std::shared_ptr<NLMError>)> callback);
    
    static uint64_t generateCorrelationId();
    static void setGlobalCorrelationId(uint64_t id);
    static uint64_t getGlobalCorrelationId();
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Enhanced error macros
#define NLM_THROW_CONFIGURATION_ERROR(msg, code) \
    nlm::ErrorHandler::logError(nlm::ErrorCategory::Configuration, nlm::ErrorSeverity::Error, msg, __FILE__, __LINE__, __func__, code)

#define NLM_THROW_VALIDATION_ERROR(msg, code) \
    nlm::ErrorHandler::logError(nlm::ErrorCategory::Validation, nlm::ErrorSeverity::Warning, msg, __FILE__, __LINE__, __func__, code)

#define NLM_THROW_MEMORY_ERROR(msg, code) \
    nlm::ErrorHandler::logError(nlm::ErrorCategory::Memory, nlm::ErrorSeverity::Error, msg, __FILE__, __LINE__, __func__, code)

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

} // namespace nlm

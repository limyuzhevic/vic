# Enhanced error handling and logging for NLM
// Comprehensive error handling system with custom exception types and advanced logging

#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <mutex>
#include <stacktrace>
#include "Logger.hpp"

namespace nlm {
namespace error {

// Enhanced exception hierarchy
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, const std::string& source = "")
        : std::runtime_error(formatMessage(message, source)), source_(source) {}
    
    virtual ~NLMException() = default;
    
    const std::string& getSource() const { return source_; }
    const std::string& getMessage() const { return what(); }
    
    // Format exception with context
    static std::string formatMessage(const std::string& message, const std::string& source) {
        if (source.empty()) return message;
        return "[" + source + "] " + message;
    }
    
protected:
    std::string source_;
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message, const std::string& key = "")
        : NLMException("Configuration error: " + message, "Config"), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message, const std::string& key = "")
        : ConfigException("Validation failed: " + message, key) {}
};

class ConfigLoadException : public ConfigException {
public:
    ConfigLoadException(const std::string& message, const std::string& filepath = "")
        : ConfigException("Failed to load config: " + message, "File"), filepath_(filepath) {}
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Brain-related exceptions
class BrainException : public NLMException {
public:
    BrainException(const std::string& message, const std::string& operation = "")
        : NLMException("Brain error: " + message, "Brain"), operation_(operation) {}
    
    const std::string& getOperation() const { return operation_; }
    
private:
    std::string operation_;
};

class InitializationException : public BrainException {
public:
    InitializationException(const std::string& message)
        : BrainException("Initialization failed: " + message, "initialize") {}
};

class MemoryException : public BrainException {
public:
    MemoryException(const std::string& message, const std::string& memory_type = "")
        : BrainException("Memory error: " + message, "memory"), memory_type_(memory_type) {}
    
    const std::string& getMemoryType() const { return memory_type_; }
    
private:
    std::string memory_type_;
};

// Neuromodulation exceptions
class NeuromodulationException : public NLMException {
public:
    NeuromodulationException(const std::string& message, const std::string& signal_type = "")
        : NLMException("Neuromodulation error: " + message, "Neuromodulation"), signal_type_(signal_type) {}
    
    const std::string& getSignalType() const { return signal_type_; }
    
private:
    std::string signal_type_;
};

// Performance-related exceptions
class PerformanceException : public NLMException {
public:
    PerformanceException(const std::string& message, double performance_value = 0.0)
        : NLMException("Performance error: " + message, "Performance"), performance_value_(performance_value) {}
    
    double getPerformanceValue() const { return performance_value_; }
    
private:
    double performance_value_;
};

// Error context and recovery
class ErrorContext {
public:
    ErrorContext(const std::string& operation, const std::string& component)
        : operation_(operation), component_(component), timestamp_(std::chrono::system_clock::now()) {}
    
    void addParameter(const std::string& name, const std::string& value);
    void addParameter(const std::string& name, int value);
    void addParameter(const std::string& name, double value);
    void addParameter(const std::string& name, bool value);
    
    const std::string& getOperation() const { return operation_; }
    const std::string& getComponent() const { return component_; }
    const std::string& getMessage() const { return message_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    
    const std::unordered_map<std::string, std::string>& getParameters() const { return parameters_; }
    
private:
    std::string operation_;
    std::string component_;
    std::string message_;
    std::chrono::system_clock::time_point timestamp_;
    std::unordered_map<std::string, std::string> parameters_;
};

// Exception handler with recovery strategies
class ExceptionHandler {
public:
    enum class RecoveryAction {
        Retry,
        Fallback,
        Abort,
        LogOnly
    };
    
    // Register exception handler for specific exception type
    template<typename ExceptionType>
    void registerHandler(std::function<RecoveryAction(const ExceptionType&)> handler);
    
    // Handle exception with appropriate recovery action
    template<typename ExceptionType>
    RecoveryAction handle(const ExceptionType& exception);
    
    // Log exception with full context
    void logException(const std::exception& exception, const ErrorContext& context);
    
    // Get exception statistics
    struct Statistics {
        size_t total_exceptions = 0;
        size_t handled_exceptions = 0;
        std::unordered_map<std::string, size_t> exception_counts;
        double average_handling_time_ms = 0.0;
    };
    
    Statistics getStatistics() const { return stats_; }
    
private:
    std::unordered_map<std::string, std::function<RecoveryAction(const std::exception&)>> handlers_;
    Statistics stats_;
    mutable std::mutex mutex_;
};

// Enhanced logging system
namespace logging {

// Log levels with numeric values
enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Critical = 5,
    Fatal = 6
};

// Enhanced log entry with additional context
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string thread_id;
    std::unordered_map<std::string, std::string> metadata;
    std::shared_ptr<std::exception> associated_exception;
};

// Enhanced logger interface
class IEnhancedLogger {
public:
    virtual ~IEnhancedLogger() = default;
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual void setFormat(const std::string& format) = 0;
    virtual void setFilter(std::function<bool(const EnhancedLogEntry&)> filter) = 0;
    virtual void addHandler(std::function<void(const EnhancedLogEntry&)> handler) = 0;
};

// Async logger implementation
class AsyncLogger : public IEnhancedLogger {
public:
    AsyncLogger(size_t queue_size = 10000, size_t worker_threads = 4);
    ~AsyncLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setFormat(const std::string& format) override;
    void setFilter(std::function<bool(const EnhancedLogEntry&)> filter) override;
    void addHandler(std::function<void(const EnhancedLogEntry&)> handler) override;
    
    // Flush all pending logs
    void flush();
    
    // Get queue statistics
    struct QueueStats {
        size_t queue_size = 0;
        size_t pending_items = 0;
        size_t processed_items = 0;
        double average_processing_time_ms = 0.0;
    };
    
    QueueStats getQueueStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Log aggregation service
class LogAggregator {
public:
    // Add log entry to aggregation
    void addEntry(const EnhancedLogEntry& entry);
    
    // Get aggregated statistics
    struct AggregatedStats {
        size_t total_entries = 0;
        std::unordered_map<LogLevel, size_t> level_counts;
        std::unordered_map<std::string, size_t> component_counts;
        std::chrono::system_clock::time_point oldest_entry;
        std::chrono::system_clock::time_point newest_entry;
        std::vector<std::string> error_messages;
    };
    
    AggregatedStats getStats() const;
    
    // Export logs to file
    void exportToFile(const std::string& filepath, const std::string& format = "json");
    
    // Filter logs by criteria
    std::vector<EnhancedLogEntry> filter(
        std::function<bool(const EnhancedLogEntry&)> predicate) const;
    
private:
    std::vector<EnhancedLogEntry> entries_;
    mutable std::mutex mutex_;
};

// Performance monitoring integration
class PerformanceLogger {
public:
    PerformanceLogger(std::shared_ptr<IEnhancedLogger> logger);
    
    // Log performance measurement
    void logPerformance(
        const std::string& operation,
        double duration_ms,
        size_t memory_usage_bytes = 0,
        const std::string& context = ""
    );
    
    // Start performance measurement
    void startMeasurement(const std::string& operation);
    
    // Stop performance measurement
    void stopMeasurement(const std::string& operation);
    
    // Get performance statistics
    struct PerformanceStats {
        std::unordered_map<std::string, double> average_durations_ms;
        std::unordered_map<std::string, size_t> call_counts;
        std::unordered_map<std::string, size_t> memory_usage;
        double total_duration_ms = 0.0;
        size_t total_calls = 0;
    };
    
    PerformanceStats getStatistics() const;
    
private:
    std::shared_ptr<IEnhancedLogger> logger_;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> start_times_;
    mutable std::mutex mutex_;
};

} // namespace logging

// Convenience macros for enhanced error handling
#define NLM_THROW_CONFIG_EXCEPTION(msg, key) nlm::error::ConfigException(msg, key)
#define NLM_THROW_BRAIN_EXCEPTION(msg, op) nlm::error::BrainException(msg, op)
#define NLM_THROW_MEMORY_EXCEPTION(msg, type) nlm::error::MemoryException(msg, type)
#define NLM_THROW_PERFORMANCE_EXCEPTION(msg, value) nlm::error::PerformanceException(msg, value)

// Error context macros
#define NLM_ERROR_CONTEXT(operation, component) nlm::error::ErrorContext operation ## _context(operation, component)

// Stream-style error handling
class ErrorStream {
public:
    ErrorStream(const std::string& operation, const std::string& component);
    ~ErrorStream();
    
    template<typename T>
    ErrorStream& operator<<(const T& value);
    
    ErrorStream& operator<<(const std::exception& ex);
    
private:
    ErrorContext context_;
    std::ostringstream stream_;
};

// Global exception handler
extern ExceptionHandler globalExceptionHandler;

// Enhanced logging macros
#define NLM_LOG_TRACE(msg) nlm::logging::log(LogLevel::Trace, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_DEBUG(msg) nlm::logging::log(LogLevel::Debug, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::logging::log(LogLevel::Info, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::logging::log(LogLevel::Warning, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::logging::log(LogLevel::Error, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::logging::log(LogLevel::Critical, msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_FATAL(msg) nlm::logging::log(LogLevel::Fatal, msg, __FILE__, __LINE__, __func__)

// Logging with exception
#define NLM_LOG_ERROR_WITH_EXCEPTION(msg, ex) nlm::logging::logWithException(LogLevel::Error, msg, ex, __FILE__, __LINE__, __func__)

// Performance logging macros
#define NLM_PERF_MEASURE(operation) nlm::logging::PerfScope operation ## _scope(operation)

} // namespace nlm

// Template implementations
namespace nlm {
namespace error {

// ExceptionHandler implementation
namespace {

// Global exception handler instance
ExceptionHandler globalExceptionHandler;

// Helper function for exception type erasure
template<typename ExceptionType>
RecoveryAction handleExceptionType(const ExceptionType& exception, 
                                  const std::unordered_map<std::string, std::function<RecoveryAction(const std::exception&)>>& handlers) {
    std::string typeName = typeid(ExceptionType).name();
    auto it = handlers.find(typeName);
    if (it != handlers.end()) {
        return it->second(exception);
    }
    return RecoveryAction::Abort;
}

} // namespace anonymous

// ErrorStream implementation
ErrorStream::ErrorStream(const std::string& operation, const std::string& component)
    : context_(operation, component) {}

ErrorStream::~ErrorStream() {
    if (!stream_.str().empty()) {
        context_.setMessage(stream_.str());
        globalExceptionHandler.logException(context_, context_);
    }
}

template<typename T>
ErrorStream& ErrorStream::operator<<(const T& value) {
    stream_ << value;
    return *this;
}

ErrorStream& ErrorStream::operator<<(const std::exception& ex) {
    stream_ << " [Exception: " << ex.what() << "]";
    return *this;
}

} // namespace error

// Logging convenience functions
namespace logging {

inline void log(LogLevel level, const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "") {
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    // Get the enhanced logger (simplified for this example)
    static AsyncLogger enhancedLogger;
    enhancedLogger.log(entry);
}

inline void logWithException(LogLevel level, const std::string& message,
                             const std::exception& exception,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "") {
    EnhancedLogEntry entry;
    entry.level = level;
    entry.message = message + " (Exception: " + std::string(exception.what()) + ")";
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.thread_id = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    entry.associated_exception = std::make_shared<std::exception>(exception);
    
    // Get the enhanced logger
    static AsyncLogger enhancedLogger;
    enhancedLogger.log(entry);
}

} // namespace logging

} // namespace nlm
#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <functional>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Error categories
enum class ErrorCategory {
    None = 0,
    Config = 1,
    Brain = 2,
    Memory = 3,
    Neuromodulation = 4,
    Prediction = 5,
    Development = 6,
    System = 7,
    Network = 8,
    IO = 9,
    Validation = 10,
    Recovery = 11
};

// Specific error codes for different error scenarios
enum class ErrorCode {
    // Generic errors
    ERROR_NONE = 0,
    ERROR_UNKNOWN = 1,
    ERROR_OUT_OF_MEMORY = 2,
    ERROR_INVALID_ARGUMENT = 3,
    ERROR_NULL_POINTER = 4,
    ERROR_TIMEOUT = 5,
    ERROR_NOT_INITIALIZED = 6,
    
    // Configuration errors
    ERROR_CONFIG_INVALID = 100,
    ERROR_CONFIG_FILE_NOT_FOUND = 101,
    ERROR_CONFIG_SYNTAX_ERROR = 102,
    ERROR_CONFIG_VALIDATION_FAILED = 103,
    ERROR_CONFIG_KEY_NOT_FOUND = 104,
    ERROR_CONFIG_TYPE_MISMATCH = 105,
    ERROR_CONFIG_VALUE_OUT_OF_RANGE = 106,
    
    // Brain initialization errors
    ERROR_BRAIN_INIT_FAILED = 200,
    ERROR_BRAIN_ALREADY_INITIALIZED = 201,
    ERROR_BRAIN_NOT_INITIALIZED = 202,
    ERROR_BRAIN_INITIALIZATION_TIMEOUT = 203,
    ERROR_BRAIN_CORRUPTED_STATE = 204,
    
    // Memory errors
    ERROR_MEMORY_ALLOCATION_FAILED = 300,
    ERROR_MEMORY_INVALID_ACCESS = 301,
    ERROR_MEMORY_CORRUPTION = 302,
    ERROR_MEMORY_FULL = 303,
    ERROR_MEMORY_NOT_AVAILABLE = 304,
    
    // Neuromodulation errors
    ERROR_NEUROMODULATION_INIT_FAILED = 400,
    ERROR_NEUROMODULATION_INVALID_SIGNAL = 401,
    ERROR_NEUROMODULATION_OVERFLOW = 402,
    
    // Prediction errors
    ERROR_PREDICTION_INIT_FAILED = 500,
    ERROR_PREDICTION_INVALID_INPUT = 501,
    ERROR_PREDICTION_OVERFLOW = 502,
    
    // Development errors
    ERROR_DEVELOPMENT_INIT_FAILED = 600,
    ERROR_DEVELOPMENT_INVALID_STAGE = 601,
    ERROR_DEVELOPMENT_CONVERGENCE_FAILED = 602,
    
    // System errors
    ERROR_SYSTEM_RESOURCE_UNAVAILABLE = 700,
    ERROR_SYSTEM_PERMISSION_DENIED = 701,
    ERROR_SYSTEM_DRIVER_NOT_FOUND = 702,
    ERROR_SYSTEM_HW_FAILURE = 703,
    
    // Network errors
    ERROR_NETWORK_CONNECTION_FAILED = 800,
    ERROR_NETWORK_TIMEOUT = 801,
    ERROR_NETWORK_PROTOCOL_ERROR = 802,
    
    // IO errors
    ERROR_IO_FILE_NOT_FOUND = 900,
    ERROR_IO_PERMISSION_DENIED = 901,
    ERROR_IO_DISK_FULL = 902,
    ERROR_IO_CORRUPTION = 903,
    
    // Validation errors
    ERROR_VALIDATION_FAILED = 1000,
    ERROR_VALIDATION_TYPE_MISMATCH = 1001,
    ERROR_VALIDATION_VALUE_OUT_OF_RANGE = 1002,
    ERROR_VALIDATION_SCHEMA_INVALID = 1003,
    
    // Recovery errors
    ERROR_RECOVERY_FAILED = 1100,
    ERROR_RECOVERY_NO_STRATEGY = 1101,
    ERROR_RECOVERY_EXHAUSTED = 1102,
    ERROR_RECOVERY_INVALID_STATE = 1103
};

// Error context structure for detailed error information
struct ErrorContext {
    ErrorContext() : 
        line(0),
        timestamp(std::chrono::system_clock::now()),
        severity(ErrorSeverity::Error),
        category(ErrorCategory::None),
        code(ErrorCode::ERROR_NONE),
        correlationId("") {
    }
    
    ErrorContext(const std::string& msg, const std::string& file, int line, 
                  const std::string& func, ErrorSeverity sev = ErrorSeverity::Error,
                  ErrorCategory cat = ErrorCategory::None, ErrorCode errCode = ErrorCode::ERROR_UNKNOWN,
                  const std::string& corrId = "")
        : message(msg),
          file(file),
          line(line),
          function(func),
          timestamp(std::chrono::system_clock::now()),
          severity(sev),
          category(cat),
          code(errCode),
          correlationId(corrId) {
    }
    
    std::string message;                        // Error message
    std::string file;                           // File where error occurred
    int line;                                   // Line number where error occurred
    std::string function;                       // Function where error occurred
    std::chrono::system_clock::time_point timestamp;  // Error timestamp
    ErrorSeverity severity;                     // Error severity level
    ErrorCategory category;                     // Error category
    ErrorCode code;                             // Specific error code
    std::string correlationId;                  // Correlation ID for tracking related errors
    std::unordered_map<std::string, std::string> context;  // Additional context data
    std::vector<ErrorCode> relatedCodes;        // Related error codes for correlation
};

// Exception class for structured error information
class ExperimentError : public std::exception {
private:
    ErrorContext context_;
    mutable std::string what_buf_;
    
public:
    explicit ExperimentError(const ErrorContext& ctx) : context_(ctx) {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(context_.code) << "] " << context_.message;
        if (!context_.file.empty()) {
            oss << " (" << context_.file << ":" << context_.line << ")";
        }
        what_buf_ = oss.str();
    }
    
    explicit ExperimentError(const std::string& msg, ErrorCode code = ErrorCode::ERROR_UNKNOWN,
                             ErrorSeverity severity = ErrorSeverity::Error,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "")
        : ExperimentError(createContext(msg, code, severity, file, line, function)) {
    }
    
    explicit ExperimentError(ErrorCode code, const std::string& msg,
                             ErrorSeverity severity = ErrorSeverity::Error,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "")
        : ExperimentError(createContext(msg, code, severity, file, line, function)) {
    }
    
    // Get the error context
    const ErrorContext& getContext() const { return context_; }
    
    // Get the error code
    ErrorCode getCode() const { return context_.code; }
    
    // Get the error severity
    ErrorSeverity getSeverity() const { return context_.severity; }
    
    // Get the error category
    ErrorCategory getCategory() const { return context_.category; }
    
    // Get the correlation ID
    const std::string& getCorrelationId() const { return context_.correlationId; }
    
    // Get the message
    const char* what() const noexcept override {
        return what_buf_.c_str();
    }
    
    // Add additional context information
    void addContext(const std::string& key, const std::string& value) {
        context_.context[key] = value;
    }
    
    // Add a related error code
    void addRelatedCode(ErrorCode code) {
        context_.relatedCodes.push_back(code);
    }
    
    // Generate a unique correlation ID
    static std::string generateCorrelationId() {
        std::stringstream ss;
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        ss << "ERR_" << timestamp << "_" 
           << std::this_thread::get_id();
        return ss.str();
    }
    
private:
    static ErrorContext createContext(const std::string& msg, ErrorCode code, 
                                      ErrorSeverity severity, const std::string& file, 
                                      int line, const std::string& function) {
        ErrorContext ctx;
        ctx.message = msg;
        ctx.code = code;
        ctx.severity = severity;
        ctx.category = static_cast<ErrorCategory>(static_cast<int>(code) / 100);
        ctx.file = file;
        ctx.line = line;
        ctx.function = function;
        ctx.correlationId = generateCorrelationId();
        return ctx;
    }
};

// Error recovery strategy interface
class IErrorRecoveryStrategy {
public:
    virtual ~IErrorRecoveryStrategy() = default;
    
    // Check if this strategy can handle the given error
    virtual bool canHandle(const ErrorContext& error) const = 0;
    
    // Execute the recovery strategy
    virtual bool execute(const ErrorContext& error) = 0;
    
    // Get strategy name
    virtual std::string getName() const = 0;
    
    // Get strategy priority
    virtual int getPriority() const = 0;
};

// Retry strategy with exponential backoff
class RetryStrategy : public IErrorRecoveryStrategy {
private:
    ErrorCode errorCode_;
    int maxRetries_;
    std::chrono::milliseconds baseDelay_;
    double backoffMultiplier_;
    size_t retryCount_;
    
public:
    RetryStrategy(ErrorCode errorCode, int maxRetries = 3,
                  std::chrono::milliseconds baseDelay = std::chrono::milliseconds(100),
                  double backoffMultiplier = 2.0)
        : errorCode_(errorCode), maxRetries_(maxRetries), baseDelay_(baseDelay),
          backoffMultiplier_(backoffMultiplier), retryCount_(0) {
    }
    
    bool canHandle(const ErrorContext& error) const override {
        return error.code == errorCode_;
    }
    
    bool execute(const ErrorContext& error) override {
        if (retryCount_ < maxRetries_) {
            retryCount_++;
            std::chrono::milliseconds delay(
                static_cast<long>(baseDelay_.count() * std::pow(backoffMultiplier_, retryCount_ - 1))
            );
            // In a real implementation, you would wait here
            return true;
        }
        return false;
    }
    
    std::string getName() const override {
        return "RetryStrategy";
    }
    
    int getPriority() const override {
        return 100;
    }
    
    void reset() {
        retryCount_ = 0;
    }
};

// Fallback strategy for graceful degradation
class FallbackStrategy : public IErrorRecoveryStrategy {
private:
    ErrorCode errorCode_;
    std::function<bool(const ErrorContext&)> fallbackAction_;
    
public:
    FallbackStrategy(ErrorCode errorCode, const std::function<bool(const ErrorContext&)>& action)
        : errorCode_(errorCode), fallbackAction_(action) {
    }
    
    bool canHandle(const ErrorContext& error) const override {
        return error.code == errorCode_;
    }
    
    bool execute(const ErrorContext& error) override {
        return fallbackAction_(error);
    }
    
    std::string getName() const override {
        return "FallbackStrategy";
    }
    
    int getPriority() const override {
        return 50;
    }
};

// Error recovery manager
class ErrorRecoveryManager {
private:
    std::vector<std::unique_ptr<IErrorRecoveryStrategy>> strategies_;
    std::mutex mutex_;
    
public:
    ErrorRecoveryManager() = default;
    
    // Add a recovery strategy
    void addStrategy(std::unique_ptr<IErrorRecoveryStrategy> strategy) {
        std::lock_guard<std::mutex> lock(mutex_);
        strategies_.push_back(std::move(strategy));
    }
    
    // Attempt recovery for the given error
    bool attemptRecovery(const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Sort strategies by priority (higher priority first)
        std::sort(strategies_.begin(), strategies_.end(),
                  [](const std::unique_ptr<IErrorRecoveryStrategy>& a,
                     const std::unique_ptr<IErrorRecoveryStrategy>& b) {
                      return a->getPriority() > b->getPriority();
                  });
        
        for (const auto& strategy : strategies_) {
            if (strategy->canHandle(error)) {
                return strategy->execute(error);
            }
        }
        return false;
    }
    
    // Clear all strategies
    void clearStrategies() {
        std::lock_guard<std::mutex> lock(mutex_);
        strategies_.clear();
    }
    
    // Get all strategy names
    std::vector<std::string> getStrategyNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        for (const auto& strategy : strategies_) {
            names.push_back(strategy->getName());
        }
        return names;
    }
};

// Error statistics and correlation
struct ErrorStats {
    ErrorStats() : 
        totalErrors(0),
        errorsByCategory(),
        errorsByCode(),
        lastErrorTime(std::chrono::system_clock::now()),
        errorRate(0.0f),
        recoveryAttempts(0),
        recoverySuccesses(0) {
    }
    
    size_t totalErrors;                                      // Total number of errors
    std::unordered_map<ErrorCategory, size_t> errorsByCategory;  // Errors by category
    std::unordered_map<ErrorCode, size_t> errorsByCode;          // Errors by code
    std::chrono::system_clock::time_point lastErrorTime;        // Time of last error
    float errorRate;                                          // Errors per second
    size_t recoveryAttempts;                                  // Number of recovery attempts
    size_t recoverySuccesses;                                 // Number of successful recoveries
    std::unordered_map<std::string, size_t> correlations;      // Error correlation counts
    std::unordered_map<std::string, std::vector<ErrorContext>> relatedErrors;  // Related errors by correlation ID
};

// Error reporter for statistics and diagnostics
class ErrorReporter {
private:
    ErrorStats stats_;
    std::mutex mutex_;
    std::function<void(const ErrorContext&)> customReporter_;
    
public:
    ErrorReporter() = default;
    
    // Report an error
    void report(const ErrorContext& error) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        stats_.totalErrors++;
        stats_.errorsByCategory[error.category]++;
        stats_.errorsByCode[error.code]++;
        stats_.lastErrorTime = error.timestamp;
        
        // Update error rate (simplified - in real implementation you'd track this over time)
        static auto lastReportTime = std::chrono::system_clock::now();
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastReportTime);
        if (duration.count() > 0) {
            stats_.errorRate = static_cast<float>(stats_.totalErrors) / duration.count();
            lastReportTime = now;
        }
        
        // Track correlation
        if (!error.correlationId.empty()) {
            stats_.correlations[error.correlationId]++;
            if (stats_.relatedErrors.find(error.correlationId) == stats_.relatedErrors.end()) {
                stats_.relatedErrors[error.correlationId] = std::vector<ErrorContext>();
            }
            stats_.relatedErrors[error.correlationId].push_back(error);
        }
        
        // Custom reporting
        if (customReporter_) {
            customReporter_(error);
        }
    }
    
    // Set custom error reporter
    void setCustomReporter(const std::function<void(const ErrorContext&)>& reporter) {
        std::lock_guard<std::mutex> lock(mutex_);
        customReporter_ = reporter;
    }
    
    // Get error statistics
    ErrorStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    // Generate error report
    std::string generateReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        ss << "=== Error Report ===\n";
        ss << "Total Errors: " << stats_.totalErrors << "\n";
        ss << "Error Rate: " << stats_.errorRate << " errors/sec\n";
        ss << "Recovery Attempts: " << stats_.recoveryAttempts << "\n";
        ss << "Recovery Successes: " << stats_.recoverySuccesses << "\n";
        ss << "\n--- Errors by Category ---\n";
        for (const auto& pair : stats_.errorsByCategory) {
            ss << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
        ss << "\n--- Errors by Code ---\n";
        for (const auto& pair : stats_.errorsByCode) {
            ss << static_cast<int>(pair.first) << ": " << pair.second << "\n";
        }
        ss << "\n--- Correlations ---\n";
        for (const auto& pair : stats_.correlations) {
            ss << pair.first << ": " << pair.second << " related errors\n";
        }
        
        return ss.str();
    }
    
    // Record a recovery attempt
    void recordRecoveryAttempt(bool success) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.recoveryAttempts++;
        if (success) {
            stats_.recoverySuccesses++;
        }
    }
};

// Global error recovery and reporting instances
inline ErrorRecoveryManager& getErrorRecoveryManager() {
    static ErrorRecoveryManager manager;
    return manager;
}

inline ErrorReporter& getErrorReporter() {
    static ErrorReporter reporter;
    return reporter;
}

// Macro for error checking and reporting
#define CHECK_ERROR(condition, code, message) \
    do { \
        if (!(condition)) { \
            nlm::ErrorContext ctx(message, __FILE__, __LINE__, __func__, \
                                nlm::ErrorSeverity::Error, nlm::ErrorCategory::System, code); \
            nlm::getErrorReporter().report(ctx); \
            nlm::getErrorRecoveryManager().attemptRecovery(ctx); \
            throw nlm::ExperimentError(ctx); \
        } \
    } while(0)

// Helper function to log errors to Logger
inline void logErrorToLogger(const ErrorContext& error) {
    std::stringstream ss;
    ss << "[" << static_cast<int>(error.code) << "] " << error.message;
    if (!error.file.empty()) {
        ss << " (" << error.file << ":" << error.line << ")";
    }
    if (!error.function.empty()) {
        ss << " in " << error.function;
    }
    
    // Convert severity to Logger level
    nlm::LogLevel logLevel;
    switch (error.severity) {
        case nlm::ErrorSeverity::Debug:   logLevel = nlm::LogLevel::Debug; break;
        case nlm::ErrorSeverity::Info:    logLevel = nlm::LogLevel::Info; break;
        case nlm::ErrorSeverity::Warning: logLevel = nlm::LogLevel::Warning; break;
        case nlm::ErrorSeverity::Error:   logLevel = nlm::LogLevel::Error; break;
        case nlm::ErrorSeverity::Critical: logLevel = nlm::LogLevel::Critical; break;
        case nlm::ErrorSeverity::Fatal:   logLevel = nlm::LogLevel::Critical; break;
        default: logLevel = nlm::LogLevel::Error; break;
    }
    
    // Use Logger's log method
    nlm::Logger::getGlobal().log(logLevel, ss.str(), error.file, error.line, error.function);
}

} // namespace nlm
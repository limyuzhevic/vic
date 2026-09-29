#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>
#include "Exceptions/NLMBaseException.hpp"
#include "Logger.hpp"

using json = nlohmann::json;

namespace nlm {

// Error codes for NLM system
enum class ErrorCode {
    // Success
    Success = 0,
    
    // Validation errors
    ValidationError = 1000,
    InvalidParameter = 1001,
    MissingConfiguration = 1002,
    FileNotFound = 1003,
    PermissionDenied = 1004,
    
    // Memory errors
    MemoryAllocation = 2000,
    MemoryCorruption = 2001,
    OutOfMemory = 2002,
    
    // File system errors
    FileReadError = 3000,
    FileWriteError = 3001,
    DirectoryNotFound = 3002,
    
    // Network errors
    ConnectionError = 4000,
    Timeout = 4001,
    ProtocolError = 4002,
    
    // Neural network errors
    NeuralNetworkError = 5000,
    InvalidArchitecture = 5001,
    TrainingError = 5002,
    InferenceError = 5003,
    
    // Simulation errors
    SimulationError = 6000,
    IntegrationError = 6001,
    TimeStepError = 6002,
    
    // System errors
    SystemError = 7000,
    NotImplemented = 7001,
    UnsupportedFeature = 7002,
    
    // Generic errors
    UnknownError = 9999,
    FatalError = 10000
};

// Error categories for better classification
enum class ErrorCategory {
    Validation,
    Resource,
    Network,
    NeuralNetwork,
    Simulation,
    System,
    User,
    External
};

// Error severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Fatal
};

// Error handling strategies
enum class RecoveryStrategy {
    None,
    Retry,
    Fallback,
    GracefulDegradation,
    CheckpointRestore,
    AutoRecovery
};

// Recovery result
enum class RecoveryResult {
    Success,
    Failed,
    Skipped,
    NeedsManualIntervention
};

// Retry configuration
struct RetryConfig {
    int maxRetries = 3;
    std::chrono::milliseconds initialDelay = std::chrono::milliseconds(100);
    double backoffMultiplier = 2.0;
    std::vector<ErrorCode> retryableErrors = {
        ErrorCode::ConnectionError,
        ErrorCode::Timeout,
        ErrorCode::FileReadError,
        ErrorCode::FileWriteError
    };
    std::chrono::milliseconds maxDelay = std::chrono::milliseconds(10000);
};

// Error statistics for monitoring
struct ErrorStats {
    ErrorCode code;
    std::string message;
    ErrorCategory category;
    ErrorSeverity severity;
    int count;
    std::chrono::system_clock::time_point firstOccurrence;
    std::chrono::system_clock::time_point lastOccurrence;
    double rate;
    std::map<std::string, std::string> context;
};

// Error entry for structured logging
struct ErrorEntry {
    ErrorCode code;
    std::string message;
    ErrorCategory category;
    ErrorSeverity severity;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    std::string stackTrace;
    std::string correlationId;
    
    // JSON serialization
    json toJson() const {
        json j;
        j["code"] = static_cast<int>(code);
        j["message"] = message;
        j["category"] = static_cast<int>(category);
        j["severity"] = static_cast<int>(severity);
        j["file"] = file;
        j["line"] = line;
        j["function"] = function;
        j["timestamp"] = std::chrono::system_clock::to_time_t(timestamp);
        j["context"] = context;
        j["stackTrace"] = stackTrace;
        j["correlationId"] = correlationId;
        return j;
    }
    
    // JSON deserialization
    static ErrorEntry fromJson(const json& j) {
        ErrorEntry entry;
        entry.code = static_cast<ErrorCode>(j.value("code", 9999));
        entry.message = j.value("message", "");
        entry.category = static_cast<ErrorCategory>(j.value("category", 0));
        entry.severity = static_cast<ErrorSeverity>(j.value("severity", 3));
        entry.file = j.value("file", "");
        entry.line = j.value("line", 0);
        entry.function = j.value("function", "");
        if (j.contains("timestamp")) {
            auto time_t_val = j["timestamp"].get<time_t>();
            entry.timestamp = std::chrono::system_clock::from_time_t(time_t_val);
        }
        entry.context = j.value("context", std::map<std::string, std::string>{});
        entry.stackTrace = j.value("stackTrace", "");
        entry.correlationId = j.value("correlationId", "");
        return entry;
    }
};

// Error pattern for analysis
struct ErrorPattern {
    ErrorCode code;
    std::string patternType;
    std::chrono::system_clock::time_point firstOccurrence;
    std::chrono::system_clock::time_point lastOccurrence;
    int occurrenceCount;
    std::map<std::string, std::string> patternData;
    std::string suggestedAction;
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    // Error handling methods
    void handleError(const NLMBaseException& error);
    void handleError(ErrorCode code, const std::string& message, 
                    const std::string& file = "", int line = 0,
                    const std::string& function = "");
    void handleError(const std::exception& error);
    
    // Error recovery methods
    RecoveryResult recover(const NLMBaseException& error, const std::string& operation = "");
    RecoveryResult retry(const std::function<void()>& operation, 
                       const RetryConfig& config = RetryConfig());
    
    // Configuration methods
    void setRecoveryStrategy(ErrorCode code, RecoveryStrategy strategy);
    void setRetryConfig(const RetryConfig& config);
    void setMaxRetries(int maxRetries);
    void setEnableCheckpointRecovery(bool enable);
    
    // Checkpoint/restore methods
    bool saveCheckpoint(const std::string& name);
    bool restoreCheckpoint(const std::string& name);
    void clearCheckpoints();
    
    // Monitoring methods
    const std::vector<ErrorEntry>& getRecentErrors() const;
    const ErrorStats& getErrorStats(ErrorCode code) const;
    std::map<ErrorCode, ErrorStats> getAllErrorStats() const;
    
    // Configuration methods
    void setEnableStructuredLogging(bool enable);
    void setStructuredLogFile(const std::string& filepath);
    void setEnableErrorCorrelation(bool enable);
    void setAlertThreshold(double threshold);
    
    // Error analysis methods
    std::vector<ErrorPattern> detectErrorPatterns(const std::chrono::system_clock::time_point& since);
    double calculateErrorRate(const std::chrono::system_clock::time_point& since) const;
    std::string analyzeRootCause(const ErrorEntry& error) const;
    
    // Utility methods
    std::string getErrorCodeString(ErrorCode code) const;
    std::string getErrorCategoryString(ErrorCategory category) const;
    std::string getErrorSeverityString(ErrorSeverity severity) const;
    
    // Statistics methods
    size_t getTotalErrorCount() const;
    double getErrorRatePerMinute() const;
    std::chrono::system_clock::time_point getLastErrorTime() const;
    
    // Configuration
    struct Config {
        bool enableStructuredLogging = false;
        bool enableErrorCorrelation = true;
        bool enableCheckpointRecovery = true;
        bool enableAutoRecovery = true;
        std::string structuredLogFile = "error_log.json";
        double alertThreshold = 0.1; // 10% error rate
        int maxErrorsToStore = 1000;
        std::chrono::minutes errorStatsWindow = std::chrono::minutes(60);
        bool enableErrorPatternDetection = true;
        bool enableRootCauseAnalysis = true;
    };
    
    void setConfig(const Config& config);
    const Config& getConfig() const;

private:
    ErrorHandler();
    ~ErrorHandler();
    
    // Disable copying
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    // Internal methods
    void logError(const ErrorEntry& entry);
    void logStructuredError(const ErrorEntry& entry);
    bool isRetryable(ErrorCode code) const;
    RecoveryResult executeRecoveryStrategy(const NLMBaseException& error, 
                                         const std::string& operation);
    void updateErrorStats(const ErrorEntry& entry);
    void checkAlertThreshold() const;
    void addErrorEntry(const ErrorEntry& entry);
    std::string generateStackTrace() const;
    std::string generateCorrelationId() const;
    
    // Data members
    std::vector<ErrorEntry> errorHistory_;
    std::map<ErrorCode, ErrorStats> errorStats_;
    std::map<ErrorCode, RecoveryStrategy> recoveryStrategies_;
    RetryConfig retryConfig_;
    Config config_;
    mutable std::mutex mutex_;
    std::map<std::string, json> checkpoints_;
    std::chrono::system_clock::time_point lastAlertTime_;
    uint64_t sequenceNumber_ = 0;
};

// Convenience functions
inline void logError(ErrorCode code, const std::string& message,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "") {
    ErrorHandler::getInstance().handleError(code, message, file, line, function);
}

inline bool retryOperation(const std::function<void()>& operation,
                          const RetryConfig& config = RetryConfig()) {
    return ErrorHandler::getInstance().retry(operation, config) == RecoveryResult::Success;
}

inline RecoveryResult recoverError(const NLMBaseException& error,
                                  const std::string& operation = "") {
    return ErrorHandler::getInstance().recover(error, operation);
}

} // namespace nlm

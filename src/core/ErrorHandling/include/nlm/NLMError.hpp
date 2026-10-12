// Base NLM error hierarchy for comprehensive error handling and recovery
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <stdexcept>
#include <sstream>
#include <iomanip>

// Stacktrace support (platform-specific)
#if defined(__has_include) && __has_include(<stacktrace>)
    #include <stacktrace>
#else
    #ifdef _MSC_VER
        #include <windows.h>
        #include <dbghelp.h>
        #pragma comment(lib, "dbghelp.lib")
    #else
        #include <execinfo.h>
    #endif
#endif

namespace nlm {

// Error categorization for different types of failures
enum class ErrorCategory {
    System,      // Infrastructure, hardware, OS-level issues
    User,        // Invalid input, misuse, configuration errors
    Temporary,   // Transient failures, network issues, resource contention
    Permanent,   // Irreversible failures, data corruption, hardware failure
    Security,    // Authentication, authorization, privacy issues
    Resource,    // Memory, disk, network resource exhaustion
    Neural,      // Neural network specific errors
    Memory,      // Memory management errors
    Validation,  // Data validation failures
    Serialization, // Data serialization/deserialization errors
    Configuration, // Configuration errors
    Network,     // Network communication errors
    Performance, // Performance-related errors
    Unknown      // Uncategorized errors
};

// Error severity levels for filtering and prioritization
enum class ErrorSeverity {
    Debug,       // Fine-grained informational events
    Info,        // Important informational events
    Warning,     // Warning conditions that may become errors
    Error,       // Error conditions that prevent some functionality
    Critical,    // Critical errors that affect core functionality
    Fatal,       // Unrecoverable errors that require immediate attention
    Alert        // Emergency conditions requiring immediate response
};

// Error classification for automated handling
enum class ErrorClassification {
    Retryable,      // Can be recovered from by retry
    Fallback,       // Can use fallback functionality
    Degrade,        // Can reduce functionality gracefully
    Checkpoint,     // Requires checkpoint/restore
    Repair,         // Requires system repair
    Isolate,        // Can isolate the problem
    Escalate,       // Requires escalation to human operator
    Ignore,         // Can be ignored safely
    None            // No specific classification
};

// Base NLMError class with comprehensive error context and functionality
class NLMError : public std::exception {
public:
    NLMError(const std::string& message,
             ErrorSeverity severity = ErrorSeverity::Error,
             ErrorCategory category = ErrorCategory::Unknown,
             ErrorClassification classification = ErrorClassification::None)
        : message_(message),
          severity_(severity),
          category_(category),
          classification_(classification),
          timestamp_(std::chrono::system_clock::now()),
          threadId_(std::this_thread::get_id()),
          processId_(getpid()),
          stackTrace_(),
          backtrace_(extractBacktrace()),
          context_(),
          diagnostics_(),
          suggestions_(),
          recoveryActions_(),
          retryCount_(0),
          maxRetries_(3),
          recoverySuccessful_(false),
          metadata_() {
        // Capture stacktrace if available
        #if defined(__has_include) && __has_include(<stacktrace>)
            stackTrace_ = std::stacktrace::current();
        #endif
    }

    virtual ~NLMError() noexcept override {}

    // Error message handling
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }

    // Error details and context
    virtual std::string getMessage() const { return message_; }
    virtual ErrorSeverity getSeverity() const { return severity_; }
    virtual ErrorCategory getCategory() const { return category_; }
    virtual ErrorClassification getClassification() const { return classification_; }

    // Temporal information
    virtual std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    virtual std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp_);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F");
        return ss.str();
    }

    // Source location information
    virtual std::string getSourceFile() const { return sourceFile_; }
    virtual int getSourceLine() const { return sourceLine_; }
    virtual std::string getSourceFunction() const { return sourceFunction_; }
    virtual std::string getSourceLocation() const {
        if (!sourceFile_.empty() && sourceLine_ > 0) {
            return sourceFile_ + ":" + std::to_string(sourceLine_);
        }
        return "";
    }

    // Thread and process information
    virtual std::thread::id getThreadId() const { return threadId_; }
    virtual pid_t getProcessId() const { return processId_; }

    // Stack trace and diagnostics
    virtual const std::stacktrace& getStackTrace() const { return stackTrace_; }
    virtual std::string getFormattedStackTrace() const {
        std::stringstream ss;
        ss << "Stack Trace:\n";
        for (const auto& frame : stackTrace_) {
            ss << "  " << frame << "\n";
        }
        return ss.str();
    }
    virtual const std::string& getBacktrace() const { return backtrace_; }

    // Error context and diagnostics
    template<typename T>
    void addContext(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(contextMutex_);
        context_[key] = std::to_string(value);
    }

    template<typename T>
    void addDiagnostic(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(diagnosticMutex_);
        diagnostics_[key] = std::to_string(value);
    }

    template<typename T>
    void addSuggestion(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(suggestionMutex_);
        suggestions_[key] = std::to_string(value);
    }

    template<typename T>
    void addRecoveryAction(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(recoveryActionMutex_);
        recoveryActions_[key] = std::to_string(value);
    }

    template<typename T>
    void addMetadata(const std::string& key, const T& value) {
        std::lock_guard<std::mutex> lock(metadataMutex_);
        metadata_[key] = std::to_string(value);
    }

    // Context accessors
    virtual const std::unordered_map<std::string, std::string>& getContext() const { return context_; }
    virtual const std::unordered_map<std::string, std::string>& getDiagnostics() const { return diagnostics_; }
    virtual const std::unordered_map<std::string, std::string>& getSuggestions() const { return suggestions_; }
    virtual const std::unordered_map<std::string, std::string>& getRecoveryActions() const { return recoveryActions_; }
    virtual const std::unordered_map<std::string, std::string>& getMetadata() const { return metadata_; }

    // Performance metrics during error
    virtual uint64_t getPeakMemoryUsage() const { return peakMemoryUsage_; }
    virtual double getProcessingTimeMs() const { return processingTimeMs_; }
    virtual uint64_t getCPUUsageCycles() const { return cpuUsageCycles_; }

    // Error statistics
    virtual uint32_t getRetryCount() const { return retryCount_; }
    virtual uint32_t getMaxRetries() const { return maxRetries_; }
    virtual bool isRecoverySuccessful() const { return recoverySuccessful_; }
    virtual uint32_t getErrorFrequency() const { return errorFrequency_; }

    // Recovery management
    virtual void incrementRetryCount() { retryCount_++; }
    virtual void setMaxRetries(uint32_t max) { maxRetries_ = max; }
    virtual void setRecoverySuccessful(bool success) { recoverySuccessful_ = success; }
    virtual void incrementErrorFrequency() { errorFrequency_++; }

    // Error formatting
    virtual std::string toString() const;
    virtual std::string toJSON() const;
    virtual std::string toYAML() const;
    virtual std::string toXML() const;

    // Error comparison and classification
    virtual bool operator==(const NLMError& other) const;
    virtual bool operator!=(const NLMError& other) const { return !(*this == other); }

    // Error filtering and matching
    virtual bool matches(const std::string& pattern) const;
    virtual bool matchesSeverity(ErrorSeverity minSeverity) const;
    virtual bool matchesCategory(ErrorCategory category) const;

    // Error handling integration
    virtual void logError() const;
    virtual void reportError() const;
    virtual void alertError() const;
    virtual bool attemptRecovery() const;

    // Factory methods for specific error types
    static std::unique_ptr<NLMError> createConfigurationError(const std::string& message, const std::string& field = "", const std::string& suggestion = "");
    static std::unique_ptr<NLMError> createValidationError(const std::string& message, const std::string& field = "", const std::string& suggestion = "");
    static std::unique_ptr<NLMError> createSerializationError(const std::string& message, const std::string& component = "", const std::string& suggestion = "");
    static std::unique_ptr<NLMError> createNeuralError(const std::string& message, const std::string& operation = "", const std::string& suggestion = "");
    static std::unique_ptr<NLMError> createMemoryError(const std::string& message, size_t memoryAddress = 0, const std::string& suggestion = "");
    static std::unique_ptr<NLMError> createSystemError(const std::string& message, const std::string& system = "", const std::string& suggestion = "");

protected:
    virtual std::string extractBacktrace() const;

    // Error state
    std::string message_;
    ErrorSeverity severity_;
    ErrorCategory category_;
    ErrorClassification classification_;
    std::chrono::system_clock::time_point timestamp_;
    std::thread::id threadId_;
    pid_t processId_;
    std::stacktrace stackTrace_;
    std::string backtrace_;

    // Source location (populated by error handling macros)
    std::string sourceFile_;
    int sourceLine_;
    std::string sourceFunction_;

    // Context information
    std::unordered_map<std::string, std::string> context_;
    std::unordered_map<std::string, std::string> diagnostics_;
    std::unordered_map<std::string, std::string> suggestions_;
    std::unordered_map<std::string, std::string> recoveryActions_;
    std::unordered_map<std::string, std::string> metadata_;

    // Thread safety
    mutable std::mutex contextMutex_;
    mutable std::mutex diagnosticMutex_;
    mutable std::mutex suggestionMutex_;
    mutable std::mutex recoveryActionMutex_;
    mutable std::mutex metadataMutex_;

    // Performance metrics
    uint64_t peakMemoryUsage_;
    double processingTimeMs_;
    uint64_t cpuUsageCycles_;

    // Error statistics
    uint32_t retryCount_;
    uint32_t maxRetries_;
    bool recoverySuccessful_;
    uint32_t errorFrequency_;
};

// Type aliases for convenience
using NLMErrorPtr = std::unique_ptr<NLMError>;
using NLMErrorConstPtr = std::unique_ptr<const NLMError>;
using NLMErrorSharedPtr = std::shared_ptr<NLMError>;
using NLMErrorConstSharedPtr = std::shared_ptr<const NLMError>;

} // namespace nlm
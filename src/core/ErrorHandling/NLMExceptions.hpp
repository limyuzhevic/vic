#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <vector>
#include <chrono>
#include <exception>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <variant>
#include <functional>
#include <ctime>

namespace nlm {

// Exception severity levels
enum class ExceptionSeverity {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

// NLM-specific exception categories
enum class ExceptionCategory {
    // Core system errors
    INITIALIZATION_FAILURE,
    CONFIGURATION_ERROR,
    MEMORY_ALLOCATION_FAILURE,
    NETWORK_ERROR,
    // Neural network errors
    NEURON_ERROR,
    SYNAPSE_ERROR,
    BRAIN_ERROR,
    PLASTICITY_ERROR,
    DYNAMICS_ERROR,
    // Cognitive errors
    CONCEPT_FORMATION_ERROR,
    PLANNING_ERROR,
    PERCEPTION_ERROR,
    ACTION_ERROR,
    // Neuromodulation errors
    DOPAMINE_ERROR,
    REWARD_ERROR,
    NOVELTY_ERROR,
    CURIOUSITY_ERROR,
    PREDICTION_ERROR,
    // Memory system errors
    WORKING_MEMORY_ERROR,
    EPISODIC_MEMORY_ERROR,
    SEMANTIC_MEMORY_ERROR,
    PROCEDURAL_MEMORY_ERROR,
    ASSOCIATIVE_MEMORY_ERROR,
    // System-level errors
    THREADING_ERROR,
    RESOURCE_EXHAUSTION,
    TIMEOUT_ERROR,
    PLUGIN_LOAD_ERROR,
    VALIDATION_ERROR,
    UNKNOWN_ERROR
};

// Base NLM exception
class NLMException : public std::exception {
public:
    NLMException(const std::string& message, ExceptionCategory category, 
                 ExceptionSeverity severity = ExceptionSeverity::ERROR,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : message_(message), category_(category), severity_(severity),
          file_(file), line_(line), function_(function),
          timestamp_(std::chrono::system_clock::now()) {
        
        // Build detailed message with context
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(severity_) << "] ";
        oss << categoryToString(category_) << ": ";
        oss << message_;
        
        if (!file_.empty()) {
            oss << " (" << file_ << ":" << line_ << ")";
            if (!function_.empty()) {
                oss << " in " << function_;
            }
        }
        
        fullMessage_ = oss.str();
        buildContextStack();
    }
    
    virtual ~NLMException() noexcept = default;
    
    // Exception chaining support
    template<typename... Args>
    NLMException& addContext(const std::string& context, const Args&... args) {
        ContextInfo info;
        info.message = format(context, args...);
        info.timestamp = std::chrono::system_clock::now();
        contextStack_.push_back(info);
        return *this;
    }
    
    // Exception chaining operator
    NLMException& operator<<(const std::string& context) {
        return addContext(context);
    }
    
    const char* what() const noexcept override {
        return fullMessage_.c_str();
    }
    
    // Context and metadata access
    const std::string& getMessage() const { return message_; }
    ExceptionCategory getCategory() const { return category_; }
    ExceptionSeverity getSeverity() const { return severity_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp_; }
    const std::vector<ContextInfo>& getContextStack() const { return contextStack_; }
    
    // Recovery strategy
    virtual bool canRecover() const { return false; }
    
    // Error statistics
    struct ErrorStats {
        ExceptionCategory category;
        ExceptionSeverity severity;
        std::chrono::system_clock::time_point firstOccurrence;
        uint64_t occurrenceCount;
        std::vector<std::string> affectedFiles;
        double recoverySuccessRate;
    };
    
    static std::unordered_map<ExceptionCategory, ErrorStats>& getErrorStats() {
        static std::unordered_map<ExceptionCategory, ErrorStats> stats;
        return stats;
    }
    
protected:
    void buildContextStack() {
        // Capture stack trace here
        contextStack_.push_back({message_, timestamp_});
    }
    
    std::string format(const std::string& format, const auto&... args) {
        // Simple string formatting - would use more robust implementation in production
        return format;  // Placeholder
    }
    
private:
    std::string message_;
    ExceptionCategory category_;
    ExceptionSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::string fullMessage_;
    std::vector<ContextInfo> contextStack_;
    
    static std::string categoryToString(ExceptionCategory category) {
        switch (category) {
            case ExceptionCategory::INITIALIZATION_FAILURE: return "INITIALIZATION_FAILURE";
            case ExceptionCategory::CONFIGURATION_ERROR: return "CONFIGURATION_ERROR";
            case ExceptionCategory::MEMORY_ALLOCATION_FAILURE: return "MEMORY_ALLOCATION_FAILURE";
            case ExceptionCategory::NETWORK_ERROR: return "NETWORK_ERROR";
            case ExceptionCategory::NEURON_ERROR: return "NEURON_ERROR";
            case ExceptionCategory::SYNAPSE_ERROR: return "SYNAPSE_ERROR";
            case ExceptionCategory::BRAIN_ERROR: return "BRAIN_ERROR";
            case ExceptionCategory::PLASTICITY_ERROR: return "PLASTICITY_ERROR";
            case ExceptionCategory::DYNAMICS_ERROR: return "DYNAMICS_ERROR";
            case ExceptionCategory::CONCEPT_FORMATION_ERROR: return "CONCEPT_FORMATION_ERROR";
            case ExceptionCategory::PLANNING_ERROR: return "PLANNING_ERROR";
            case ExceptionCategory::PERCEPTION_ERROR: return "PERCEPTION_ERROR";
            case ExceptionCategory::ACTION_ERROR: return "ACTION_ERROR";
            case ExceptionCategory::DOPAMINE_ERROR: return "DOPAMINE_ERROR";
            case ExceptionCategory::REWARD_ERROR: return "REWARD_ERROR";
            case ExceptionCategory::NOVELTY_ERROR: return "NOVELTY_ERROR";
            case ExceptionCategory::CURIOUSITY_ERROR: return "CURIOUSITY_ERROR";
            case ExceptionCategory::PREDICTION_ERROR: return "PREDICTION_ERROR";
            case ExceptionCategory::WORKING_MEMORY_ERROR: return "WORKING_MEMORY_ERROR";
            case ExceptionCategory::EPISODIC_MEMORY_ERROR: return "EPISODIC_MEMORY_ERROR";
            case ExceptionCategory::SEMANTIC_MEMORY_ERROR: return "SEMANTIC_MEMORY_ERROR";
            case ExceptionCategory::PROCEDURAL_MEMORY_ERROR: return "PROCEDURAL_MEMORY_ERROR";
            case ExceptionCategory::ASSOCIATIVE_MEMORY_ERROR: return "ASSOCIATIVE_MEMORY_ERROR";
            case ExceptionCategory::THREADING_ERROR: return "THREADING_ERROR";
            case ExceptionCategory::RESOURCE_EXHAUSTION: return "RESOURCE_EXHAUSTION";
            case ExceptionCategory::TIMEOUT_ERROR: return "TIMEOUT_ERROR";
            case ExceptionCategory::PLUGIN_LOAD_ERROR: return "PLUGIN_LOAD_ERROR";
            case ExceptionCategory::VALIDATION_ERROR: return "VALIDATION_ERROR";
            case ExceptionCategory::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
            default: return "UNKNOWN";
        }
    }
};

// Specialized exception types
class NLMInitException : public NLMException {
public:
    NLMInitException(const std::string& message, ExceptionCategory category = ExceptionCategory::INITIALIZATION_FAILURE,
                     ExceptionSeverity severity = ExceptionSeverity::CRITICAL)
        : NLMException(message, category, severity) {}
};

class NLMMemoryException : public NLMException {
public:
    NLMMemoryException(const std::string& message, ExceptionCategory category = ExceptionCategory::MEMORY_ALLOCATION_FAILURE,
                      ExceptionSeverity severity = ExceptionSeverity::ERROR)
        : NLMException(message, category, severity) {}
};

class NLMConfigException : public NLMException {
public:
    NLMConfigException(const std::string& message, ExceptionCategory category = ExceptionCategory::CONFIGURATION_ERROR,
                      ExceptionSeverity severity = ExceptionSeverity::WARNING)
        : NLMException(message, category, severity) {}
};

class NLMNetworkException : public NLMException {
public:
    NLMNetworkException(const std::string& message, ExceptionCategory category = ExceptionCategory::NETWORK_ERROR,
                       ExceptionSeverity severity = ExceptionSeverity::ERROR)
        : NLMException(message, category, severity) {}
};

// Error reporting and collection system
class ErrorReporter {
public:
    struct ErrorReport {
        std::string id;
        std::string message;
        ExceptionCategory category;
        ExceptionSeverity severity;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point timestamp;
        std::vector<std::string> stackTrace;
        bool recovered;
        std::string recoveryAction;
    };
    
    using ErrorCallback = std::function<void(const ErrorReport&)>;
    
    ErrorReporter() : recoveryEnabled_(false), maxHistorySize_(1000) {}
    
    // Thread-safe error reporting
    template<typename TException>
    void report(const TException& exception, bool autoRecover = false) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        ErrorReport report;
        report.id = generateReportId();
        report.message = exception.what();
        report.category = exception.getCategory();
        report.severity = exception.getSeverity();
        report.file = exception.getFile();
        report.line = exception.getLine();
        report.function = exception.getFunction();
        report.timestamp = exception.getTimestamp();
        report.stackTrace = extractStackTrace(exception);
        report.recovered = false;
        report.recoveryAction = "none";
        
        // Store error
        errorHistory_.push_back(report);
        
        // Trim history if too large
        if (errorHistory_.size() > maxHistorySize_) {
            errorHistory_.erase(errorHistory_.begin(), errorHistory_.begin() + 50);
        }
        
        // Try automatic recovery if enabled
        if (autoRecover && exception.canRecover()) {
            report.recoveryAction = "auto_recovered";
            report.recovered = true;
            // In real implementation, would perform recovery
        }
        
        // Notify callbacks
        for (const auto& callback : callbacks_) {
            callback(report);
        }
        
        // Update statistics
        updateErrorStats(report);
    }
    
    // Recovery mechanism
    bool attemptRecovery(const ErrorReport& report, const std::string& strategy) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Implement different recovery strategies
        if (strategy == "restart_component") {
            // Restart affected component
            return restartComponent(report.category);
        } else if (strategy == "reset_state") {
            // Reset to safe state
            return resetToSafeState(report.category);
        } else if (strategy == "fallback") {
            // Use fallback behavior
            return useFallbackBehavior(report.category);
        }
        
        return false;
    }
    
    // Error statistics and analytics
    struct SystemErrorStats {
        uint64_t totalErrors;
        uint64_t recoveredErrors;
        uint64_t unrecoveredErrors;
        std::unordered_map<ExceptionCategory, uint64_t> errorsByCategory;
        std::unordered_map<ExceptionSeverity, uint64_t> errorsBySeverity;
        double averageRecoveryTime;
        std::vector<ErrorReport> recentErrors;
        std::chrono::system_clock::time_point lastErrorTime;
    };
    
    SystemErrorStats getSystemStats() const {
        SystemErrorStats stats;
        stats.totalErrors = errorHistory_.size();
        stats.recoveredErrors = std::count_if(errorHistory_.begin(), errorHistory_.end(),
                                             [](const ErrorReport& r) { return r.recovered; });
        stats.unrecoveredErrors = stats.totalErrors - stats.recoveredErrors;
        
        for (const auto& error : errorHistory_) {
            stats.errorsByCategory[error.category]++;
            stats.errorsBySeverity[error.severity]++;
        }
        
        if (!errorHistory_.empty()) {
            stats.lastErrorTime = errorHistory_.back().timestamp;
        }
        
        // Get recent errors (last 100)
        stats.recentErrors = std::vector<ErrorReport>(
            errorHistory_.end() - std::min(size_t(100), errorHistory_.size()),
            errorHistory_.end()
        );
        
        return stats;
    }
    
    // Callback management
    void addErrorCallback(ErrorCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_.push_back(callback);
    }
    
    void setRecoveryEnabled(bool enabled) { recoveryEnabled_ = enabled; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    
    // Thread-safe access to error history
    std::vector<ErrorReport> getRecentErrors(size_t limit = 100) const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = std::min(limit, errorHistory_.size());
        return std::vector<ErrorReport>(errorHistory_.end() - count, errorHistory_.end());
    }
    
    const std::vector<ErrorReport>& getAllErrors() const {
        return errorHistory_;
    }
    
private:
    std::string generateReportId() {
        static std::atomic<uint64_t> idCounter{0};
        return "ERR_" + std::to_string(++idCounter);
    }
    
    std::vector<std::string> extractStackTrace(const NLMException& exception) const {
        // In real implementation, would capture actual stack trace
        return {"stack_trace_placeholder"};
    }
    
    void updateErrorStats(const ErrorReport& report) {
        // Update error statistics
    }
    
    bool restartComponent(ExceptionCategory category) {
        // Implement component restart logic
        return true;
    }
    
    bool resetToSafeState(ExceptionCategory category) {
        // Implement safe state reset
        return true;
    }
    
    bool useFallbackBehavior(ExceptionCategory category) {
        // Implement fallback behavior
        return true;
    }
    
    mutable std::mutex mutex_;
    std::vector<ErrorReport> errorHistory_;
    std::vector<ErrorCallback> callbacks_;
    bool recoveryEnabled_;
    size_t maxHistorySize_;
};

// Global error reporter instance
inline ErrorReporter& getErrorReporter() {
    static ErrorReporter reporter;
    return reporter;
}

// Convenience functions for error reporting
inline void reportError(const NLMException& exception, bool autoRecover = false) {
    getErrorReporter().report(exception, autoRecover);
}

inline void reportError(const std::string& message, ExceptionCategory category, 
                       ExceptionSeverity severity = ExceptionSeverity::ERROR) {
    NLMException exception(message, category, severity);
    getErrorReporter().report(exception);
}

// Macro for throwing with context
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

} // namespace nlm

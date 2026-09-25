// Comprehensive Error Handling Framework for NLM
// Enhanced error management with recovery and diagnostics

#ifndef NLM_ERROR_HANDLING_H
#define NLM_ERROR_HANDLING_H

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <optional>
#include <future>

namespace nlm {

// Enhanced error severity levels
enum class ErrorSeverity {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Fatal = 5
};

// Enhanced error categories
enum class ErrorCategory {
    System = 0,
    Configuration = 1,
    Memory = 2,
    Neural = 3,
    Plasticity = 4,
    Sensory = 5,
    Motor = 6,
    Integration = 7,
    Development = 8,
    External = 9
};

// Error context information
struct ErrorContext {
    std::string component;
    std::string operation;
    std::string file;
    int line = 0;
    std::string function;
    std::unordered_map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext(const std::string& comp = "", const std::string& op = "",
                 const std::string& file = "", int line = 0,
                 const std::string& func = "")
        : component(comp), operation(op), file(file), line(line), 
          function(func), timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedLocation() const {
        std::stringstream ss;
        if (!file.empty()) {
            ss << file;
            if (line > 0) ss << ":" << line;
        }
        if (!function.empty()) {
            if (!file.empty()) ss << " in ";
            ss << function;
        }
        return ss.str();
    }
};

// Enhanced error information structure
struct EnhancedErrorInfo {
    NLMErrors type;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    ErrorContext context;
    std::optional<std::string> recoveryAction;
    std::vector<std::string> suggestedSteps;
    bool canAutoRecover = false;
    bool canRetry = false;
    int retryCount = 0;
    std::chrono::milliseconds retryDelay = std::chrono::milliseconds(100);
    std::unordered_map<std::string, std::string> errorData;
    
    EnhancedErrorInfo(NLMErrors t, const std::string& msg, 
                     ErrorSeverity sev = ErrorSeverity::Error,
                     ErrorCategory cat = ErrorCategory::System)
        : type(t), severity(sev), category(cat), message(msg), canAutoRecover(false),
          canRetry(false), retryCount(0) {}
    
    std::string getDetailedMessage() const {
        std::stringstream ss;
        ss << "[" << errorSeverityToString(severity) << "] "
           << "[" << errorCategoryToString(category) << "] "
           << "[" << errorTypeToString(type) << "] "
           << message;
        if (!context.component.empty()) {
            ss << " (Component: " << context.component << ")";
        }
        if (!context.operation.empty()) {
            ss << " (Operation: " << context.operation << ")";
        }
        if (!context.getFormattedLocation().empty()) {
            ss << " (Location: " << context.getFormattedLocation() << ")";
        }
        return ss.str();
    }
    
private:
    static std::string errorSeverityToString(ErrorSeverity sev) {
        static const std::vector<std::string> names = {
            "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL"
        };
        return sev < names.size() ? names[static_cast<size_t>(sev)] : "UNKNOWN";
    }
    
    static std::string errorCategoryToString(ErrorCategory cat) {
        static const std::vector<std::string> names = {
            "SYSTEM", "CONFIGURATION", "MEMORY", "NEURAL", "PLASTICITY",
            "SENSORY", "MOTOR", "INTEGRATION", "DEVELOPMENT", "EXTERNAL"
        };
        return cat < names.size() ? names[static_cast<size_t>(cat)] : "UNKNOWN";
    }
    
    static std::string errorTypeToString(NLMErrors type) {
        static const std::vector<std::string> names = {
            "ConfigParsing", "FileIO", "MemoryAllocation", "InvalidParameter",
            "SystemNotInitialized", "PlasticityFailed", "NeuronInvalid",
            "SynapseInvalid", "CallbackError", "IntegrationFailed",
            "DevelopmentError", "ValidationError", "UnknownError"
        };
        return type < names.size() ? names[static_cast<size_t>(type)] : "UNKNOWN";
    }
};

// Enhanced error manager with recovery and diagnostics
class EnhancedErrorManager {
public:
    // Error callback for monitoring
    using ErrorCallback = std::function<void(const EnhancedErrorInfo&)>
    // Recovery callback for automatic recovery
    using RecoveryCallback = std::function<bool(const EnhancedErrorInfo&)>;
    
    // Error manager singleton
    static EnhancedErrorManager& getInstance() {
        static EnhancedErrorManager instance;
        return instance;
    }
    
    // Error registration and management
    void addError(const EnhancedErrorInfo& error) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorHistory_.push_back(error);
        
        // Trigger error callbacks
        for (const auto& callback : errorCallbacks_) {
            try {
                callback(error);
            } catch (const std::exception& e) {
                // Log callback errors but don't propagate
                logError(EnhancedErrorInfo(NLMErrors::UnknownError,
                    "Error in error callback: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System));
            }
        }
        
        // Check for recovery opportunities
        attemptRecovery(error);
    }
    
    void addError(NLMErrors type, const std::string& message,
                  ErrorSeverity severity = ErrorSeverity::Error,
                  ErrorCategory category = ErrorCategory::System,
                  const ErrorContext& context = ErrorContext()) {
        EnhancedErrorInfo error(type, message, severity, category);
        error.context = context;
        addError(error);
    }
    
    // Error retrieval and analysis
    std::vector<EnhancedErrorInfo> getRecentErrors(size_t count = 100) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        size_t start = std::max<size_t>(0, errorHistory_.size() - count);
        return std::vector<EnhancedErrorInfo>(errorHistory_.begin() + start, errorHistory_.end());
    }
    
    std::vector<EnhancedErrorInfo> getErrorsByType(NLMErrors type) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<EnhancedErrorInfo> filtered;
        for (const auto& error : errorHistory_) {
            if (error.type == type) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    std::vector<EnhancedErrorInfo> getErrorsByCategory(ErrorCategory category) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<EnhancedErrorInfo> filtered;
        for (const auto& error : errorHistory_) {
            if (error.category == category) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    std::vector<EnhancedErrorInfo> getErrorsBySeverity(ErrorSeverity severity) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<EnhancedErrorInfo> filtered;
        for (const auto& error : errorHistory_) {
            if (error.severity == severity) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    // Error statistics and analysis
    size_t getErrorCount() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return errorHistory_.size();
    }
    
    size_t getErrorCount(NLMErrors type) const {
        auto errors = getErrorsByType(type);
        return errors.size();
    }
    
    size_t getErrorCount(ErrorSeverity severity) const {
        auto errors = getErrorsBySeverity(severity);
        return errors.size();
    }
    
    double getErrorRatePerMinute() const {
        if (errorHistory_.empty()) return 0.0;
        
        auto now = std::chrono::system_clock::now();
        auto firstErrorTime = errorHistory_.front().context.timestamp;
        auto timeSpan = std::chrono::duration_cast<std::chrono::minutes>(now - firstErrorTime);
        if (timeSpan.count() == 0) return static_cast<double>(errorHistory_.size());
        
        return static_cast<double>(errorHistory_.size()) / timeSpan.count();
    }
    
    // Error callbacks
    void addErrorCallback(ErrorCallback callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorCallbacks_.push_back(callback);
    }
    
    void removeErrorCallback(ErrorCallback callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorCallbacks_.erase(std::remove(errorCallbacks_.begin(), errorCallbacks_.end(), callback),
                             errorCallbacks_.end());
    }
    
    void clearErrorCallbacks() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorCallbacks_.clear();
    }
    
    // Recovery management
    void addRecoveryCallback(RecoveryCallback callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        recoveryCallbacks_.push_back(callback);
    }
    
    void removeRecoveryCallback(RecoveryCallback callback) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        recoveryCallbacks_.erase(std::remove(recoveryCallbacks_.begin(), recoveryCallbacks_.end(), callback),
                                recoveryCallbacks_.end());
    }
    
    // Error clearing and maintenance
    void clearErrors() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorHistory_.clear();
    }
    
    void clearErrors(NLMErrors type) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        errorHistory_.erase(std::remove_if(errorHistory_.begin(), errorHistory_.end(),
                                          [type](const EnhancedErrorInfo& error) { return error.type == type; }),
                           errorHistory_.end());
    }
    
    // Diagnostics and monitoring
    std::string generateErrorReport() const {
        std::stringstream ss;
        ss << "=== NLM Enhanced Error Report ===" << std::endl;
        ss << "Total Errors: " << errorHistory_.size() << std::endl;
        ss << "Error Rate: " << std::fixed << std::setprecision(2) 
           << getErrorRatePerMinute() << " errors/minute" << std::endl;
        
        // Error distribution by type
        ss << "\n=== Error Distribution by Type ===" << std::endl;
        for (int i = 0; i < 13; ++i) {
            NLMErrors type = static_cast<NLMErrors>(i);
            auto count = getErrorCount(type);
            if (count > 0) {
                ss << errorTypeToString(type) << ": " << count << std::endl;
            }
        }
        
        // Error distribution by severity
        ss << "\n=== Error Distribution by Severity ===" << std::endl;
        for (int i = 0; i < 6; ++i) {
            ErrorSeverity severity = static_cast<ErrorSeverity>(i);
            auto count = getErrorCount(severity);
            if (count > 0) {
                ss << errorSeverityToString(severity) << ": " << count << std::endl;
            }
        }
        
        return ss.str();
    }
    
    // Error filtering and querying
    std::vector<EnhancedErrorInfo> filterErrors(std::function<bool(const EnhancedErrorInfo&)> predicate) const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        std::vector<EnhancedErrorInfo> filtered;
        for (const auto& error : errorHistory_) {
            if (predicate(error)) {
                filtered.push_back(error);
            }
        }
        return filtered;
    }
    
    std::vector<EnhancedErrorInfo> getErrorsInTimeRange(
        std::chrono::system_clock::time_point start,
        std::chrono::system_clock::time_point end) const {
        return filterErrors([start, end](const EnhancedErrorInfo& error) {
            return error.context.timestamp >= start && 
                   error.context.timestamp <= end;
        });
    }
    
protected:
    EnhancedErrorManager() = default;
    ~EnhancedErrorManager() = default;
    
    EnhancedErrorManager(const EnhancedErrorManager&) = delete;
    EnhancedErrorManager& operator=(const EnhancedErrorManager&) = delete;
    
private:
    mutable std::shared_mutex mutex_;
    std::vector<EnhancedErrorInfo> errorHistory_;
    std::vector<ErrorCallback> errorCallbacks_;
    std::vector<RecoveryCallback> recoveryCallbacks_;
    
    void attemptRecovery(const EnhancedErrorInfo& error) {
        if (!error.canAutoRecover) return;
        
        for (const auto& callback : recoveryCallbacks_) {
            try {
                if (callback(error)) {
                    logError(EnhancedErrorInfo(NLMErrors::UnknownError,
                        "Automatic recovery successful for error: " + errorTypeToString(error.type),
                        ErrorSeverity::Info, ErrorCategory::System));
                    return;
                }
            } catch (const std::exception& e) {
                logError(EnhancedErrorInfo(NLMErrors::UnknownError,
                    "Recovery callback failed: " + std::string(e.what()),
                    ErrorSeverity::Error, ErrorCategory::System));
            }
        }
    }
    
    void logError(const EnhancedErrorInfo& error) {
        // Log to both error history and standard error stream
        addError(error);
        
        // Print error to stderr for immediate visibility
        std::cerr << "[ENHANCED ERROR] " << error.getDetailedMessage() << std::endl;
        if (!error.recoveryAction.has_value()) {
            std::cerr << "  No recovery action available." << std::endl;
        }
    }
    
    // Helper functions (static)
    static std::string errorTypeToString(NLMErrors type) {
        static const std::vector<std::string> names = {
            "ConfigParsing", "FileIO", "MemoryAllocation", "InvalidParameter",
            "SystemNotInitialized", "PlasticityFailed", "NeuronInvalid",
            "SynapseInvalid", "CallbackError", "IntegrationFailed",
            "DevelopmentError", "ValidationError", "UnknownError"
        };
        return type < names.size() ? names[static_cast<size_t>(type)] : "UNKNOWN";
    }
    
    static std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::vector<std::string> names = {
            "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL"
        };
        return severity < names.size() ? names[static_cast<size_t>(severity)] : "UNKNOWN";
    }
    
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::vector<std::string> names = {
            "SYSTEM", "CONFIGURATION", "MEMORY", "NEURAL", "PLASTICITY",
            "SENSORY", "MOTOR", "INTEGRATION", "DEVELOPMENT", "EXTERNAL"
        };
        return category < names.size() ? names[static_cast<size_t>(category)] : "UNKNOWN";
    }
};

// Global enhanced error manager instance
inline EnhancedErrorManager& getEnhancedErrorManager() {
    return EnhancedErrorManager::getInstance();
}

// Enhanced error macros
#define NLM_ENHANCED_ERROR(type, message, severity, category, context) \
    do { \
        nlm::EnhancedErrorManager& mgr = nlm::EnhancedErrorManager::getInstance(); \
        nlm::EnhancedErrorInfo error(type, message, severity, category); \
        error.context = context; \
        mgr.addError(error); \
    } while(0)

#define NLM_ENHANCED_THROW(type, message, severity, category, context) \
    do { \
        nlm::EnhancedErrorManager& mgr = nlm::EnhancedErrorManager::getInstance(); \
        nlm::EnhancedErrorInfo error(type, message, severity, category); \
        error.context = context; \
        mgr.addError(error); \
        throw nlm::NLMErrorManager::Exception(type, message, context.getFormattedLocation()); \
    } while(0)

// Convenience macros
#define NLM_ERROR(type, message) \
    NLM_ENHANCED_ERROR(type, message, ErrorSeverity::Error, ErrorCategory::System, nlm::ErrorContext(__func__, #type))

#define NLM_THROW(type, message) \
    NLM_ENHANCED_THROW(type, message, ErrorSeverity::Error, ErrorCategory::System, nlm::ErrorContext(__func__, #type))

#define NLM_VALIDATE(param, condition, type, message) \
    do { \
        if (!(condition)) { \
            nlm::ErrorContext context(__func__, #type); \
            context.metadata["param_name"] = #param; \
            context.metadata["param_value"] = std::to_string(param); \
            NLM_THROW(type, message); \
        } \
    } while(0)

// Enhanced logging macros
#define NLM_LOG_DEBUG(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Debug, ErrorCategory::System, context)

#define NLM_LOG_INFO(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Info, ErrorCategory::System, context)

#define NLM_LOG_WARNING(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Warning, ErrorCategory::System, context)

#define NLM_LOG_ERROR(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Error, ErrorCategory::System, context)

#define NLM_LOG_CRITICAL(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Critical, ErrorCategory::System, context)

#define NLM_LOG_FATAL(message, context) \
    nlm::getEnhancedErrorManager().addError(NLMErrors::UnknownError, message, ErrorSeverity::Fatal, ErrorCategory::System, context)

} // namespace nlm

#endif // NLM_ERROR_HANDLING_H

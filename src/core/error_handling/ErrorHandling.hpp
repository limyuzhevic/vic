// NLM Error Handling System
// 
// Comprehensive error handling and recovery system for NLM.
// Provides robust error management, recovery mechanisms, and diagnostics.

#ifndef ERROR_HANDLING_HPP
#define ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>

namespace nlm {

// Error categories for different types of issues
enum class ErrorCategory {
    MEMORY_ERROR,
    NEURAL_ERROR,
    PLASTICITY_ERROR,
    COGNITION_ERROR,
    NEUROMODULATION_ERROR,
    DEVELOPMENT_ERROR,
    PERSISTENCE_ERROR,
    INTEGRATION_ERROR,
    CONFIGURATION_ERROR,
    SYSTEM_ERROR,
    VALIDATION_ERROR,
    RESOURCE_ERROR,
    UNKNOWN_ERROR
};

// Error severity levels
enum class ErrorSeverity {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    FATAL
};

// Error codes for specific error conditions
enum class ErrorCode {
    // Memory system errors
    MEM_NULL_POINTER = 1000,
    MEM_ALLOCATION_FAILED,
    MEM_CORRUPTION_DETECTED,
    MEM_OUT_OF_MEMORY,
    MEM_ACCESS_VIOLATION,
    
    // Neural system errors
    NEURAL_NULL_POINTER = 2000,
    NEURAL_DYNAMICS_FAILED,
    NEURAL_SYNAPSIS_FAILED,
    NEURAL_SPIKE_FAILED,
    
    // Plasticity errors
    PLASTICITY_STDP_FAILED = 3000,
    PLASTICITY_HEBBIAN_FAILED,
    PLASTICITY_STRUCTURAL_FAILED,
    PLASTICITY_ELIGIBILITY_FAILED,
    
    // Cognition errors
    COGNITION_PLANNER_FAILED = 4000,
    COGNITION_CONCEPT_FAILED,
    COGNITION_ATTENTION_FAILED,
    COGNITION_SELF_MODEL_FAILED,
    
    // Neuromodulation errors
    NEUROMOD_DOPAMINE_FAILED = 5000,
    NEUROMOD_NOVELTY_FAILED,
    NEUROMOD_CURIOUSY_FAILED,
    NEUROMOD_PREDICTION_ERROR_FAILED,
    
    // Development errors
    DEVELOPMENT_MATURATION_FAILED = 6000,
    DEVELOPMENT_PRUNING_FAILED,
    DEVELOPMENT_SYNAPTOGENESIS_FAILED,
    
    // Persistence errors
    PERSISTENCE_SAVE_FAILED = 7000,
    PERSISTENCE_LOAD_FAILED,
    PERSISTENCE_CORRUPTION_DETECTED,
    PERSISTENCE_VERSION_MISMATCH,
    
    // Integration errors
    INTEGRATION_FAILED = 8000,
    INTEGRATION_TIMEOUT,
    INTEGRATION_PARTIAL,
    INTEGRATION_NOT_READY,
    
    // Configuration errors
    CONFIG_INVALID_VALUE = 9000,
    CONFIG_MISSING_KEY,
    CONFIG_TYPE_MISMATCH,
    CONFIG_VALIDATION_FAILED,
    
    // Resource errors
    RESOURCE_NOT_FOUND = 10000,
    RESOURCE_BUSY,
    RESOURCE_LOCKED,
    RESOURCE_TIMEOUT,
    
    // System errors
    SYSTEM_INITIALIZATION_FAILED = 11000,
    SYSTEM_SHUTDOWN_FAILED,
    SYSTEM_INVALID_STATE,
    SYSTEM_CORRUPTION,
    
    // Validation errors
    VALIDATION_FAILED = 12000,
    VALIDATION_TIMEOUT,
    VALIDATION_INCOMPLETE,
    VALIDATION_CONFLICT,
    
    // Generic errors
    UNKNOWN_ERROR = 99999,
    OUT_OF_RANGE,
    NOT_IMPLEMENTED,
    NOT_SUPPORTED,
    CORRUPTION_DETECTED
};

// Error context information
struct ErrorContext {
    ErrorContext() : severity(ErrorSeverity::ERROR), 
                     timestamp(std::chrono::system_clock::now()),
                     threadId(std::this_thread::get_id()),
                     stackDepth(0) {}
    
    std::string category;
    std::string message;
    ErrorCategory categoryType;
    ErrorCode errorCode;
    ErrorSeverity severity;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id threadId;
    std::string component;
    std::string operation;
    std::map<std::string, std::string> contextData;
    std::vector<std::string> stackTrace;
    int stackDepth;
    std::string file;
    int line;
    std::string function;
};

// Error recovery actions
enum class RecoveryAction {
    RETRY,
    RETRY_WITH_DELAY,
    RETRY_WITH_FALLBACK,
    MODIFY_PARAMETERS,
    SKIP_OPERATION,
    REINITIALIZE,
    RELOAD_STATE,
    SHUTDOWN,
    ABORT,
    RESTART,
    NONE
};

// Error recovery strategy
struct RecoveryStrategy {
    ErrorCode errorCode;
    RecoveryAction action;
    int maxRetries;
    double retryDelayMs;
    std::function<bool(const ErrorContext&)> shouldRetry;
    std::function<void(ErrorContext&)> onRecovery;
    std::vector<ErrorCode> dependentErrors;
};

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    
    // Handle an error
    virtual void handleError(const ErrorContext& error) = 0;
    
    // Handle a warning
    virtual void handleWarning(const ErrorContext& warning) = 0;
    
    // Get the last error
    virtual ErrorContext getLastError() const = 0;
    
    // Get all errors
    virtual std::vector<ErrorContext> getAllErrors() const = 0;
    
    // Clear error history
    virtual void clearErrors() = 0;
    
    // Check if system is in error state
    virtual bool isErrorState() const = 0;
    
    // Get error count
    virtual size_t getErrorCount() const = 0;
};

// Exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR)
        : std::runtime_error(message), errorCode(code), timestamp(std::chrono::system_clock::now()) {}
    
    NLMException(const ErrorContext& context)
        : std::runtime_error(context.message), errorCode(context.errorCode), 
          timestamp(context.timestamp), contextData(context.contextData) {}
    
    ErrorCode getErrorCode() const { return errorCode; }
    const std::chrono::system_clock::time_point& getTimestamp() const { return timestamp; }
    const std::map<std::string, std::string>& getContextData() const { return contextData; }
    
private:
    ErrorCode errorCode;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> contextData;
};

// Default error handler implementation
class DefaultErrorHandler : public ErrorHandler {
public:
    DefaultErrorHandler() : errorCount(0), inErrorState(false) {}
    
    void handleError(const ErrorContext& error) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        errorCount++;
        if (error.severity >= ErrorSeverity::ERROR) {
            inErrorState = true;
        }
        
        errors.push_back(error);
        
        // Log the error
        logError(error);
        
        // Apply recovery if needed
        applyRecoveryStrategy(error);
    }
    
    void handleWarning(const ErrorContext& warning) override {
        std::lock_guard<std::mutex> lock(mutex);
        
        errors.push_back(warning);
        
        // Log the warning
        logWarning(warning);
    }
    
    ErrorContext getLastError() const override {
        std::lock_guard<std::mutex> lock(mutex);
        if (!errors.empty()) {
            return errors.back();
        }
        return ErrorContext();
    }
    
    std::vector<ErrorContext> getAllErrors() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return errors;
    }
    
    void clearErrors() override {
        std::lock_guard<std::mutex> lock(mutex);
        errors.clear();
        inErrorState = false;
        errorCount = 0;
    }
    
    bool isErrorState() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return inErrorState;
    }
    
    size_t getErrorCount() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return errorCount;
    }
    
    // Add recovery strategy
    void addRecoveryStrategy(const RecoveryStrategy& strategy) {
        std::lock_guard<std::mutex> lock(mutex);
        recoveryStrategies[strategy.errorCode] = strategy;
    }
    
    // Get recovery strategy
    RecoveryStrategy getRecoveryStrategy(ErrorCode code) const {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = recoveryStrategies.find(code);
        if (it != recoveryStrategies.end()) {
            return it->second;
        }
        return RecoveryStrategy{code, RecoveryAction::NONE, 0, 0.0, nullptr, nullptr, {}};
    }
    
protected:
    virtual void logError(const ErrorContext& error) {
        std::cout << "[ERROR] " << error.category << ": " << error.message << std::endl;
        if (error.severity >= ErrorSeverity::CRITICAL) {
            std::cerr << "[CRITICAL] " << error.category << ": " << error.message << std::endl;
        }
    }
    
    virtual void logWarning(const ErrorContext& warning) {
        std::cout << "[WARNING] " << warning.category << ": " << warning.message << std::endl;
    }
    
    virtual void applyRecoveryStrategy(const ErrorContext& error) {
        RecoveryStrategy strategy = getRecoveryStrategy(error.errorCode);
        
        if (strategy.action == RecoveryAction::NONE) {
            return;
        }
        
        switch (strategy.action) {
            case RecoveryAction::RETRY:
                // Implement retry logic
                break;
            case RecoveryAction::RETRY_WITH_DELAY:
                // Implement retry with delay
                break;
            case RecoveryAction::MODIFY_PARAMETERS:
                // Implement parameter modification
                break;
            case RecoveryAction::SKIP_OPERATION:
                // Implement operation skipping
                break;
            default:
                break;
        }
    }
    
private:
    mutable std::mutex mutex;
    std::vector<ErrorContext> errors;
    size_t errorCount;
    bool inErrorState;
    std::map<ErrorCode, RecoveryStrategy> recoveryStrategies;
};

// Error reporting and analysis
class ErrorReporter {
public:
    ErrorReporter(std::shared_ptr<ErrorHandler> handler)
        : handler(handler) {}
    
    // Generate error report
    std::string generateReport() const {
        if (!handler) return "No error handler available";
        
        std::stringstream ss;
        ss << "=== NLM Error Report ===" << std::endl;
        ss << "Total Errors: " << handler->getErrorCount() << std::endl;
        ss << "Error State: " << (handler->isErrorState() ? "YES" : "NO") << std::endl;
        ss << std::endl;
        
        auto errors = handler->getAllErrors();
        for (const auto& error : errors) {
            ss << "--- Error ---" << std::endl;
            ss << "Category: " << error.category << std::endl;
            ss << "Message: " << error.message << std::endl;
            ss << "Severity: " << static_cast<int>(error.severity) << std::endl;
            ss << "Component: " << error.component << std::endl;
            ss << "Operation: " << error.operation << std::endl;
            ss << "Timestamp: " << formatTimestamp(error.timestamp) << std::endl;
            ss << "Thread: " << error.threadId << std::endl;
            ss << std::endl;
        }
        
        ss << "=== End Report ===" << std::endl;
        return ss.str();
    }
    
    // Save error report to file
    bool saveReport(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
        file.close();
        return true;
    }
    
    // Get error statistics
    struct ErrorStatistics {
        int totalErrors;
        int criticalErrors;
        int errorByCategory[20]; // 20 error categories
        double errorRate;
        std::chrono::system_clock::time_point firstError;
        std::chrono::system_clock::time_point lastError;
    };
    
    ErrorStatistics getStatistics() const {
        ErrorStatistics stats;
        stats.totalErrors = handler->getErrorCount();
        stats.criticalErrors = 0;
        std::fill(std::begin(stats.errorByCategory), std::end(stats.errorByCategory), 0);
        stats.errorRate = 0.0;
        
        auto errors = handler->getAllErrors();
        for (const auto& error : errors) {
            if (error.severity >= ErrorSeverity::CRITICAL) {
                stats.criticalErrors++;
            }
            // Increment error category count
            // This would need mapping from ErrorCategory to index
        }
        
        if (stats.totalErrors > 0) {
            stats.errorRate = static_cast<double>(stats.criticalErrors) / stats.totalErrors;
        }
        
        return stats;
    }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::shared_ptr<ErrorHandler> handler;
};

// Error context builder utility
class ErrorContextBuilder {
public:
    ErrorContextBuilder& setCategory(const std::string& category) {
        context.category = category;
        return *this;
    }
    
    ErrorContextBuilder& setMessage(const std::string& message) {
        context.message = message;
        return *this;
    }
    
    ErrorContextBuilder& setCategoryType(ErrorCategory category) {
        context.categoryType = category;
        return *this;
    }
    
    ErrorContextBuilder& setErrorCode(ErrorCode code) {
        context.errorCode = code;
        return *this;
    }
    
    ErrorContextBuilder& setSeverity(ErrorSeverity severity) {
        context.severity = severity;
        return *this;
    }
    
    ErrorContextBuilder& setComponent(const std::string& component) {
        context.component = component;
        return *this;
    }
    
    ErrorContextBuilder& setOperation(const std::string& operation) {
        context.operation = operation;
        return *this;
    }
    
    ErrorContextBuilder& addContextData(const std::string& key, const std::string& value) {
        context.contextData[key] = value;
        return *this;
    }
    
    ErrorContextBuilder& addStackTrace(const std::vector<std::string>& trace) {
        context.stackTrace = trace;
        return *this;
    }
    
    ErrorContextBuilder& setLocation(const char* file, int line, const char* function) {
        context.file = file;
        context.line = line;
        context.function = function;
        return *this;
    }
    
    ErrorContext build() {
        return context;
    }
    
private:
    ErrorContext context;
};

// Global error handler instance
extern std::shared_ptr<ErrorHandler> gErrorHandler;

// Global error reporter instance
extern std::shared_ptr<ErrorReporter> gErrorReporter;

// Error handling macros
#define NLM_HANDLE_ERROR(category, message, severity) \
    do { \
        ErrorContextBuilder builder; \
        builder.setCategory(category) \
              .setMessage(message) \
              .setSeverity(severity) \
              .setComponent(#category) \
              .setLocation(__FILE__, __LINE__, __func__); \
        ErrorContext error = builder.build(); \
        if (gErrorHandler) { \
            gErrorHandler->handleError(error); \
        } else { \
            throw NLMException(error.message, error.errorCode); \
        } \
    } while(0)

#define NLM_HANDLE_WARNING(category, message) \
    do { \
        ErrorContextBuilder builder; \
        builder.setCategory(category) \
              .setMessage(message) \
              .setSeverity(ErrorSeverity::WARNING) \
              .setComponent(#category); \
        ErrorContext warning = builder.build(); \
        if (gErrorHandler) { \
            gErrorHandler->handleWarning(warning); \
        } \
    } while(0)

#define NLM_THROW_ERROR(category, message, code, severity) \
    do { \
        ErrorContextBuilder builder; \
        builder.setCategory(category) \
              .setMessage(message) \
              .setErrorCode(code) \
              .setSeverity(severity) \
              .setComponent(#category) \
              .setLocation(__FILE__, __LINE__, __func__); \
        ErrorContext error = builder.build(); \
        throw NLMException(error); \
    } while(0)

// Exception throwing macros
#define NLM_THROW(category, message, code) NLM_THROW_ERROR(category, message, code, ErrorSeverity::ERROR)
#define NLM_THROW_CRITICAL(category, message, code) NLM_THROW_ERROR(category, message, code, ErrorSeverity::CRITICAL)
#define NLM_THROW_FATAL(category, message, code) NLM_THROW_ERROR(category, message, code, ErrorSeverity::FATAL)

// Exception catching macro
#define NLM_TRY_CATCH(operation, errorCategory) \
    try { \
        operation; \
    } catch (const NLMException& e) { \
        NLM_THROW_ERROR(errorCategory, std::string("Operation failed: ") + e.what(), \
                       e.getErrorCode(), ErrorSeverity::ERROR); \
    } catch (const std::exception& e) { \
        NLM_THROW_ERROR(errorCategory, std::string("Standard exception: ") + e.what(), \
                       ErrorCode::UNKNOWN_ERROR, ErrorSeverity::ERROR); \
    } catch (...) { \
        NLM_THROW_ERROR(errorCategory, "Unknown exception occurred", \
                       ErrorCode::UNKNOWN_ERROR, ErrorSeverity::CRITICAL); \
    }

} // namespace nlm

#endif // ERROR_HANDLING_HPP

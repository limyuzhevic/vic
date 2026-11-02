// Error Handling and Validation Implementation
// Contains implementations for error handling and validation classes

#include "ErrorHandling.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Error Reporter implementation
void ErrorReporter::report(ErrorCode code, const std::string& category, const std::string& message,
                         const std::string& context, bool isRecoverable) {
    ErrorReport report;
    report.timestamp = std::chrono::system_clock::now();
    report.code = code;
    report.category = category;
    report.message = message;
    report.context = context;
    report.isRecoverable = isRecoverable;
    
    errorLog_.push_back(report);
    
    // Log to console with formatting
    std::cerr << "[ERROR] " << getErrorCodeString(code) << ": " 
              << category << ": " << message << std::endl;
    if (!context.empty()) {
        std::cerr << "  Context: " << context << std::endl;
    }
}

std::string ErrorReporter::getErrorCodeString(ErrorCode code) {
    const std::unordered_map<ErrorCode, std::string> codeStrings = {
        {ErrorCode::Success, "SUCCESS"},
        {ErrorCode::Generic, "GENERIC"},
        {ErrorCode::ConfigError, "CONFIG"},
        {ErrorCode::BrainError, "BRAIN"},
        {ErrorCode::MemoryError, "MEMORY"},
        {ErrorCode::ValidationError, "VALIDATION"},
        {ErrorCode::FileError, "FILE"},
        {ErrorCode::NetworkError, "NETWORK"},
        {ErrorCode::PlasticityError, "PLASTICITY"},
        {ErrorCode::NeuromodulationError, "NEUROMODULATION"},
        {ErrorCode::DevelopmentError, "DEVELOPMENT"},
    };
    
    auto it = codeStrings.find(code);
    return it != codeStrings.end() ? it->second : "UNKNOWN";
}

// Safe Execution implementation
template<typename Function, typename... Args>
auto SafeExecution::executeSafely(ErrorCode errorCode, const std::string& category,
                               const std::string& context, Function&& func, Args&&... args) 
-> decltype(func(args...)) {
    try {
        auto result = func(std::forward<Args>(args)...);
        ErrorReporter::getInstance().report(ErrorCode::Success, category,
                                          "Operation completed successfully", context, true);
        return result;
    } catch (const NLMException& e) {
        ErrorReporter::getInstance().report(errorCode, category, e.what(), context, true);
        throw;
    } catch (const std::exception& e) {
        ErrorReporter::getInstance().report(errorCode, category,
                                          std::string("Standard exception: ") + e.what(), context, false);
        throw;
    } catch (...) {
        ErrorReporter::getInstance().report(errorCode, category, "Unknown exception occurred", context, false);
        throw;
    }
}

template<typename Function, typename... Args>
auto SafeExecution::executeWithRetry(ErrorCode errorCode, const std::string& category,
                                   const std::string& context, Function&& func, Args&&... args,
                                   int maxRetries, std::chrono::milliseconds delay) {
    int attempt = 0;
    std::exception_ptr lastException;
    
    while (attempt < maxRetries) {
        try {
            auto result = func(std::forward<Args>(args)...);
            return result;
        } catch (...) {
            lastException = std::current_exception();
            attempt++;
            
            if (attempt < maxRetries) {
                std::this_thread::sleep_for(delay);
                delay *= 2; // Exponential backoff
            }
        }
    }
    
    if (lastException) {
        std::rethrow_exception(lastException);
    }
    
    throw NLMException("All retry attempts failed", errorCode);
}

template<typename Function, typename... Args>
auto SafeExecution::validateAndExecute(const Validator::ValidationResult& validation,
                                     ErrorCode errorCode, const std::string& category,
                                     const std::string& context, Function&& func, Args&&... args) {
    if (!validation.valid) {
        ErrorReporter::getInstance().report(errorCode, category,
                                          "Validation failed: " + validation.getSummary(),
                                          context, false);
        throw ValidationException("Validation failed: " + validation.getSummary());
    }
    
    if (!validation.warnings.empty()) {
        for (const auto& warning : validation.warnings) {
            ErrorReporter::getInstance().report(ErrorCode::ValidationError, category,
                                              "Warning: " + warning, context, true);
        }
    }
    
    return executeSafely(errorCode, category, context, func, std::forward<Args>(args)...);
}

// Recovery Manager implementation
void RecoveryManager::createRecoveryPoint(const std::string& description, std::function<void()> restoreFunction) {
    RecoveryPoint point(description, restoreFunction);
    recoveryPoints_.push_back(point);
    
    Debugger::getInstance().logDebug("Created recovery point: " + description);
}

bool RecoveryManager::restoreFromLastRecoveryPoint() {
    if (recoveryPoints_.empty()) {
        return false;
    }
    
    auto point = recoveryPoints_.back();
    recoveryPoints_.pop_back();
    
    try {
        point.restoreFunction();
        Debugger::getInstance().logDebug("Restored from recovery point: " + point.description);
        return true;
    } catch (const std::exception& e) {
        Debugger::getInstance().logDebug("Failed to restore from recovery point " + point.description + ": " + e.what());
        point.isValid = false;
        return false;
    }
}

// Global error handling integration
namespace {
    ErrorReporter& getGlobalErrorReporter() {
        return ErrorReporter::getInstance();
    }
    
    RecoveryManager& getGlobalRecoveryManager() {
        return RecoveryManager::getInstance();
    }
    
    Debugger& getGlobalDebugger() {
        return Debugger::getInstance();
    }
    
    Validator& getGlobalValidator() {
        static Validator validator;
        return validator;
    }
    
    SafeExecution& getGlobalSafeExecution() {
        static SafeExecution executor;
        return executor;
    }
}

// Convenience functions for common error handling scenarios
void reportBrainError(const std::string& message, const std::string& context = "") {
    getGlobalErrorReporter().report(ErrorCode::BrainError, "Brain", message, context, true);
}

void reportMemoryError(const std::string& message, const std::string& context = "") {
    getGlobalErrorReporter().report(ErrorCode::MemoryError, "Memory", message, context, true);
}

void reportConfigError(const std::string& message, const std::string& context = "") {
    getGlobalErrorReporter().report(ErrorCode::ConfigError, "Config", message, context, false);
}

void reportValidationError(const std::string& message, const std::string& context = "") {
    getGlobalErrorReporter().report(ErrorCode::ValidationError, "Validation", message, context, false);
}

// Safe execution convenience functions
template<typename Function, typename... Args>
auto executeBrainOperation(Function&& func, Args&&... args) {
    return getGlobalSafeExecution().executeSafely(ErrorCode::BrainError, "Brain Operation", 
                                                  "", std::forward<Function>(func), std::forward<Args>(args)...);
}

template<typename Function, typename... Args>
auto executeMemoryOperation(Function&& func, Args&&... args) {
    return getGlobalSafeExecution().executeSafely(ErrorCode::MemoryError, "Memory Operation",
                                                  "", std::forward<Function>(func), std::forward<Args>(args)...);
}

// Validation convenience functions
Validator::ValidationResult validateWithRecovery(const Validator::ValidationResult& validation,
                                               const std::string& operation) {
    if (!validation.valid) {
        RecoveryManager::getInstance().createRecoveryPoint(
            "Pre-validation state for " + operation,
            []() { /* Restore to pre-validation state */ }
        );
        throw ValidationException("Validation failed: " + validation.getSummary());
    }
    
    return validation;
}

// Error recovery helper
void attemptRecovery(const std::string& operation) {
    if (RecoveryManager::getInstance().restoreFromLastRecoveryPoint()) {
        Debugger::getInstance().logDebug("Successfully recovered from error in operation: " + operation);
    } else {
        Debugger::getInstance().logDebug("No recovery point available for operation: " + operation);
    }
}

} // namespace nlm

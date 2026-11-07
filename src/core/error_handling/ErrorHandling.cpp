// NLM Error Handling Implementation

#include "ErrorHandling.hpp"
#include "core/Logger/Logger.hpp"
#include <thread>
#include <fstream>
#include <algorithm>

namespace nlm {

// Global instances
std::shared_ptr<ErrorHandler> gErrorHandler;
std::shared_ptr<ErrorReporter> gErrorReporter;

// DefaultErrorHandler implementation
void DefaultErrorHandler::logError(const ErrorContext& error) {
    std::string timestamp = formatTimestamp(error.timestamp);
    std::string severity = getSeverityString(error.severity);
    
    std::cout << "[" << timestamp << "] [" << severity << "] [" << error.category << "] "
              << error.message << std::endl;
    
    if (error.severity >= ErrorSeverity::CRITICAL) {
        std::cerr << "[CRITICAL] " << error.category << ": " << error.message << std::endl;
    }
}

void DefaultErrorHandler::logWarning(const ErrorContext& warning) {
    std::string timestamp = formatTimestamp(warning.timestamp);
    std::string severity = getSeverityString(warning.severity);
    
    std::cout << "[" << timestamp << "] [" << severity << "] [" << warning.category << "] "
              << warning.message << std::endl;
}

std::string DefaultErrorHandler::getSeverityString(ErrorSeverity severity) const {
    switch (severity) {
        case ErrorSeverity::DEBUG: return "DEBUG";
        case ErrorSeverity::INFO: return "INFO";
        case ErrorSeverity::WARNING: return "WARNING";
        case ErrorSeverity::ERROR: return "ERROR";
        case ErrorSeverity::CRITICAL: return "CRITICAL";
        case ErrorSeverity::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

std::string DefaultErrorHandler::formatTimestamp(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Initialize global instances
void initializeErrorSystem() {
    if (!gErrorHandler) {
        gErrorHandler = std::make_shared<DefaultErrorHandler>();
    }
    if (!gErrorReporter) {
        gErrorReporter = std::make_shared<ErrorReporter>(gErrorHandler);
    }
    
    // Set up default recovery strategies
    DefaultErrorHandler* defaultHandler = std::dynamic_pointer_cast<DefaultErrorHandler>(gErrorHandler).get();
    if (defaultHandler) {
        defaultHandler->setupDefaultRecoveryStrategies();
    }
}

void shutdownErrorSystem() {
    // Cleanup
    gErrorHandler.reset();
    gErrorReporter.reset();
}

void setErrorHandler(std::shared_ptr<ErrorHandler> handler) {
    gErrorHandler = handler;
    gErrorReporter = std::make_shared<ErrorReporter>(handler);
}

// Error context factory
ErrorContext createErrorContext(ErrorCategory categoryType, const std::string& category, 
                               const std::string& message, ErrorCode errorCode,
                               ErrorSeverity severity, const std::string& component,
                               const std::string& operation) {
    ErrorContext context;
    context.category = category;
    context.message = message;
    context.categoryType = categoryType;
    context.errorCode = errorCode;
    context.severity = severity;
    context.component = component;
    context.operation = operation;
    context.timestamp = std::chrono::system_clock::now();
    context.threadId = std::this_thread::get_id();
    
    return context;
}

// Recovery strategy setup
void DefaultErrorHandler::setupDefaultRecoveryStrategies() {
    // Memory error recovery
    addRecoveryStrategy({
        ErrorCode::MEM_NULL_POINTER,
        RecoveryAction::RETRY_WITH_DELAY,
        3,
        1000.0,
        [](const ErrorContext& error) { return true; },
        [](ErrorContext& error) {
            // Retry memory allocation with different parameters
            error.contextData["retry_count"] = std::to_string(std::stoi(error.contextData["retry_count"] or "0") + 1);
        },
        {ErrorCode::MEM_ALLOCATION_FAILED}
    });
    
    // Neural error recovery
    addRecoveryStrategy({
        ErrorCode::NEURAL_NULL_POINTER,
        RecoveryAction::REINITIALIZE,
        1,
        0.0,
        [](const ErrorContext& error) { return true; },
        [](ErrorContext& error) {
            // Reinitialize neural system
            error.contextData["reinitialized"] = "true";
        },
        {}
    });
    
    // Plasticity error recovery
    addRecoveryStrategy({
        ErrorCode::PLASTICITY_STDP_FAILED,
        RecoveryAction::MODIFY_PARAMETERS,
        2,
        500.0,
        [](const ErrorContext& error) { return true; },
        [](ErrorContext& error) {
            // Adjust STDP parameters
            error.contextData["learning_rate_modified"] = "true";
        },
        {}
    });
    
    // Configuration error recovery
    addRecoveryStrategy({
        ErrorCode::CONFIG_INVALID_VALUE,
        RecoveryAction::SKIP_OPERATION,
        0,
        0.0,
        [](const ErrorContext& error) { return false; }, // Don't retry
        [](ErrorContext& error) {
            // Log configuration issue
            error.contextData["action_taken"] = "skip_with_warning";
        },
        {}
    });
}

// Error utilities
std::string getErrorCategoryString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::MEMORY_ERROR: return "MEMORY";
        case ErrorCategory::NEURAL_ERROR: return "NEURAL";
        case ErrorCategory::PLASTICITY_ERROR: return "PLASTICITY";
        case ErrorCategory::COGNITION_ERROR: return "COGNITION";
        case ErrorCategory::NEUROMODULATION_ERROR: return "NEUROMODULATION";
        case ErrorCategory::DEVELOPMENT_ERROR: return "DEVELOPMENT";
        case ErrorCategory::PERSISTENCE_ERROR: return "PERSISTENCE";
        case ErrorCategory::INTEGRATION_ERROR: return "INTEGRATION";
        case ErrorCategory::CONFIGURATION_ERROR: return "CONFIGURATION";
        case ErrorCategory::SYSTEM_ERROR: return "SYSTEM";
        case ErrorCategory::VALIDATION_ERROR: return "VALIDATION";
        case ErrorCategory::RESOURCE_ERROR: return "RESOURCE";
        case ErrorCategory::UNKNOWN_ERROR: return "UNKNOWN";
        default: return "UNSPECIFIED";
    }
}

std::string getErrorCodeString(ErrorCode code) {
    switch (code) {
        case ErrorCode::MEM_NULL_POINTER: return "MEM_NULL_POINTER";
        case ErrorCode::MEM_ALLOCATION_FAILED: return "MEM_ALLOCATION_FAILED";
        case ErrorCode::MEM_CORRUPTION_DETECTED: return "MEM_CORRUPTION_DETECTED";
        case ErrorCode::MEM_OUT_OF_MEMORY: return "MEM_OUT_OF_MEMORY";
        case ErrorCode::MEM_ACCESS_VIOLATION: return "MEM_ACCESS_VIOLATION";
        case ErrorCode::NEURAL_NULL_POINTER: return "NEURAL_NULL_POINTER";
        case ErrorCode::NEURAL_DYNAMICS_FAILED: return "NEURAL_DYNAMICS_FAILED";
        case ErrorCode::NEURAL_SYNAPSIS_FAILED: return "NEURAL_SYNAPSIS_FAILED";
        case ErrorCode::NEURAL_SPIKE_FAILED: return "NEURAL_SPIKE_FAILED";
        case ErrorCode::PLASTICITY_STDP_FAILED: return "PLASTICITY_STDP_FAILED";
        case ErrorCode::PLASTICITY_HEBBIAN_FAILED: return "PLASTICITY_HEBBIAN_FAILED";
        case ErrorCode::PLASTICITY_STRUCTURAL_FAILED: return "PLASTICITY_STRUCTURAL_FAILED";
        case ErrorCode::PLASTICITY_ELIGIBILITY_FAILED: return "PLASTICITY_ELIGIBILITY_FAILED";
        case ErrorCode::COGNITION_PLANNER_FAILED: return "COGNITION_PLANNER_FAILED";
        case ErrorCode::COGNITION_CONCEPT_FAILED: return "COGNITION_CONCEPT_FAILED";
        case ErrorCode::COGNITION_ATTENTION_FAILED: return "COGNITION_ATTENTION_FAILED";
        case ErrorCode::COGNITION_SELF_MODEL_FAILED: return "COGNITION_SELF_MODEL_FAILED";
        case ErrorCode::NEUROMOD_DOPAMINE_FAILED: return "NEUROMOD_DOPAMINE_FAILED";
        case ErrorCode::NEUROMOD_NOVELTY_FAILED: return "NEUROMOD_NOVELTY_FAILED";
        case ErrorCode::NEUROMOD_CURIOUSY_FAILED: return "NEUROMOD_CURIOUSY_FAILED";
        case ErrorCode::NEUROMOD_PREDICTION_ERROR_FAILED: return "NEUROMOD_PREDICTION_ERROR_FAILED";
        case ErrorCode::DEVELOPMENT_MATURATION_FAILED: return "DEVELOPMENT_MATURATION_FAILED";
        case ErrorCode::DEVELOPMENT_PRUNING_FAILED: return "DEVELOPMENT_PRUNING_FAILED";
        case ErrorCode::DEVELOPMENT_SYNAPTOGENESIS_FAILED: return "DEVELOPMENT_SYNAPTOGENESIS_FAILED";
        case ErrorCode::PERSISTENCE_SAVE_FAILED: return "PERSISTENCE_SAVE_FAILED";
        case ErrorCode::PERSISTENCE_LOAD_FAILED: return "PERSISTENCE_LOAD_FAILED";
        case ErrorCode::PERSISTENCE_CORRUPTION_DETECTED: return "PERSISTENCE_CORRUPTION_DETECTED";
        case ErrorCode::PERSISTENCE_VERSION_MISMATCH: return "PERSISTENCE_VERSION_MISMATCH";
        case ErrorCode::INTEGRATION_FAILED: return "INTEGRATION_FAILED";
        case ErrorCode::INTEGRATION_TIMEOUT: return "INTEGRATION_TIMEOUT";
        case ErrorCode::INTEGRATION_PARTIAL: return "INTEGRATION_PARTIAL";
        case ErrorCode::INTEGRATION_NOT_READY: return "INTEGRATION_NOT_READY";
        case ErrorCode::CONFIG_INVALID_VALUE: return "CONFIG_INVALID_VALUE";
        case ErrorCode::CONFIG_MISSING_KEY: return "CONFIG_MISSING_KEY";
        case ErrorCode::CONFIG_TYPE_MISMATCH: return "CONFIG_TYPE_MISMATCH";
        case ErrorCode::CONFIG_VALIDATION_FAILED: return "CONFIG_VALIDATION_FAILED";
        case ErrorCode::RESOURCE_NOT_FOUND: return "RESOURCE_NOT_FOUND";
        case ErrorCode::RESOURCE_BUSY: return "RESOURCE_BUSY";
        case ErrorCode::RESOURCE_LOCKED: return "RESOURCE_LOCKED";
        case ErrorCode::RESOURCE_TIMEOUT: return "RESOURCE_TIMEOUT";
        case ErrorCode::SYSTEM_INITIALIZATION_FAILED: return "SYSTEM_INITIALIZATION_FAILED";
        case ErrorCode::SYSTEM_SHUTDOWN_FAILED: return "SYSTEM_SHUTDOWN_FAILED";
        case ErrorCode::SYSTEM_INVALID_STATE: return "SYSTEM_INVALID_STATE";
        case ErrorCode::SYSTEM_CORRUPTION: return "SYSTEM_CORRUPTION";
        case ErrorCode::VALIDATION_FAILED: return "VALIDATION_FAILED";
        case ErrorCode::VALIDATION_TIMEOUT: return "VALIDATION_TIMEOUT";
        case ErrorCode::VALIDATION_INCOMPLETE: return "VALIDATION_INCOMPLETE";
        case ErrorCode::VALIDATION_CONFLICT: return "VALIDATION_CONFLICT";
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        case ErrorCode::OUT_OF_RANGE: return "OUT_OF_RANGE";
        case ErrorCode::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case ErrorCode::NOT_SUPPORTED: return "NOT_SUPPORTED";
        case ErrorCode::CORRUPTION_DETECTED: return "CORRUPTION_DETECTED";
        default: return "UNSPECIFIED_ERROR";
    }
}

// Global functions for easy access
void handleError(const std::string& category, const std::string& message, 
                 ErrorCode errorCode, ErrorSeverity severity, 
                 const std::string& component) {
    if (gErrorHandler) {
        ErrorContext context = createErrorContext(
            ErrorCategory::UNKNOWN_ERROR, // Category type would need mapping
            category, message, errorCode, severity, component, "generic_operation"
        );
        gErrorHandler->handleError(context);
    }
}

void handleWarning(const std::string& category, const std::string& message,
                   const std::string& component) {
    if (gErrorHandler) {
        ErrorContext context = createErrorContext(
            ErrorCategory::UNKNOWN_ERROR,
            category, message, ErrorCode::UNKNOWN_ERROR, ErrorSeverity::WARNING,
            component, "generic_operation"
        );
        gErrorHandler->handleWarning(context);
    }
}

void throwError(const std::string& category, const std::string& message,
                ErrorCode errorCode, ErrorSeverity severity,
                const std::string& component) {
    ErrorContext context = createErrorContext(
        ErrorCategory::UNKNOWN_ERROR,
        category, message, errorCode, severity, component, "generic_operation"
    );
    throw NLMException(context);
}

// Initialize and shutdown functions
void initializeErrorSystem() {
    if (!gErrorHandler) {
        gErrorHandler = std::make_shared<DefaultErrorHandler>();
    }
    if (!gErrorReporter) {
        gErrorReporter = std::make_shared<ErrorReporter>(gErrorHandler);
    }
}

void shutdownErrorSystem() {
    if (gErrorHandler) {
        gErrorHandler->clearErrors();
    }
    gErrorHandler.reset();
    gErrorReporter.reset();
}

} // namespace nlm

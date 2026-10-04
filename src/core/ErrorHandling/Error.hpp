#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <stdexcept>

namespace nlm {

// Error categories for better organization
enum class ErrorCategory {
    Configuration,
    Validation,
    Memory,
    Neural,
    I_O,
    System,
    Neuromodulation,
    Development,
    Sensory,
    Motor,
    Unknown
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

// Base exception class with rich error information
class NLMError : public std::exception {
protected:
    std::string message_;
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string file_;
    int line_;
    std::string function_;
    std::string stacktrace_;
    std::map<std::string, std::string> context_;
    std::vector<std::string> recoverySuggestions_;
    int errorCode_;
    std::string timestamp_;

public:
    NLMError(const std::string& message,
             ErrorCategory category = ErrorCategory::Unknown,
             ErrorSeverity severity = ErrorSeverity::Error,
             const std::string& file = "",
             int line = 0,
             const std::string& function = "",
             int errorCode = 1)
        : message_(message), category_(category), severity_(severity),
          file_(file), line_(line), function_(function),
          errorCode_(errorCode), timestamp_("") {
    }

    virtual ~NLMError() noexcept = default;

    // Get formatted error message
    virtual const char* what() const noexcept override {
        return message_.c_str();
    }

    // Get error category
    ErrorCategory getCategory() const { return category_; }

    // Get error severity
    ErrorSeverity getSeverity() const { return severity_; }

    // Get error code
    int getErrorCode() const { return errorCode_; }

    // Get file location
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }

    // Get context information
    const std::string& getContext(const std::string& key) const {
        auto it = context_.find(key);
        static const std::string empty = "";
        return it != context_.end() ? it->second : empty;
    }

    // Add context information
    void addContext(const std::string& key, const std::string& value) {
        context_[key] = value;
    }

    // Get formatted message with context
    virtual std::string getFormattedMessage() const {
        std::string formatted = "[" + errorCategoryToString(category_) + "] " +
                                "[" + errorSeverityToString(severity_) + "] " +
                                "[Code " + std::to_string(errorCode_) + "] " +
                                message_;

        if (!file_.empty()) {
            formatted += "\n  File: " + file_;
            if (line_ > 0) {
                formatted += ":" + std::to_string(line_);
            }
            if (!function_.empty()) {
                formatted += " in " + function_;
            }
        }

        for (const auto& [key, value] : context_) {
            formatted += "\n  " + key + ": " + value;
        }

        if (!recoverySuggestions_.empty()) {
            formatted += "\n\nRecovery Suggestions:";
            for (const auto& suggestion : recoverySuggestions_) {
                formatted += "\n  - " + suggestion;
            }
        }

        return formatted;
    }

    // Add recovery suggestion
    void addRecoverySuggestion(const std::string& suggestion) {
        recoverySuggestions_.push_back(suggestion);
    }

    // Get all recovery suggestions
    const std::vector<std::string>& getRecoverySuggestions() const {
        return recoverySuggestions_;
    }

    // Convert category to string
    static std::string errorCategoryToString(ErrorCategory category) {
        static const std::map<ErrorCategory, std::string> categoryMap = {
            {ErrorCategory::Configuration, "CONFIGURATION"},
            {ErrorCategory::Validation, "VALIDATION"},
            {ErrorCategory::Memory, "MEMORY"},
            {ErrorCategory::Neural, "NEURAL"},
            {ErrorCategory::I_O, "I/O"},
            {ErrorCategory::System, "SYSTEM"},
            {ErrorCategory::Neuromodulation, "NEUROMODULATION"},
            {ErrorCategory::Development, "DEVELOPMENT"},
            {ErrorCategory::Sensory, "SENSORY"},
            {ErrorCategory::Motor, "MOTOR"},
            {ErrorCategory::Unknown, "UNKNOWN"}
        };
        auto it = categoryMap.find(category);
        return it != categoryMap.end() ? it->second : "UNKNOWN";
    }

    // Convert severity to string
    static std::string errorSeverityToString(ErrorSeverity severity) {
        static const std::map<ErrorSeverity, std::string> severityMap = {
            {ErrorSeverity::Debug, "DEBUG"},
            {ErrorSeverity::Info, "INFO"},
            {ErrorSeverity::Warning, "WARNING"},
            {ErrorSeverity::Error, "ERROR"},
            {ErrorSeverity::Critical, "CRITICAL"},
            {ErrorSeverity::Fatal, "FATAL"}
        };
        auto it = severityMap.find(severity);
        return it != severityMap.end() ? it->second : "ERROR";
    }
};

// Specific error types for better error handling
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message,
                      ErrorSeverity severity = ErrorSeverity::Error)
        : NLMError(message, ErrorCategory::Configuration, severity) {}
};

class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMError(message, ErrorCategory::Validation, severity) {}
};

class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               ErrorSeverity severity = ErrorSeverity::Critical)
        : NLMError(message, ErrorCategory::Memory, severity) {}
};

class NeuralError : public NLMError {
public:
    NeuralError(const std::string& message,
               ErrorSeverity severity = ErrorSeverity::Error)
        : NLMError(message, ErrorCategory::Neural, severity) {}
};

class IOError : public NLMError {
public:
    IOError(const std::string& message,
            ErrorSeverity severity = ErrorSeverity::Error)
        : NLMError(message, ErrorCategory::I_O, severity) {}
};

class NeuromodulationError : public NLMError {
public:
    NeuromodulationError(const std::string& message,
                        ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMError(message, ErrorCategory::Neuromodulation, severity) {}
};

class DevelopmentError : public NLMError {
public:
    DevelopmentError(const std::string& message,
                    ErrorSeverity severity = ErrorSeverity::Info)
        : NLMError(message, ErrorCategory::Development, severity) {}
};

class SensoryError : public NLMError {
public:
    SensoryError(const std::string& message,
                ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMError(message, ErrorCategory::Sensory, severity) {}
};

class MotorError : public NLMError {
public:
    MotorError(const std::string& message,
              ErrorSeverity severity = ErrorSeverity::Warning)
        : NLMError(message, ErrorCategory::Motor, severity) {}
};

// Error recovery strategies
enum class ErrorRecoveryStrategy {
    None,
    Retry,
    Fallback,
    GracefulDegradation,
    AutoRecovery,
    UserIntervention
};

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::string details;
    double performanceImpact;  // 0.0 to 1.0, where 1.0 is worst impact

    ValidationResult() : isValid(true), performanceImpact(0.0) {}

    ValidationResult(const std::vector<std::string>& errors,
                    const std::vector<std::string>& warnings = std::vector<std::string>(),
                    const std::string& details = "")
        : isValid(false), errors(errors), warnings(warnings), details(details), performanceImpact(1.0) {}
};

// Exception factory functions
inline void NLM_THROW(const std::string& message,
                     ErrorCategory category = ErrorCategory::Unknown,
                     ErrorSeverity severity = ErrorSeverity::Error,
                     const std::string& file = "",
                     int line = 0,
                     const std::string& function = "",
                     int errorCode = 1) {
    throw NLMError(message, category, severity, file, line, function, errorCode);
}

inline void NLM_THROW_CONFIG(const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Error) {
    throw ConfigurationError(message, severity);
}

inline void NLM_THROW_VALIDATION(const std::string& message,
                                ErrorSeverity severity = ErrorSeverity::Warning) {
    throw ValidationError(message, severity);
}

inline void NLM_THROW_MEMORY(const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Critical) {
    throw MemoryError(message, severity);
}

inline void NLM_THROW_NEURAL(const std::string& message,
                            ErrorSeverity severity = ErrorSeverity::Error) {
    throw NeuralError(message, severity);
}

inline void NLM_THROW_IO(const std::string& message,
                        ErrorSeverity severity = ErrorSeverity::Error) {
    throw IOError(message, severity);
}

// Macro for throwing errors with context
#define NLM_THROW_ERROR(category, severity, code, message) \
    nlm::NLMError(__FILE__, __LINE__, __func__, category, severity, code, message)

// Helper function for safe operations
template<typename T>
T NLM_SAFE_OP(const std::string& operation, std::function<T()> func,
              ErrorCategory category = ErrorCategory::Unknown,
              ErrorRecoveryStrategy recovery = ErrorRecoveryStrategy::None) {
    try {
        return func();
    } catch (const NLMError& e) {
        // Re-throw with additional context
        std::string enhancedMessage = "Operation \"" + operation + \"\" failed: " + std::string(e.what());
        NLM_THROW(enhancedMessage, category, ErrorSeverity::Error);
    }
}

// Error recovery mechanism
class ErrorRecoveryManager {
public:
    ErrorRecoveryManager() {}

    // Attempt recovery with specified strategy
    static bool attemptRecovery(const NLMError& error, ErrorRecoveryStrategy strategy) {
        switch (strategy) {
            case ErrorRecoveryStrategy::None:
                return false;
            case ErrorRecoveryStrategy::Retry:
                return retryOperation(error);
            case ErrorRecoveryStrategy::Fallback:\n                return fallbackOperation(error);
            case ErrorRecoveryStrategy::GracefulDegradation:
                return gracefulDegradation(error);
            case ErrorRecoveryStrategy::AutoRecovery:
                return autoRecovery(error);
            case ErrorRecoveryStrategy::UserIntervention:
                return userInterventionRequired(error);
            default:
                return false;
        }
    }

private:
    static bool retryOperation(const NLMError& error) {
        // Implement retry logic
        return false;
    }

    static bool fallbackOperation(const NLMError& error) {
        // Implement fallback logic
        return false;
    }

    static bool gracefulDegradation(const NLMError& error) {
        // Implement graceful degradation logic
        return false;
    }

    static bool autoRecovery(const NLMError& error) {
        // Implement auto-recovery logic
        return false;
    }

    static bool userInterventionRequired(const NLMError& error) {
        // Implement user intervention logic
        return true;
    }
};

} // namespace nlm
#include "ErrorHandling.hpp"
#include "Logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

// NLMError implementation
NLMError::NLMError(const std::string& message, ErrorSeverity severity,
                   ErrorCategory category, const std::string& file, int line,
                   const std::string& function)
    : std::runtime_error(message), severity_(severity), category_(category),
      file_(file), line_(line), function_(function) {
    message_ = message;
    
    // Generate timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    timestamp_ = ss.str();
}

std::string NLMError::getReport() const {
    std::stringstream ss;
    
    ss << "=== NLM Error Report ===\n";
    ss << "Severity: " << static_cast<int>(severity_) << " (";
    switch (severity_) {
        case ErrorSeverity::Debug: ss << "Debug"; break;
        case ErrorSeverity::Info: ss << "Info"; break;
        case ErrorSeverity::Warning: ss << "Warning"; break;
        case ErrorSeverity::Error: ss << "Error"; break;
        case ErrorSeverity::Critical: ss << "Critical"; break;
        case ErrorSeverity::Fatal: ss << "Fatal"; break;
    }
    ss << ")\n";
    
    ss << "Category: " << static_cast<int>(category_) << "\n";
    
    ss << "Message: " << message_ << "\n";
    
    if (!file_.empty()) {
        ss << "Location: " << file_;
        if (line_ > 0) ss << ":" << line_;
        ss << " in " << function_ << "\n";
    }
    
    ss << "Timestamp: " << timestamp_ << "\n";
    
    if (!context_.empty()) {
        ss << "Context: " << context_ << "\n";
    }
    
    if (!cause_.empty()) {
        ss << "Cause: " << cause_ << "\n";
    }
    
    if (!stackTrace_.empty()) {
        ss << "Stack Trace:\n" << stackTrace_ << "\n";
    }
    
    if (!details_.empty()) {
        ss << "Details:\n";
        for (const auto& pair : details_) {
            ss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return ss.str();
}

// ParameterValidator implementation
ValidationResult ParameterValidator::validateRange(double value, double min, double max, const std::string& name) {
    if (value < min || value > max) {
        std::stringstream ss;
        ss << "Value " << name << " = " << value << " is out of range [" << min << ", " << max << "]";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validatePositive(double value, const std::string& name) {
    if (value <= 0.0) {
        std::stringstream ss;
        ss << "Value " << name << " = " << value << " must be positive";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateNonNegative(double value, const std::string& name) {
    if (value < 0.0) {
        std::stringstream ss;
        ss << "Value " << name << " = " << value << " must be non-negative";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateIntegerRange(int64_t value, int64_t min, int64_t max, const std::string& name) {
    if (value < min || value > max) {
        std::stringstream ss;
        ss << "Value " << name << " = " << value << " is out of range [" << min << ", " << max << "]";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateString(const std::string& value, size_t minLen, size_t maxLen, const std::string& name) {
    if (value.length() < minLen || value.length() > maxLen) {
        std::stringstream ss;
        ss << "String " << name << " length " << value.length() << " is out of range [" << minLen << ", " << maxLen << "]";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validatePointer(const void* ptr, const std::string& name) {
    if (ptr == nullptr) {
        std::stringstream ss;
        ss << "Pointer " << name << " is null";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateNotNull(const std::nullptr_t ptr, const std::string& name) {
    if (ptr != nullptr) {
        std::stringstream ss;
        ss << "Pointer " << name << " is not null";
        return ValidationResult(false, ss.str());
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateConfigKey(const std::string& key) {
    if (key.empty()) {
        return ValidationResult(false, "Configuration key cannot be empty");
    }
    if (key.find('=') != std::string::npos || key.find(' ') != std::string::npos) {
        return ValidationResult(false, "Configuration key cannot contain '=' or spaces: " + key);
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateNeuronId(uint64_t id) {
    if (id == 0 || id == UINT64_MAX) {
        return ValidationResult(false, std::string("Invalid neuron ID: ") + std::to_string(id));
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateSynapseId(uint64_t id) {
    if (id == 0 || id == UINT64_MAX) {
        return ValidationResult(false, std::string("Invalid synapse ID: ") + std::to_string(id));
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateRegionId(uint64_t id) {
    if (id == 0 || id == UINT64_MAX) {
        return ValidationResult(false, std::string("Invalid region ID: ") + std::to_string(id));
    }
    return ValidationResult(true);
}

ValidationResult ParameterValidator::validateAll(std::initializer_list<ValidationResult> results) {
    ValidationResult combined;
    for (const auto& result : results) {
        if (!result.isValid()) {
            combined.addError(result.getMessage());
        }
        if (!result.getWarnings().empty()) {
            combined.addWarning(result.getWarnings());
        }
    }
    return combined;
}

// ErrorHandler implementation
void ErrorHandler::reportError(const NLMError& error) {
    logError(error);
}

void ErrorHandler::reportWarning(const std::string& message, const std::string& context) {
    logWarning(message, context);
}

void ErrorHandler::reportInfo(const std::string& message, const std::string& context) {
    logInfo(message, context);
}

bool ErrorHandler::shouldSuppress(const NLMError& error) {
    // Default implementation - don't suppress any errors
    return false;
}

void ErrorHandler::handleError(const NLMError& error, std::function<void(const NLMError&)> handler) {
    if (handler) {
        handler(error);
    } else {
        reportError(error);
    }
}

bool ErrorHandler::recoverFromError(const std::string& context, std::function<bool()> recoveryAction) {
    if (recoveryAction) {
        try {
            return recoveryAction();
        } catch (...) {
            reportError(NLMError("Recovery action failed", ErrorSeverity::Error, ErrorCategory::System, context));
            return false;
        }
    }
    return true;
}

// Error reporting helper functions
void logError(const NLMError& error) {
    std::stringstream ss;
    ss << "[ERROR] " << error.getMessage();
    if (!error.getFile().empty()) {
        ss << " at " << error.getFile();
        if (error.getLine() > 0) ss << ":" << error.getLine();
    }
    Logger::getGlobal().error(ss.str(), error.getFile(), error.getLine(), "");
}

void logWarning(const std::string& message, const std::string& context) {
    std::stringstream ss;
    ss << "[WARNING] " << message;
    if (!context.empty()) {
        ss << " (context: " << context << ")";
    }
    Logger::getGlobal().warning(ss.str());
}

void logInfo(const std::string& message, const std::string& context) {
    std::stringstream ss;
    ss << "[INFO] " << message;
    if (!context.empty()) {
        ss << " (context: " << context << ")";
    }
    Logger::getGlobal().info(ss.str());
}

// Common error instances
namespace {
    const NLMError memoryAllocationError("Memory allocation failed", ErrorSeverity::Critical, ErrorCategory::Memory);
    const NLMError nullPointerError("Null pointer reference", ErrorSeverity::Error, ErrorCategory::Parameter);
    const NLMError parameterError("Invalid parameter", ErrorSeverity::Error, ErrorCategory::Parameter);
    const NLMError configurationError("Configuration error", ErrorSeverity::Error, ErrorCategory::Configuration);
}

const NLMError& getMemoryAllocationError(const std::string& context) {
    static NLMError err("Memory allocation failed", ErrorSeverity::Critical, ErrorCategory::Memory, "", 0, "");
    err.setContext(context);
    return err;
}

const NLMError& getParameterError(const std::string& paramName, const std::string& expected, const std::string& actual) {
    static NLMError err("Parameter validation failed", ErrorSeverity::Error, ErrorCategory::Parameter, "", 0, "");
    std::stringstream ss;
    ss << "Parameter '" << paramName << "': expected " << expected << ", got " << actual;
    err.getMessage() = ss.str();
    return err;
}

const NLMError& getConfigurationError(const std::string& key, const std::string& reason) {
    static NLMError err("Configuration error", ErrorSeverity::Error, ErrorCategory::Configuration, "", 0, "");
    std::stringstream ss;
    ss << "Configuration key '" << key << "': " << reason;
    err.getMessage() = ss.str();
    return err;
}

const NLMError& getNullPointerError(const std::string& objectType, const std::string& context) {
    static NLMError err("Null pointer reference", ErrorSeverity::Error, ErrorCategory::Parameter, "", 0, "");
    std::stringstream ss;
    ss << objectType << " pointer is null";
    err.getMessage() = ss.str();
    err.setContext(context);
    return err;
}

} // namespace nlm

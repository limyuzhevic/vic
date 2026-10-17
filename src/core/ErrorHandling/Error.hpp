#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <system_error>
#include <optional>
#include <source_location>
#include <type_traits>

namespace nlm {

// Error code categories
enum class ErrorCategory : uint8_t {
    None = 0,
    Configuration = 1,
    NeuralComputation = 2,
    Memory = 3,
    IO = 4,
    Validation = 5,
    Resource = 6,
    System = 7,
    Integration = 8,
    Recovery = 9,
    Custom = 255
};

// Detailed error codes
enum class ErrorCode : uint32_t {
    Unknown = 0xFFFF,
    InvalidArgument = 0xFFFE,
    RuntimeError = 0xFFFD,
    NotImplemented = 0xFFFC,
    
    // Common error codes
    ConfigValidationFailed = 0x1000,
    NeuronInvalidState = 0x2000,
    MemoryAllocationFailed = 0x3000,
    FileNotFound = 0x4000,
    ParameterInvalid = 0x5000,
    ResourceExhausted = 0x6000,
    IntegrationFailed = 0x8000,
    RecoveryFailed = 0x9000
};

// Error context structure for detailed debugging
struct ErrorContext {
    std::string message;
    ErrorCode code;
    ErrorCategory category;
    std::string sourceFile;
    int sourceLine;
    std::string sourceFunction;
    std::chrono::system_clock::time_point timestamp;
    std::optional<std::string> recoveryAction;
    std::optional<std::string> helpText;
    std::map<std::string, std::string> metadata;
    
    ErrorContext() 
        : code(ErrorCode::Unknown), category(ErrorCategory::None), sourceLine(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& msg, ErrorCode ec, ErrorCategory cat)
        : message(msg), code(ec), category(cat), sourceLine(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    ErrorContext(const std::string& msg, ErrorCode ec, ErrorCategory cat,
                 const std::string& file, int line, const std::string& func)
        : message(msg), code(ec), category(cat), sourceFile(file), sourceLine(line), sourceFunction(func) {
        timestamp = std::chrono::system_clock::now();
    }
    
    // Get formatted error message with context
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(category) << "] "
            << "0x" << std::hex << static_cast<int>(code) << ": "
            << message;
        return oss.str();
    }
    
    // Add metadata
    ErrorContext& addMetadata(const std::string& key, const std::string& value) {
        metadata[key] = value;
        return *this;
    }
    
    // Set recovery action
    ErrorContext& setRecoveryAction(const std::string& action) {
        recoveryAction = action;
        return *this;
    }
    
    // Set help text
    ErrorContext& setHelpText(const std::string& help) {
        helpText = help;
        return *this;
    }
};

// Base exception class
class NLMException : public std::exception {
public:
    NLMException(const ErrorContext& context)
        : context_(context), fullMessage_(context.getFullMessage()) {}
    
    virtual ~NLMException() noexcept {}
    
    const char* what() const noexcept override {
        return fullMessage_.c_str();
    }
    
    // Get error context
    const ErrorContext& getContext() const {
        return context_;
    }
    
    // Get error code
    ErrorCode getCode() const {
        return context_.code;
    }
    
    // Get error category
    ErrorCategory getCategory() const {
        return context_.category;
    }
    
    // Try to recover
    virtual bool tryRecover() {
        return false;
    }
    
    // Get recovery action
    std::optional<std::string> getRecoveryAction() const {
        return context_.recoveryAction;
    }
    
protected:
    ErrorContext context_;
    std::string fullMessage_;
};

// Specific exception types for different error categories

class ConfigurationException : public NLMException {
public:
    ConfigurationException(const std::string& message, ErrorCode code = ErrorCode::ConfigValidationFailed)
        : NLMException(ErrorContext(message, code, ErrorCategory::Configuration)) {}
};

class NeuralComputationException : public NLMException {
public:
    NeuralComputationException(const std::string& message, ErrorCode code = ErrorCode::NeuronInvalidState)
        : NLMException(ErrorContext(message, code, ErrorCategory::NeuralComputation)) {}
};

class MemoryException : public NLMException {
public:
    MemoryException(const std::string& message, ErrorCode code = ErrorCode::MemoryAllocationFailed)
        : NLMException(ErrorContext(message, code, ErrorCategory::Memory)) {}
};

class IOError : public NLMException {
public:
    IOError(const std::string& message, ErrorCode code = ErrorCode::FileNotFound)
        : NLMException(ErrorContext(message, code, ErrorCategory::IO)) {}
};

class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message, ErrorCode code = ErrorCode::ParameterInvalid)
        : NLMException(ErrorContext(message, code, ErrorCategory::Validation)) {}
};

class ResourceException : public NLMException {
public:
    ResourceException(const std::string& message, ErrorCode code = ErrorCode::ResourceExhausted)
        : NLMException(ErrorContext(message, code, ErrorCategory::Resource)) {}
};

class IntegrationException : public NLMException {
public:
    IntegrationException(const std::string& message, ErrorCode code = ErrorCode::IntegrationFailed)
        : NLMException(ErrorContext(message, code, ErrorCategory::Integration)) {}
};

class RecoveryException : public NLMException {
public:
    RecoveryException(const std::string& message, ErrorCode code = ErrorCode::RecoveryFailed)
        : NLMException(ErrorContext(message, code, ErrorCategory::Recovery)) {}
};

// Helper macros for exception creation
#define NLM_THROW_CONFIG(msg, code) throw nlm::ConfigurationException(msg, code)
#define NLM_THROW_NEURAL(msg, code) throw nlm::NeuralComputationException(msg, code)
#define NLM_THROW_MEMORY(msg, code) throw nlm::MemoryException(msg, code)
#define NLM_THROW_IO(msg, code) throw nlm::IOError(msg, code)
#define NLM_THROW_VALIDATION(msg, code) throw nlm::ValidationException(msg, code)
#define NLM_THROW_RESOURCE(msg, code) throw nlm::ResourceException(msg, code)
#define NLM_THROW_INTEGRATION(msg, code) throw nlm::IntegrationException(msg, code)
#define NLM_THROW_RECOVERY(msg, code) throw nlm::RecoveryException(msg, code)

// Helper function to create exception based on category
template<typename ExceptionType>
ExceptionType createException(const std::string& message, ErrorCode code) {
    return ExceptionType(message, code);
}

} // namespace nlm

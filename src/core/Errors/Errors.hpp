#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <exception>
#include <unordered_map>
#include <mutex>
#include <stacktrace>

namespace nlm {

// Base error category enumeration
enum class ErrorCategory {
    System = 0,
    Configuration = 1,
    Initialization = 2,
    Memory = 3,
    Validation = 4,
    File = 5,
    Plasticity = 6,
    Network = 7,
    Simulation = 8,
    Python = 9
};

// Base exception class with enhanced context
class NLMError : public std::exception {
protected:
    std::string message_;
    ErrorCategory category_;
    std::string file_;
    int line_;
    std::string function_;
    std::chrono::system_clock::time_point timestamp_;
    std::stacktrace stacktrace_;
    
public:
    NLMError(ErrorCategory category, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : category_(category), message_(message), file_(file), line_(line),
          function_(function), timestamp_(std::chrono::system_clock::now()) {
        if constexpr (std::stacktrace::available()) {
            stacktrace_ = std::stacktrace::current();
        }
    }
    
    virtual ~NLMError() noexcept = default;
    
    // Get error category
    ErrorCategory getCategory() const noexcept { return category_; }
    
    // Get error message with full context
    virtual const char* what() const noexcept override {
        static std::string formatted;
        std::ostringstream oss;
        oss << "NLM " << categoryToString(category_) << " Error: " << message_;
        
        if (!file_.empty()) {
            oss << "\n  File: " << file_;
            if (line_ > 0) oss << ":" << line_;
        }
        
        if (!function_.empty()) {
            oss << "\n  Function: " << function_;
        }
        
        oss << "\n  Time: " << formatTimestamp(timestamp_);
        
        if constexpr (std::stacktrace::available()) {
            oss << "\n  Stack Trace:" << stacktrace_;
        }
        
        formatted = oss.str();
        return formatted.c_str();
    }
    
    // Get error category as string
    std::string getCategoryString() const {
        return categoryToString(category_);
    }
    
    // Get parameter name if applicable
    virtual std::string getParameterName() const { return ""; }
    
    // Get file path if applicable
    virtual std::string getFilePath() const { return file_; }
    
    // Get error type for Python bindings
    virtual std::string getErrorType() const { return "NLMError"; }
    
    // Static helper to convert category to string
    static std::string categoryToString(ErrorCategory category) {
        static const std::unordered_map<ErrorCategory, std::string> categoryNames = {
            {ErrorCategory::System, "System"},
            {ErrorCategory::Configuration, "Configuration"},
            {ErrorCategory::Initialization, "Initialization"},
            {ErrorCategory::Memory, "Memory"},
            {ErrorCategory::Validation, "Validation"},
            {ErrorCategory::File, "File"},
            {ErrorCategory::Plasticity, "Plasticity"},
            {ErrorCategory::Network, "Network"},
            {ErrorCategory::Simulation, "Simulation"},
            {ErrorCategory::Python, "Python"}
        };
        
        auto it = categoryNames.find(category);
        return it != categoryNames.end() ? it->second : "Unknown";
    }
    
private:
    std::string formatTimestamp(const std::chrono::system_clock::time_point& tp) {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%F");
        return oss.str();
    }
};

// Specific exception types for different error categories

// Configuration errors
class ConfigurationError : public NLMError {
public:
    ConfigurationError(const std::string& message,
                      const std::string& paramName = "",
                      const std::string& filePath = "",
                      const std::string& expected = "",
                      const std::string& actual = "")
        : NLMError(ErrorCategory::Configuration, message), 
          paramName_(paramName), filePath_(filePath),
          expected_(expected), actual_(actual) {}
    
    std::string getParameterName() const override { return paramName_; }
    std::string getFilePath() const override { return filePath_; }
    std::string getExpectedValue() const { return expected_; }
    std::string getActualValue() const { return actual_; }
    std::string getErrorType() const override { return "ConfigurationError"; }
    
private:
    std::string paramName_;
    std::string filePath_;
    std::string expected_;
    std::string actual_;
};

// Initialization errors
class InitializationError : public NLMError {
public:
    InitializationError(const std::string& message,
                       const std::string& component = "",
                       const std::string& details = "")
        : NLMError(ErrorCategory::Initialization, message),
          component_(component), details_(details) {}
    
    std::string getComponent() const { return component_; }
    std::string getDetails() const { return details_; }
    std::string getErrorType() const override { return "InitializationError"; }
    
private:
    std::string component_;
    std::string details_;
};

// Memory errors
class MemoryError : public NLMError {
public:
    MemoryError(const std::string& message,
               size_t allocationSize = 0,
               const std::string& operation = "")
        : NLMError(ErrorCategory::Memory, message),
          allocationSize_(allocationSize), operation_(operation) {}
    
    size_t getAllocationSize() const { return allocationSize_; }
    std::string getOperation() const { return operation_; }
    std::string getErrorType() const override { return "MemoryError"; }
    
private:
    size_t allocationSize_;
    std::string operation_;
};

// Validation errors
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message,
                   const std::string& fieldName = "",
                   const std::string& constraint = "",
                   const std::string& value = "")
        : NLMError(ErrorCategory::Validation, message),
          fieldName_(fieldName), constraint_(constraint), value_(value) {}
    
    std::string getFieldName() const { return fieldName_; }
    std::string getConstraint() const { return constraint_; }
    std::string getValue() const { return value_; }
    std::string getErrorType() const override { return "ValidationError"; }
    
private:
    std::string fieldName_;
    std::string constraint_;
    std::string value_;
};

// File errors
class FileError : public NLMError {
public:
    FileError(const std::string& message,
             const std::string& filePath = "",
             FileOperation operation = FileOperation::Unknown)
        : NLMError(ErrorCategory::File, message),
          filePath_(filePath), operation_(operation) {}
    
    std::string getFilePath() const override { return filePath_; }
    FileOperation getOperation() const { return operation_; }
    std::string getErrorType() const override { return "FileError"; }
    
private:
    std::string filePath_;
    FileOperation operation_;
};

enum class FileOperation {
    Read,
    Write,
    Open,
    Close,
    Delete,
    Unknown
};

// Plasticity errors
class PlasticityError : public NLMError {
public:
    PlasticityError(const std::string& message,
                   const std::string& ruleName = "",
                   PlasticityOperation operation = PlasticityOperation::Unknown)
        : NLMError(ErrorCategory::Plasticity, message),
          ruleName_(ruleName), operation_(operation) {}
    
    std::string getRuleName() const { return ruleName_; }
    PlasticityOperation getOperation() const { return operation_; }
    std::string getErrorType() const override { return "PlasticityError"; }
    
private:
    std::string ruleName_;
    PlasticityOperation operation_;
};

enum class PlasticityOperation {
    STDPUpdate,
    HebbianUpdate,
    StructuralChange,
    Unknown
};

// Network errors
class NetworkError : public NLMError {
public:
    NetworkError(const std::string& message,
                const std::string& connectionId = "",
                NetworkErrorCode errorCode = NetworkErrorCode::Unknown)
        : NLMError(ErrorCategory::Network, message),
          connectionId_(connectionId), errorCode_(errorCode) {}
    
    std::string getConnectionId() const { return connectionId_; }
    NetworkErrorCode getErrorCode() const { return errorCode_; }
    std::string getErrorType() const override { return "NetworkError"; }
    
private:
    std::string connectionId_;
    NetworkErrorCode errorCode_;
};

enum class NetworkErrorCode {
    ConnectionFailed,
    InvalidTopology,
    Congestion,
    Timeout,
    Unknown
};

// Simulation errors
class SimulationError : public NLMError {
public:
    SimulationError(const std::string& message,
                   SimulationStep step,
                   const std::string& simulationType = "")
        : NLMError(ErrorCategory::Simulation, message),
          step_(step), simulationType_(simulationType) {}
    
    SimulationStep getStep() const { return step_; }
    std::string getSimulationType() const { return simulationType_; }
    std::string getErrorType() const override { return "SimulationError"; }
    
private:
    SimulationStep step_;
    std::string simulationType_;
};

// System errors (fallback)
class SystemError : public NLMError {
public:
    SystemError(const std::string& message,
               const std::string& subsystem = "")
        : NLMError(ErrorCategory::System, message),
          subsystem_(subsystem) {}
    
    std::string getSubsystem() const { return subsystem_; }
    std::string getErrorType() const override { return "SystemError"; }
    
private:
    std::string subsystem_;
};

} // namespace nlm
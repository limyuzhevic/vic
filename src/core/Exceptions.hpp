#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <source_location>
#include <chrono>
#include "core/Types/Types.hpp"

namespace nlm {

// Error categories for better error classification
enum class ErrorCategory {
    Validation,      // Input validation errors
    Runtime,         // Runtime operation errors
    Configuration,   // Configuration-related errors
    IOError,         // File I/O errors
    Memory,          // Memory management errors
    Network,         // Network communication errors
    System,          // System-level errors
    Internal         // Internal programming errors
};

// Exception severity levels
enum class ErrorSeverity {
    Debug,           // Debugging information
    Info,            // Informational messages
    Warning,         // Warnings that don't prevent operation
    Error,           // Errors that prevent operation
    Critical,        // Critical errors requiring immediate attention
    Fatal            // Fatal errors that require program termination
};

// Base exception class for all NLM exceptions
class NLMException : public std::runtime_error {
public:
    NLMException(
        const std::string& message,
        ErrorCategory category = ErrorCategory::Internal,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& context = ""
    ) : std::runtime_error(message), 
        category_(category),
        severity_(severity),
        context_(context),
        timestamp_(std::chrono::system_clock::now()),
        file_(),
        line_(0) {
        // Extract file and line from source_location if available
        if (!context.empty()) {
            try {
                auto loc = std::source_location::current();
                file_ = std::string(loc.file_name());
                line_ = loc.line();
            } catch (...) {
                // If source_location is not available, use context
                file_ = context;
                line_ = 0;
            }
        }
    }
    
    // Get error category
    ErrorCategory getCategory() const noexcept { return category_; }
    
    // Get error severity
    ErrorSeverity getSeverity() const noexcept { return severity_; }
    
    // Get error context
    const std::string& getContext() const noexcept { return context_; }
    
    // Get timestamp
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }
    
    // Get source file
    const std::string& getFile() const noexcept { return file_; }
    
    // Get line number
    int getLine() const noexcept { return line_; }
    
    // Convert to string representation
    virtual std::string toString() const;
    
    // Format error information
    std::string formatError() const;
};

// Configuration validation exception
class ConfigurationException : public NLMException {
public:
    ConfigurationException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& parameter = ""
    ) : NLMException(message, ErrorCategory::Configuration, severity, "ConfigurationException"),
        parameter_(parameter) {}
    
    const std::string& getParameter() const noexcept { return parameter_; }
    
private:
    std::string parameter_;
};

// I/O exception
class IOError : public NLMException {
public:
    IOError(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& filepath = ""
    ) : NLMException(message, ErrorCategory::IOError, severity, "IOError"),
        filepath_(filepath) {}
    
    const std::string& getFilepath() const noexcept { return filepath_; }
    
private:
    std::string filepath_;
};

// Memory exception
class MemoryException : public NLMException {
public:
    MemoryException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Critical,
        size_t allocated_size = 0
    ) : NLMException(message, ErrorCategory::Memory, severity, "MemoryException"),
        allocated_size_(allocated_size) {}
    
    size_t getAllocatedSize() const noexcept { return allocated_size_; }
    
private:
    size_t allocated_size_;
};

// Runtime exception
class RuntimeException : public NLMException {
public:
    RuntimeException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& operation = ""
    ) : NLMException(message, ErrorCategory::Runtime, severity, "RuntimeException"),
        operation_(operation) {}
    
    const std::string& getOperation() const noexcept { return operation_; }
    
private:
    std::string operation_;
};

// Validation exception
class ValidationException : public NLMException {
public:
    ValidationException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Warning,
        const std::string& field = ""
    ) : NLMException(message, ErrorCategory::Validation, severity, "ValidationException"),
        field_(field) {}
    
    const std::string& getField() const noexcept { return field_; }
    
private:
    std::string field_;
};

// Exception factory for creating appropriate exception types
class ExceptionFactory {
public:
    // Create configuration exception
    static std::shared_ptr<NLMException> createConfigurationException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& parameter = ""
    ) {
        return std::make_shared<ConfigurationException>(message, severity, parameter);
    }
    
    // Create I/O exception
    static std::shared_ptr<NLMException> createIOError(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& filepath = ""
    ) {
        return std::make_shared<IOError>(message, severity, filepath);
    }
    
    // Create memory exception
    static std::shared_ptr<NLMException> createMemoryException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Critical,
        size_t allocated_size = 0
    ) {
        return std::make_shared<MemoryException>(message, severity, allocated_size);
    }
    
    // Create runtime exception
    static std::shared_ptr<NLMException> createRuntimeException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& operation = ""
    ) {
        return std::make_shared<RuntimeException>(message, severity, operation);
    }
    
    // Create validation exception
    static std::shared_ptr<NLMException> createValidationException(
        const std::string& message,
        ErrorSeverity severity = ErrorSeverity::Warning,
        const std::string& field = ""
    ) {
        return std::make_shared<ValidationException>(message, severity, field);
    }
    
    // Create generic exception
    static std::shared_ptr<NLMException> createGenericException(
        const std::string& message,
        ErrorCategory category = ErrorCategory::Internal,
        ErrorSeverity severity = ErrorSeverity::Error,
        const std::string& context = ""
    ) {
        return std::make_shared<NLMException>(message, category, severity, context);
    }
};

} // namespace nlm
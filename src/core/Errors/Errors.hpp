#pragma once

#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace nlm {

// Base exception class for all NLM errors
class NLMBasicError : public std::exception {
protected:
    std::string message_;
    std::string file_;
    int line_;
    std::string function_;
    
public:
    NLMBasicError(const std::string& message, 
                  const std::string& file = "", 
                  int line = 0,
                  const std::string& function = "") 
        : message_(message), file_(file), line_(line), function_(function) {}
    
    virtual ~NLMBasicError() noexcept = default;
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    const std::string& getMessage() const { return message_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    virtual const char* errorType() const noexcept { return "NLMBasicError"; }
};

// Configuration validation errors
class ConfigError : public NLMBasicError {
public:
    enum class ErrorType {
        InvalidKey,
        InvalidValue,
        MissingRequiredParameter,
        InvalidFileFormat,
        ValidationFailed,
        ConversionFailed
    };
    
    ConfigError(const std::string& message, ErrorType type,
                const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "ConfigError"; }
    
private:
    ErrorType type_;
};

// File I/O errors
class FileError : public NLMBasicError {
public:
    enum class ErrorType {
        FileNotFound,
        PermissionDenied,
        FileExists,
        ReadFailed,
        WriteFailed,
        InvalidPath,
        PathTraversalAttempted,
        DirectoryExpected,
        FileExpected
    };
    
    FileError(const std::string& message, ErrorType type,
              const std::string& file = "", int line = 0,
              const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "FileError"; }
    
private:
    ErrorType type_;
};

// Memory management errors
class MemoryError : public NLMBasicError {
public:
    enum class ErrorType {
        AllocationFailed,
        NullPointerDereference,
        BufferOverflow,
        MemoryLeak,
        InvalidAddress,
        OutOfMemory,
        AlignmentError
    };
    
    MemoryError(const std::string& message, ErrorType type,
                const std::string& file = "", int line = 0,
                const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "MemoryError"; }
    
private:
    ErrorType type_;
};

// Input validation errors
class ValidationError : public NLMBasicError {
public:
    enum class ErrorType {
        EmptyInput,
        InvalidFormat,
        OutOfRange,
        InvalidType,
        MissingValue,
        DuplicateEntry,
        InvalidConfiguration
    };
    
    ValidationError(const std::string& message, ErrorType type,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "ValidationError"; }
    
private:
    ErrorType type_;
};

// Network/communication errors
class NetworkError : public NLMBasicError {
public:
    enum class ErrorType {
        ConnectionFailed,
        Timeout,
        ProtocolError,
        InvalidAddress,
        NetworkUnreachable
    };
    
    NetworkError(const std::string& message, ErrorType type,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "NetworkError"; }
    
private:
    ErrorType type_;
};

// Performance-related errors
class PerformanceError : public NLMBasicError {
public:
    enum class ErrorType {
        Timeout,
        ResourceLimitExceeded,
        MemoryUsageTooHigh,
        CPUUsageTooHigh,
        InvalidConfiguration,
        HardwareLimitReached
    };
    
    PerformanceError(const std::string& message, ErrorType type,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "")
        : NLMBasicError(message, file, line, function), type_(type) {}
    
    ErrorType getErrorType() const { return type_; }
    
    const char* errorType() const noexcept override { return "PerformanceError"; }
    
private:
    ErrorType type_;
};

} // namespace nlm

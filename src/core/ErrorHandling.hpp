// Error handling utilities for NLM
#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <map>
#include <vector>
#include <sstream>

namespace nlm {

// Error codes for different types of failures
enum class ErrorCode {
    Success = 0,
    Unknown = 1,
    InvalidArgument = 2,
    FileNotFound = 3,
    PermissionDenied = 4,
    ParseError = 5,
    MemoryAllocationFailed = 6,
    ConfigurationError = 7,
    InitializationFailed = 8,
    OperationFailed = 9,
    ResourceNotAvailable = 10
};

// Exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCode code, const std::string& message, 
                 const std::string& source = "")
        : std::runtime_error(message), code_(code), source_(source) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getSource() const { return source_; }
    
private:
    ErrorCode code_;
    std::string source_;
};

// Result type for functions that can fail
template<typename T>
class Result {
public:
    Result(T value) : value_(std::move(value)), error_(ErrorCode::Success) {}
    Result(ErrorCode error, const std::string& message) 
        : error_(error), errorMessage_(message) {}
    
    bool isSuccess() const { return error_ == ErrorCode::Success; }
    bool isError() const { return !isSuccess(); }
    
    const T& value() const { return value_; }
    const std::string& errorMessage() const { return errorMessage_; }
    ErrorCode errorCode() const { return error_; }
    
    T& value() { return value_; }
    
private:
    T value_;
    ErrorCode error_;
    std::string errorMessage_;
};

// Error category for different error types
class ErrorCategory {
public:
    static const ErrorCategory& getSystem() {
        static const ErrorCategory instance("System");
        return instance;
    }
    
    static const ErrorCategory& getConfig() {
        static const ErrorCategory instance("Config");
        return instance;
    }
    
    static const ErrorCategory& getMemory() {
        static const ErrorCategory instance("Memory");
        return instance;
    }
    
    static const ErrorCategory& getIO() {
        static const ErrorCategory instance("IO");
        return instance;
    }
    
    const std::string& name() const { return name_; }
    
protected:
    ErrorCategory(const std::string& name) : name_(name) {}
    std::string name_;
};

// Error information structure
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string source;
    std::string category;
    std::vector<std::string> suggestions;
    
    ErrorInfo(ErrorCode c, const std::string& msg, const std::string& src = "")
        : code(c), message(msg), source(src), category("Unknown") {}
};

// Error registry for managing common errors
class ErrorRegistry {
public:
    static ErrorRegistry& getInstance() {
        static ErrorRegistry instance;
        return instance;
    }
    
    void registerError(ErrorCode code, const std::string& category,
                      const std::string& message) {
        errors_[code] = {code, message, "", category};
    }
    
    const ErrorInfo& getError(ErrorCode code) const {
        auto it = errors_.find(code);
        if (it != errors_.end()) {
            return it->second;
        }
        return defaultError_;
    }
    
    std::vector<ErrorInfo> getErrorsForCategory(const std::string& category) const {
        std::vector<ErrorInfo> result;
        for (const auto& pair : errors_) {
            if (pair.second.category == category) {
                result.push_back(pair.second);
            }
        }
        return result;
    }
    
private:
    ErrorRegistry() {
        // Initialize common errors
        registerError(ErrorCode::Success, "System", "Operation completed successfully");
        registerError(ErrorCode::Unknown, "System", "An unknown error occurred");
        registerError(ErrorCode::InvalidArgument, "System", "Invalid argument provided");
        registerError(ErrorCode::FileNotFound, "IO", "File not found");
        registerError(ErrorCode::PermissionDenied, "IO", "Permission denied");
        registerError(ErrorCode::ParseError, "Config", "Failed to parse configuration");
        registerError(ErrorCode::MemoryAllocationFailed, "Memory", "Memory allocation failed");
        registerError(ErrorCode::ConfigurationError, "Config", "Invalid configuration");
        registerError(ErrorCode::InitializationFailed, "System", "Failed to initialize");
        registerError(ErrorCode::OperationFailed, "System", "Operation failed");
        registerError(ErrorCode::ResourceNotAvailable, "System", "Resource not available");
    }
    
    std::map<ErrorCode, ErrorInfo> errors_;
    ErrorInfo defaultError_{ErrorCode::Unknown, "Unknown error"};
};

// Helper functions for error handling
inline std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success: return "Success";
        case ErrorCode::Unknown: return "Unknown";
        case ErrorCode::InvalidArgument: return "InvalidArgument";
        case ErrorCode::FileNotFound: return "FileNotFound";
        case ErrorCode::PermissionDenied: return "PermissionDenied";
        case ErrorCode::ParseError: return "ParseError";
        case ErrorCode::MemoryAllocationFailed: return "MemoryAllocationFailed";
        case ErrorCode::ConfigurationError: return "ConfigurationError";
        case ErrorCode::InitializationFailed: return "InitializationFailed";
        case ErrorCode::OperationFailed: return "OperationFailed";
        case ErrorCode::ResourceNotAvailable: return "ResourceNotAvailable";
        default: return "Unknown";
    }
}

// Macro for throwing NLM exceptions
#define NLM_THROW(errorCode, message) throw NLMException(errorCode, message, __func__)

// Macro for creating NLM error results
#define NLM_ERROR_RESULT(errorCode, message) Result<void>(errorCode, message)

// Macro for checking error conditions
#define NLM_CHECK(condition, errorCode, message) \
    if (!(condition)) { \
        NLM_THROW(errorCode, message); \
    }

// Macro for safe error handling
#define NLM_SAFE_OPERATION(operation, errorCode, message) \
    try { \
        operation; \
        return Result<T>(Result<T>::success); \
    } catch (const NLMException& e) { \
        return Result<T>(e.getCode(), e.what()); \
    } catch (...) { \
        return Result<T>(ErrorCode::Unknown, message); \
    }

} // namespace nlm
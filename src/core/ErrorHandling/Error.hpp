// Enhanced Error Handling System
#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <vector>
#include <sstream>

namespace nlm {

// Error codes for better error reporting
enum class ErrorCode {
    // Success
    OK = 0,
    
    // Configuration errors
    CONFIG_FILE_NOT_FOUND = 1,
    CONFIG_FILE_INVALID_FORMAT = 2,
    CONFIG_VALUE_INVALID = 3,
    CONFIG_KEY_MISSING = 4,
    CONFIG_VALIDATION_FAILED = 5,
    
    // Resource errors
    MEMORY_ALLOCATION_FAILED = 10,
    RESOURCE_NOT_AVAILABLE = 11,
    INVALID_RESOURCE_STATE = 12,
    
    // System errors
    INITIALIZATION_FAILED = 20,
    INVALID_PARAMETERS = 21,
    FILE_ACCESS_DENIED = 22,
    NETWORK_ERROR = 23,
    
    // Runtime errors
    NULL_POINTER_DEREFERENCE = 30,
    OUT_OF_BOUNDS_ACCESS = 31,
    DIVISION_BY_ZERO = 32,
    INVALID_OPERATION = 33,
    
    // Debug/Memory errors
    MEMORY_LEAK_DETECTED = 40,
    INVALID_MEMORY_ACCESS = 41,
    
    // General errors
    UNKNOWN_ERROR = 99
};

class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCode code, const std::string& message, 
             const std::string& file = "", int line = 0,
             const std::string& function = "")
        : std::runtime_error(formatMessage(code, message)),
          code_(code), file_(file), line_(line), function_(function) {}
    
    ErrorCode getCode() const { return code_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getFunction() const { return function_; }
    
    std::string getFullDescription() const {
        std::ostringstream oss;
        oss << "Error Code: " << static_cast<int>(code_) << "\n";
        oss << "Message: " << what() << "\n";
        if (!file_.empty()) {
            oss << "Location: " << file_;
            if (line_ > 0) oss << ":" << line_;
            if (!function_.empty()) oss << " in " << function_;
            oss << "\n";
        }
        return oss.str();
    }
    
private:
    static std::string formatMessage(ErrorCode code, const std::string& message) {
        std::ostringstream oss;
        oss << "[Error " << static_cast<int>(code) << "] " << message;
        return oss.str();
    }
    
    ErrorCode code_;
    std::string file_;
    int line_;
    std::string function_;
};

// Error category for different error types
class ErrorCategory {
public:
    virtual ~ErrorCategory() = default;
    virtual const char* name() const noexcept = 0;
    virtual std::string message(int ev) const = 0;
    virtual ErrorCode getErrorCode(int ev) const = 0;
    virtual std::string categoryName() const = 0;
};

class ConfigErrorCategory : public ErrorCategory {
public:
    const char* name() const noexcept override { return "config"; }
    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::CONFIG_FILE_NOT_FOUND: return "Configuration file not found";
            case ErrorCode::CONFIG_FILE_INVALID_FORMAT: return "Invalid configuration file format";
            case ErrorCode::CONFIG_VALUE_INVALID: return "Invalid configuration value";
            case ErrorCode::CONFIG_KEY_MISSING: return "Required configuration key is missing";
            case ErrorCode::CONFIG_VALIDATION_FAILED: return "Configuration validation failed";
            default: return "Unknown configuration error";
        }
    }
    ErrorCode getErrorCode(int ev) const override { return static_cast<ErrorCode>(ev); }
    std::string categoryName() const override { return "Configuration"; }
};

class ResourceErrorCategory : public ErrorCategory {
public:
    const char* name() const noexcept override { return "resource"; }
    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::MEMORY_ALLOCATION_FAILED: return "Memory allocation failed";
            case ErrorCode::RESOURCE_NOT_AVAILABLE: return "Required resource is not available";
            case ErrorCode::INVALID_RESOURCE_STATE: return "Invalid resource state";
            default: return "Unknown resource error";
        }
    }
    ErrorCode getErrorCode(int ev) const override { return static_cast<ErrorCode>(ev); }
    std::string categoryName() const override { return "Resource Management";
}

class SystemErrorCategory : public ErrorCategory {
public:
    const char* name() const noexcept override { return "system"; }
    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::INITIALIZATION_FAILED: return "Initialization failed";
            case ErrorCode::INVALID_PARAMETERS: return "Invalid parameters provided";
            case ErrorCode::FILE_ACCESS_DENIED: return "File access denied";
            case ErrorCode::NETWORK_ERROR: return "Network error occurred";
            default: return "Unknown system error";
        }
    }
    ErrorCode getErrorCode(int ev) const override { return static_cast<ErrorCode>(ev); }
    std::string categoryName() const override { return "System"; }
};

class RuntimeErrorCategory : public ErrorCategory {
public:
    const char* name() const noexcept override { return "runtime"; }
    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::NULL_POINTER_DEREFERENCE: return "Null pointer dereference";
            case ErrorCode::OUT_OF_BOUNDS_ACCESS: return "Out of bounds access";
            case ErrorCode::DIVISION_BY_ZERO: return "Division by zero";
            case ErrorCode::INVALID_OPERATION: return "Invalid operation performed";
            default: return "Unknown runtime error";
        }
    }
    ErrorCode getErrorCode(int ev) const override { return static_cast<ErrorCode>(ev); }
    std::string categoryName() const override { return "Runtime";
}

class DebugErrorCategory : public ErrorCategory {
public:
    const char* name() const noexcept override { return "debug"; }
    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::MEMORY_LEAK_DETECTED: return "Memory leak detected";
            case ErrorCode::INVALID_MEMORY_ACCESS: return "Invalid memory access";
            default: return "Unknown debug error";
        }
    }
    ErrorCode getErrorCode(int ev) const override { return static_cast<ErrorCode>(ev); }
    std::string categoryName() const override { return "Debug";
}

// Global error registry
class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }
    
    void registerCategory(std::unique_ptr<ErrorCategory> category) {
        categories_.push_back(std::move(category));
    }
    
    ErrorCode lookupErrorCode(int ev) const {
        for (const auto& category : categories_) {
            ErrorCode code = category->getErrorCode(ev);
            if (code != ErrorCode::UNKNOWN_ERROR) {
                return code;
            }
        }
        return ErrorCode::UNKNOWN_ERROR;
    }
    
    std::string getCategoryName(int ev) const {
        for (const auto& category : categories_) {
            if (category->getErrorCode(ev) != ErrorCode::UNKNOWN_ERROR) {
                return category->categoryName();
            }
        }
        return "General";
    }
    
private:
    ErrorHandler() {
        registerCategory(std::make_unique<ConfigErrorCategory>());
        registerCategory(std::make_unique<ResourceErrorCategory>());
        registerCategory(std::make_unique<SystemErrorCategory>());
        registerCategory(std::make_unique<RuntimeErrorCategory>());
        registerCategory(std::make_unique<DebugErrorCategory>());
    }
    
    std::vector<std::unique_ptr<ErrorCategory>> categories_;
};

// Convenience functions for throwing errors
inline void throwError(ErrorCode code, const std::string& message,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "") {
    throw NLMError(code, message, file, line, function);
}

inline void throwConfigError(ErrorCode code, const std::string& message,
                            const std::string& key = "",
                            const std::string& file = "", int line = 0,
                            const std::string& function = "") {
    std::string fullMessage = message;
    if (!key.empty()) {
        fullMessage += " (key: '" + key + "')";
    }
    throwError(code, fullMessage, file, line, function);
}

// Error checking macros
#define CHECK_CONDITION(cond, errorCode, message) \
    do { \
        if (!(cond)) { \
            throwError(errorCode, message, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define CHECK_PTR(ptr, errorCode) \
    do { \
        if (!(ptr)) { \
            throwError(errorCode, "Null pointer dereference", __FILE__, __LINE__, __func__); \
        } \
    } while(0)

#define CHECK_CONFIG_VALUE(value, type, errorCode, message) \
    do { \
        try { \
            if (!(value).has_value()) { \
                throwConfigError(errorCode, message, #value, __FILE__, __LINE__, __func__); \
            } \
        } catch(...) { \
            throwConfigError(errorCode, message, #value, __FILE__, __LINE__, __func__); \
        } \
    } while(0)

// Exception handler for logging
void logException(const std::exception& e);

} // namespace nlm

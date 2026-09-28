#pragma once

#include <string>
#include <stdexcept>
#include <memory>

namespace nlm {

// Error categories
enum class ErrorCategory {
    None,
    Memory,
    Configuration,
    Plasticity,
    NeuralDynamics,
    Integration,
    FileIO,
    BoundaryCheck
};

// Custom exception class
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, 
             ErrorCategory category = ErrorCategory::None,
             const std::string& context = "")
        : std::runtime_error(formatMessage(message, category, context)),
          category_(category), context_(context) {}
    
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    
    static std::string formatMessage(const std::string& message, 
                                    ErrorCategory category,
                                    const std::string& context) {
        std::string result = "NLM Error";
        if (!context.empty()) {
            result += " (" + context + ")";
        }
        result += ": " + message;
        
        // Add category information
        switch (category) {
            case ErrorCategory::Memory: result += " [Memory error]"; break;
            case ErrorCategory::Configuration: result += " [Configuration error]"; break;
            case ErrorCategory::Plasticity: result += " [Plasticity error]"; break;
            case ErrorCategory::NeuralDynamics: result += " [Neural dynamics error]"; break;
            case ErrorCategory::Integration: result += " [Integration error]"; break;
            case ErrorCategory::FileIO: result += " [File I/O error]"; break;
            case ErrorCategory::BoundaryCheck: result += " [Boundary check failed]"; break;
            default: break;
        }
        
        return result;
    }
    
private:
    ErrorCategory category_;
    std::string context_;
};

// Result type for functions that may fail
enum class ResultCode {
    Success,
    Failure,
    InvalidArgument,
    NotFound,
    OutOfMemory,
    FileNotFound,
    BoundaryViolation,
    ConfigurationError
};

class Result {
public:
    Result() : code_(ResultCode::Success), message_("Success") {}
    Result(ResultCode code, const std::string& message) 
        : code_(code), message_(message) {}
    
    bool success() const { return code_ == ResultCode::Success; }
    bool failure() const { return !success(); }
    ResultCode code() const { return code_; }
    const std::string& message() const { return message_; }
    
    template<typename T>
    T* asPointer(T& value) {
        if (success()) {
            return &value;
        }
        return nullptr;
    }
    
    template<typename T>
    T asValue(T defaultValue = T()) {
        if (success()) {
            return defaultValue;
        }
        return defaultValue;
    }
    
    // Error checking macros
    static Result success() { return Result(); }
    static Result error(ResultCode code, const std::string& message) {
        return Result(code, message);
    }
    
    static Result checkBoundary(bool condition, const std::string& errorMsg, 
                                ErrorCategory category = ErrorCategory::BoundaryCheck) {
        if (!condition) {
            return Result(ResultCode::BoundaryViolation, errorMsg);
        }
        return Result();
    }
    
    static Result checkPointer(void* ptr, const std::string& errorMsg,
                                ErrorCategory category = ErrorCategory::Memory) {
        if (!ptr) {
            return Result(ResultCode::InvalidArgument, errorMsg);
        }
        return Result();nlm:

#include <memory>
#include <stdexcept>

namespace nlm {

// Error types for better categorization
enum class ErrorType {
    None,
    NullPointer,
    OutOfRange,
    FileNotFound,
    InvalidArgument,
    MemoryAllocation,
    ConfigurationError,
    PlasticityError,
    IntegrationError
};

// Structured error result
class ErrorResult {
public:
    ErrorResult() : success_(true), error_type_(ErrorType::None), error_message_("") {}
    
    ErrorResult(bool success, ErrorType type, const std::string& message)
        : success_(success), error_type_(type), error_message_(message) {}
    
    bool success() const { return success_; }
    bool failure() const { return !success_; }
    ErrorType type() const { return error_type_; }
    const std::string& message() const { return error_message_; }
    
    // Convenience constructors
    static ErrorResult success() { return ErrorResult(); }
    
    static ErrorResult nullPointer(const std::string& context) {
        return ErrorResult(false, ErrorType::NullPointer, 
                          "Null pointer in " + context);
    }
    
    static ErrorResult outOfRange(const std::string& context, 
                                 const std::string& details = "") {
        return ErrorResult(false, ErrorType::OutOfRange,
                          "Out of range in " + context + 
                          (details.empty() ? "" : ": " + details));
    }
    
    static ErrorResult fileNotFound(const std::string& filename) {
        return ErrorResult(false, ErrorType::FileNotFound,
                          "File not found: " + filename);
    }
    
    static ErrorResult invalidArgument(const std::string& context,
                                       const std::string& argument = "") {
        return ErrorResult(false, ErrorType::InvalidArgument,
                          "Invalid argument in " + context +
                          (argument.empty() ? "" : ": " + argument));
    }
    
    static ErrorResult memoryAllocation(const std::string& context,
                                        size_t size = 0) {
        std::string msg = "Memory allocation failed in " + context;
        if (size > 0) {
            msg += " (requested " + std::to_string(size) + " bytes)";
        }
        return ErrorResult(false, ErrorType::MemoryAllocation, msg);
    }
    
    static ErrorResult configuration(const std::string& key,
                                      const std::string& value = "") {
        std::string msg = "Configuration error for key: " + key;
        if (!value.empty()) {
            msg += " with value: " + value;
        }
        return ErrorResult(false, ErrorType::ConfigurationError, msg);
    }
    
    static ErrorResult plasticity(const std::string& context,
                                  const std::string& details = "") {
        std::string msg = "Plasticity error in " + context;
        if (!details.empty()) {
            msg += ": " + details;
        }
        return ErrorResult(false, ErrorType::PlasticityError, msg);
    }
    
    static ErrorResult integration(const std::string& context,
                                   const std::string& details = "") {
        std::string msg = "Integration error in " + context;
        if (!details.empty()) {
            msg += ": " + details;
        }
        return ErrorResult(false, ErrorType::IntegrationError, msg);
    }
    
    // Template for safe operations
    template<typename T>
    T* onSuccess(const std::string& context, T& value) {
        if (success()) {
            return &value;
        }
        return nullptr;
    }
    
    template<typename T>
    T onFailure(const std::string& context, T defaultValue) {
        if (failure()) {
            return defaultValue;
        }
        return defaultValue;
    }
    
    // Check operation with context
    bool check(bool condition, const std::string& context) {
        if (!condition) {
            error_message_ = "Condition failed in " + context;
            success_ = false;
            error_type_ = ErrorType::InvalidArgument;
        }
        return condition;
    }
    
    bool checkNotNull(void* ptr, const std::string& context) {
        return check(ptr != nullptr, context);
    }
    
    bool checkRange(size_t index, size_t max, const std::string& context) {
        return check(index < max, "range check in " + context);
    }
    
private:
    bool success_;
    ErrorType error_type_;
    std::string error_message_;
};

// Macro for standardized error handling
#define CHECK_NULL(ptr, context) \
    if (!(ptr)) { \
        return ErrorResult::nullPointer(context); \
    }

#define CHECK_CONDITION(condition, context) \
    if (!(condition)) { \
        return ErrorResult::invalidArgument(context, #condition); \
    }

#define CHECK_RANGE(index, max, context) \
    if ((index) >= (max)) { \
        return ErrorResult::outOfRange(context, "index " + std::to_string(index) + " >= " + std::to_string(max)); \
    }

#define CHECK_FILE_EXISTS(filename, context) \
    if (std::filesystem::exists(filename)) { \
        return ErrorResult::fileNotFound(filename); \
    }

// Legacy compatibility functions
definition ErrorResult makeError(ErrorType type, const std::string& message) {
    return ErrorResult(false, type, message);
}

definition ErrorResult makeSuccess() {
    return ErrorResult();
}

} // namespace nlm
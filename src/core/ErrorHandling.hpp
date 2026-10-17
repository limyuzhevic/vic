#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace nlm {

// Error codes for consistent error handling across the codebase
enum class ErrorCode {
    Success = 0,
    InvalidArgument = -1,
    NullPointer = -2,
    OutOfRange = -3,
    FileNotFound = -4,
    ConfigurationError = -5,
    ValidationError = -6,
    MemoryAllocation = -7,
    NotInitialized = -8,
    InvalidState = -9,
    OperationFailed = -10
};

// Standardized error handling macros
#define NLM_THROW(errorCode, message) \
    do { \
        throw std::runtime_error("NLM Error [" + std::to_string(static_cast<int>(errorCode)) + "]: " + message); \
    } while(0)

#define NLM_THROW_INVALID_ARG(message) \
    do { \
        throw std::invalid_argument("NLM InvalidArgument: " + message); \
    } while(0)

#define NLM_THROW_NULL_PTR(message) \
    do { \
        throw std::runtime_error("NLM NullPointer: " + message); \
    } while(0)

#define NLM_THROW_VALIDATION(message) \
    do { \
        throw std::runtime_error("NLM ValidationError: " + message); \
    } while(0)

// Standardized null check macro
#define NLM_CHECK_NULL(ptr, message) \
    do { \
        if ((ptr) == nullptr) { \
            NLM_THROW_NULL_PTR(message); \
        } \
    } while(0)

// Standardized validation check macro
#define NLM_CHECK_VALIDATION(condition, message) \
    do { \
        if (!(condition)) { \
            NLM_THROW_VALIDATION(message); \
        } \
    } while(0)

// Standardized empty container check
#define NLM_CHECK_NOT_EMPTY(container, message) \
    do { \
        if ((container).empty()) { \
            NLM_THROW_VALIDATION((message) + " (container is empty)"); \
        } \
    } while(0)

// Standardized range check
#define NLM_CHECK_RANGE(value, min, max, message) \
    do { \
        if ((value) < (min) || (value) > (max)) { \
            NLM_THROW_ERROR(ErrorCode::OutOfRange, (message) + " (value " + std::to_string(static_cast<int>(value)) + " not in range [" + std::to_string(static_cast<int>(min)) + ", " + std::to_string(static_cast<int>(max)) + "]"); \
        } \
    } while(0)

// Standardized error logging macros (can be connected to actual logging system)
#define NLM_LOG_ERROR(message) \
    do { \
        // Integration with actual logging system goes here \
        /* std::cerr << "[NLM ERROR] " << message << std::endl; */ \
    } while(0)

#define NLM_LOG_WARNING(message) \
    do { \
        /* std::cerr << "[NLM WARNING] " << message << std::endl; */ \
    } while(0)

#define NLM_LOG_INFO(message) \
    do { \
        /* std::cout << "[NLM INFO] " << message << std::endl; */ \
    } while(0)

// Standardized error class for detailed error information
class NLMError : public std::runtime_error {
public:
    NLMError(ErrorCode code, const std::string& message)
        : std::runtime_error(formatError(code, message)), code_(code), message_(message) {}
    
    ErrorCode getCode() const noexcept { return code_; }
    const std::string& getMessage() const noexcept { return message_; }
    
private:
    static std::string formatError(ErrorCode code, const std::string& message) {
        return "NLM Error [" + std::to_string(static_cast<int>(code)) + "]: " + message;
    }
    
    ErrorCode code_;
    std::string message_;
};

// Standardized result type for functions that can fail
// This provides a consistent way to return either a value or an error
// instead of using different error codes or exceptions per function.
template<typename T>
class Result {
public:
    Result(const T& value) : value_(value), error_(ErrorCode::Success), hasValue_(true) {}
    Result(T&& value) : value_(std::move(value)), error_(ErrorCode::Success), hasValue_(true) {}
    Result(ErrorCode error, const std::string& message) 
        : value_(T{}), error_(error), hasValue_(false), errorMessage_(message) {}
    
    bool success() const noexcept { return error_ == ErrorCode::Success; }
    bool failure() const noexcept { return !success(); }
    
    const T& getValue() const & { 
        if (!success()) {
            throw NLMError(error_, errorMessage_);
        }
        return value_; 
    }
    
    T& getValue() & { 
        if (!success()) {
            throw NLMError(error_, errorMessage_);
        }
        return value_; 
    }
    
    const T&& getValue() const && { 
        if (!success()) {
            throw NLMError(error_, errorMessage_);
        }
        return std::move(value_); 
    }
    
    T&& getValue() && { 
        if (!success()) {
            throw NLMError(error_, errorMessage_);
        }
        return std::move(value_); 
    }
    
    ErrorCode getErrorCode() const noexcept { return error_; }
    const std::string& getErrorMessage() const noexcept { return errorMessage_; }
    
    static Result success(const T& value) { return Result(value); }
    static Result success(T&& value) { return Result(std::move(value)); }
    static Result failure(ErrorCode error, const std::string& message) { 
        return Result(error, message); 
    }
    
private:
    T value_;
    ErrorCode error_;
    bool hasValue_;
    std::string errorMessage_;
    
    Result(ErrorCode error, const std::string& message) 
        : value_(T{}), error_(error), hasValue_(false), errorMessage_(message) {}
};

// Convenience typedefs for common result types
using VoidResult = Result<void>;
using IntResult = Result<int>;
using FloatResult = Result<float>;
using StringResult = Result<std::string>;
using VectorResult = Result<std::vector<float>>;

// Helper functions for creating common results
inline VoidResult SuccessResult() { return VoidResult::success(); }
inline IntResult SuccessResult(int value) { return IntResult::success(value); }
inline FloatResult SuccessResult(float value) { return FloatResult::success(value); }
inline StringResult SuccessResult(const std::string& value) { return StringResult::success(value); }
inline VectorResult SuccessResult(const std::vector<float>& value) { return VectorResult::success(value); }

// Standardized validation helper functions
template<typename T>
bool isValidRange(const T& value, const T& min, const T& max) {
    return value >= min && value <= max;
}

template<typename Container>
bool isNotEmpty(const Container& container) {
    return !container.empty();
}

inline bool isNotNull(const void* ptr) {
    return ptr != nullptr;
}

// Standardized error handling function for consistency
inline ErrorCode handleError(ErrorCode error, const std::string& context) {
    NLM_LOG_ERROR(context + " - Error: " + std::to_string(static_cast<int>(error)));
    return error;
}

// Helper macro for consistent error handling in function prologues
#define NLM_FUNCTION_PROLOGUE \
    do { \
        if (this == nullptr) { \
            NLM_THROW_NULL_PTR("Function called on null object"); \
        } \
    } while(0)

// Helper macro for parameter validation
#define NLM_VALIDATE_PARAM(condition, message) \
    do { \
        if (!(condition)) { \
            NLM_THROW_INVALID_ARG(message); \
        } \
    } while(0)

// Helper macro for operation validation
#define NLM_VALIDATE_OPERATION(condition, message) \
    do { \
        if (!(condition)) { \
            NLM_THROW_VALIDATION(message); \
        } \
    } while(0)

} // namespace nlm

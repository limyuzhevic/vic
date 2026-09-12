#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

namespace nlm {

// Custom exception for NLM errors
class NLMError : public std::runtime_error {
public:
    enum class ErrorCode {
        NULL_POINTER,
        OUT_OF_BOUNDS,
        DIVISION_BY_ZERO,
        INVALID_PARAMETER,
        MEMORY_ALLOCATION_FAILED,
        CONFIGURATION_ERROR,
        FILE_OPERATION_FAILED,
        SYSTEM_INITIALIZATION_FAILED
    };

    NLMError(ErrorCode code, const std::string& message) 
        : std::runtime_error(message), code_(code) {}

    ErrorCode getCode() const { return code_; }

private:
    ErrorCode code_;
};

// Error handling macros
#define NLM_CHECK_NULL(ptr, msg) \
    if (!(ptr)) { \
        NLM_LOG_ERROR(msg); \
        throw NLMError(NLMError::ErrorCode::NULL_POINTER, msg); \
    }

#define NLM_CHECK_DIVISION(x, y, msg) \
    if ((y) == 0) { \
        NLM_LOG_ERROR(msg); \
        throw NLMError(NLMError::ErrorCode::DIVISION_BY_ZERO, msg); \
    }

#define NLM_CHECK_BOUNDS(index, size, msg) \
    if ((index) >= (size)) { \
        NLM_LOG_ERROR(msg); \
        throw NLMError(NLMError::ErrorCode::OUT_OF_BOUNDS, msg); \
    }

#define NLM_VALIDATE_PARAM(condition, msg) \
    if (!(condition)) { \
        NLM_LOG_ERROR(msg); \
        throw NLMError(NLMError::ErrorCode::INVALID_PARAMETER, msg); \
    }

#define NLM_TRY_CATCH(operation, error_msg) \
    try { \
        operation; \
    } catch (const std::exception& e) { \
        NLM_LOG_ERROR(error_msg + std::string(" Exception: ") + e.what()); \
        throw; \
    }

// Safe division with zero check
defineinline float safe_divide(float numerator, float denominator, float default_value = 0.0f) {
    if (denominator == 0.0f) {
        NLM_LOG_WARNING("Division by zero in safe_divide, using default value");
        return default_value;
    }
    return numerator / denominator;
}

// Safe array access with bounds checking
defineinline template<typename T, typename Index>
T safe_access(const std::vector<T>& vec, Index index, const T& default_value = T()) {
    if (index >= vec.size()) {
        NLM_LOG_WARNING("Array bounds access in safe_access, using default value");
        return default_value;
    }
    return vec[index];
}

} // namespace nlm

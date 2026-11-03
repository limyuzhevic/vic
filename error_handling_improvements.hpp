#error_handling_improvements.hpp
// Error handling improvements for NLM core functions
// Replaces silent return patterns with proper error reporting

#pragma once

#include <string>
#include <memory>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Error handling utilities
class NLMError {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        NULL_POINTER,
        INVALID_PARAMETER,
        FILE_NOT_FOUND,
        MEMORY_ALLOCATION_FAILED,
        INITIALIZATION_FAILED,
        CONFIGURATION_ERROR,
        NETWORK_ERROR,
        TIMEOUT,
        UNKNOWN_ERROR
    };

    static std::string getErrorCodeString(ErrorCode code);
    static void logError(ErrorCode code, const std::string& context, 
                        const std::string& details = "");
};

// Enhanced error checking macros
#define CHECK_NULL(ptr, error_msg) \
    if (!(ptr)) { \
        NLMError::logError(NLMError::ErrorCode::NULL_POINTER, error_msg, "NULL pointer detected"); \
        return false; \
    }

#define CHECK_NULL_RETURN(ptr, default_value) \
    if (!(ptr)) { \
        NLMError::logError(NLMError::ErrorCode::NULL_POINTER, "Pointer is NULL", "Function call may fail"); \
        return default_value; \
    }

#define CHECK_CONDITION(condition, error_code, error_msg) \
    if (!(condition)) { \
        NLMError::logError(error_code, error_msg, "Validation failed"); \
        return false; \
    }

// Safe pointer access template
template<typename T>
class SafePointer {
private:
    std::shared_ptr<T> ptr;
    
public:
    SafePointer() : ptr(nullptr) {}
    SafePointer(std::shared_ptr<T> p) : ptr(p) {}
    SafePointer(T* p) : ptr(p) {}
    
    bool valid() const { return ptr != nullptr; }
    operator bool() const { return valid(); }
    T* operator->() const { return ptr.get(); }
    T& operator*() const { return *ptr; }
    
    template<typename U>
    bool operator==(const SafePointer<U>& other) const {
        return ptr.get() == other.operator->();
    }
    
    template<typename U>
    bool operator!=(const SafePointer<U>& other) const {
        return !(*this == other);
    }
};

// Exception-safe operation wrapper
class SafeOperation {
private:
    bool success_;
    std::string errorMessage_;
    
public:
    SafeOperation() : success_(true), errorMessage_("") {}
    
    bool success() const { return success_; }
    const std::string& errorMessage() const { return errorMessage_; }
    
    void setSuccess() { success_ = true; }
    void setFailure(const std::string& msg) { 
        success_ = false; 
        errorMessage_ = msg; 
        NLMError::logError(NLMError::ErrorCode::UNKNOWN_ERROR, "Operation failed", msg);
    }
    
    template<typename Func, typename... Args>
    auto execute(Func&& func, Args&&... args) -> decltype(func(args...)) {
        try {
            auto result = func(std::forward<Args>(args)...);
            setSuccess();
            return result;
        } catch (const std::exception& e) {
            setFailure(e.what());
            throw; // Re-throw after logging
        }
    }
};

} // namespace nlm

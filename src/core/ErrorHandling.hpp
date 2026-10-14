#pragma once

/**
 * @file ErrorHandling.hpp
 * @brief Comprehensive error handling system for NLM
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the NLM error handling system, providing a consistent
 * approach to error reporting, validation, and recovery across the entire
 * computational neuroscience platform.
 * 
 * The error handling system includes:
 * - Exception hierarchy for different error types
 * - Error categories (validation, runtime, system, etc.)
 * - Error context preservation
 * - Result type for error handling without exceptions
 * - Integration with logging system
 * - Validation utilities
 * 
 * This system follows best practices for:
 * - Modern C++ error handling (RAII, exception safety)
 * - Resource management and cleanup
 * - Performance (minimal overhead in hot paths)
 * - Debugging support (detailed error context)
 */

#include <string>
#include <memory>
#include <system_error>
#include <stdexcept>
#include <format>
#include <optional>
#include <vector>
#include <functional>
#include <chrono>
#include <any>
#include <sstream>
#include "Logger.hpp"

namespace nlm {

/**
 * @brief Error categories for classification
 */
enum class ErrorCategory {
    Validation,      ///< Input validation errors
    Runtime,         ///< Runtime operation errors
    Configuration,    ///< Configuration parsing/handling errors
    System,          ///< System-level errors (memory, I/O, etc.)
    Network,         ///< Network communication errors
    Physics,         ///< Numerical/physics simulation errors
    Memory,          ///< Memory management errors
    Threading,       ///< Threading/concurrency errors
    User,            ///< User input/API usage errors
    Development      ///< Development-time errors (assertions, preconditions)
};

/**
 * @brief Base error class with context preservation
 */
class NLMError : public std::runtime_error {
public:
    NLMError(const std::string& message, ErrorCategory category = ErrorCategory::Runtime,
             const std::string& operation = "", int errorCode = 0)
        : std::runtime_error(message)
        , category_(category)
        , operation_(operation)
        , errorCode_(errorCode)
        , timestamp_(std::chrono::system_clock::now()) {
        logError();
    }

    virtual ~NLMError() noexcept = default;

    // Error category
    ErrorCategory getCategory() const noexcept { return category_; }
    
    // Operation that caused the error
    const std::string& getOperation() const noexcept { return operation_; }
    
    // Error code (useful for system errors)
    int getErrorCode() const noexcept { return errorCode_; }
    
    // When the error occurred
    std::chrono::system_clock::time_point getTimestamp() const noexcept { return timestamp_; }
    
    // Format error details for debugging
    virtual std::string formatDetails() const;
    
    // Print to stream with context
    virtual void print(std::ostream& os) const;

protected:
    virtual void logError() const;

private:
    ErrorCategory category_;
    std::string operation_;
    int errorCode_;
    std::chrono::system_clock::time_point timestamp_;
};

/**
 * @brief Validation errors for input validation failures
 */
class ValidationError : public NLMError {
public:
    ValidationError(const std::string& message, const std::string& field = "",
                    const std::string& expected = "", const std::string& actual = "")
        : NLMError(message, ErrorCategory::Validation)
        , field_(field)
        , expected_(expected)
        , actual_(actual) {}

    const std::string& getField() const noexcept { return field_; }
    const std::string& getExpected() const noexcept { return expected_; }
    const std::string& getActual() const noexcept { return actual_; }
    
    std::string formatDetails() const override;
    void print(std::ostream& os) const override;

private:
    std::string field_;
    std::string expected_;
    std::string actual_;
};

/**
 * @brief Runtime errors for operational failures
 */
class RuntimeError : public NLMError {
public:
    RuntimeError(const std::string& message, const std::string& operation,
                 int errorCode = 0, std::error_code systemError = std::error_code())
        : NLMError(message, ErrorCategory::Runtime, operation, errorCode)
        , systemError_(systemError) {}

    std::error_code getSystemError() const noexcept { return systemError_; }
    std::string formatDetails() const override;
    void print(std::ostream& os) const override;

private:
    std::error_code systemError_;
};

/**
 * @brief System errors for resource management failures
 */
class SystemError : public NLMError {
public:
    enum class SystemErrorType {
        MemoryAllocation,
        FileSystem,
        Network,
        Threading,
        OperatingSystem,
        LibraryFunction,
        PlatformSpecific
    };

    SystemError(const std::string& message, SystemErrorType type,
                int errorCode = 0, const std::string& operation = "")
        : NLMError(message, ErrorCategory::System, operation, errorCode)
        , type_(type) {}

    SystemErrorType getType() const noexcept { return type_; }
    std::string getTypeName() const;
    std::string formatDetails() const override;
    void print(std::ostream& os) const override;

private:
    SystemErrorType type_;
};

/**
 * @brief Configuration errors for configuration parsing/handling
 */
class ConfigError : public NLMError {
public:
    enum class ErrorType {
        InvalidFormat,
        MissingRequired,
        InvalidValue,
        CircularReference,
        SchemaValidation,
        FileNotFound,
        PermissionDenied
    };

    ConfigError(const std::string& message, ErrorType type,
                const std::string& key = "", const std::string& filepath = "")
        : NLMError(message, ErrorCategory::Configuration)
        , type_(type)
        , key_(key)
        , filepath_(filepath) {}

    ErrorType getType() const noexcept { return type_; }
    const std::string& getKey() const noexcept { return key_; }
    const std::string& getFilepath() const noexcept { return filepath_; }
    std::string getTypeName() const;
    std::string formatDetails() const override;
    void print(std::ostream& os) const override;

private:
    ErrorType type_;
    std::string key_;
    std::string filepath_;
};

/**
 * @brief Result type for error handling without exceptions
 * 
 * Template for operations that can fail but want to use exception-free
 * error handling patterns.
 * 
 * @tparam T Return type
 */
template<typename T>
class Result {
public:
    Result(const T& value) : value_(value), hasError_(false) {}
    Result(T&& value) : value_(std::move(value)), hasError_(false) {}
    Result(const NLMError& error) : error_(error), hasError_(true) {}
    Result(NLMError&& error) : error_(std::move(error)), hasError_(true) {}

    bool hasError() const noexcept { return hasError_; }
    bool isSuccess() const noexcept { return !hasError_; }
    
    T& get() & { return value_; }
    const T& get() const& { return value_; }
    T&& get() && { return std::move(value_); }
    const T&& get() const&& { return std::move(value_); }
    
    const NLMError& getError() const& { return error_; }
    NLMError& getError() & { return error_; }
    
    // Access value with error checking
    const T* operator->() const { return &get(); }
    T* operator->() { return &get(); }
    
    // Implicit conversion to bool for checking success
    explicit operator bool() const noexcept { return !hasError_; }
    
    // Convenience methods
    T valueOr(const T& defaultValue) const { 
        return hasError() ? defaultValue : get(); 
    }
    
    T valueOrThrow() const {
        if (hasError()) {
            throw error_;
        }
        return get();
    }

private:
    union {
        T value_;
        NLMError error_;
    };
    bool hasError_;
    
    // Placement new for union
    void* operator new(std::size_t) = delete;
    void operator delete(void*) = delete;
};

/**
 * @brief Validation utilities for input checking
 */
class Validation {
public:
    // Type validation
    template<typename T>
    static bool isNotNull(const T* ptr, const std::string& fieldName = "") {
        if (!ptr) {
            logError(fieldName + " cannot be null");
            return false;
        }
        return true;
    }
    
    template<typename T>
    static bool isNotNull(T& value, const std::string& fieldName = "") {
        if (!&value) {
            logError(fieldName + " cannot be null");
            return false;
        }
        return true;
    }
    
    // Range validation
    static bool isInRange(double value, double min, double max, 
                         const std::string& fieldName = "",
                         const std::string& unit = "") {
        if (value < min || value > max) {
            logError(format("{} must be in range [{}, {}]{}", 
                           fieldName, min, max, 
                           unit.empty() ? "" : " " + unit));
            return false;
        }
        return true;
    }
    
    // Size validation
    template<typename Container>
    static bool hasSize(const Container& container, size_t expectedSize,
                       const std::string& fieldName = "") {
        if (container.size() != expectedSize) {
            logError(format("{} must have size {} but has {}",
                           fieldName, expectedSize, container.size()));
            return false;
        }
        return true;
    }
    
    // Positive validation
    static bool isPositive(double value, const std::string& fieldName = "") {
        if (value <= 0.0) {
            logError(format("{} must be positive, got {}", fieldName, value));
            return false;
        }
        return true;
    }
    
    // String validation
    static bool isNotEmpty(const std::string& str, const std::string& fieldName = "") {
        if (str.empty()) {
            logError(format("{} cannot be empty", fieldName));
            return false;
        }
        return true;
    }
    
    // Enum validation
    template<typename Enum>
    static bool isValidEnum(Enum value, const std::string& fieldName = "") {
        // Basic validation - could be enhanced with enum range checking
        if (static_cast<int>(value) < 0) {
            logError(format("Invalid value for {} enum: {}", fieldName, 
                           static_cast<int>(value)));
            return false;
        }
        return true;
    }
    
    // Conditional validation
    template<typename T>
    static bool ifThen(bool condition, const T& validationResult, 
                      const std::string& fieldName = "") {
        if (condition && !validationResult) {
            logError(format("Validation failed for {} despite condition passing", 
                           fieldName));
            return false;
        }
        return condition && validationResult;
    }

private:
    static void logError(const std::string& message) {
        NLM_LOG_ERROR(message);
    }
};

/**
 * @brief Exception safety utilities
 */
class ExceptionSafety {
public:
    /**
     * @brief Template for exception-safe resource management
     */
    template<typename T, typename... Args>
    class SafeResource {
    public:
        SafeResource(Args&&... args) {
            try {
                resource_ = std::make_unique<T>(std::forward<Args>(args)...);
            } catch (...) {
                logError("Resource allocation failed");
                throw;
            }
        }
        
        T& get() { return *resource_; }
        const T& get() const { return *resource_; }
        T* operator->() { return resource_.get(); }
        const T* operator->() const { return resource_.get(); }
        
        void reset() {
            resource_.reset();
        }
        
        bool isValid() const { return resource_ != nullptr; }
        
    private:
        std::unique_ptr<T> resource_;
    };
    
    /**
     * @brief RAII-style exception safety for operations
     */
    template<typename T>
    class Transaction {
    public:
        Transaction(std::function<T()> operation) 
            : operation_(std::move(operation)), committed_(false) {}
        
        T commit() {
            if (committed_) {
                throw RuntimeError("Transaction already committed", "commit");
            }
            committed_ = true;
            return operation_();
        }
        
        void rollback() {
            if (!committed_) {
                // Perform rollback logic
            }
        }
        
    private:
        std::function<T()> operation_;
        bool committed_;
    };

private:
    static void logError(const std::string& message) {
        NLM_LOG_ERROR(message);
    }
};

/**
 * @brief Convenience macros for error handling
 */
#define NLM_THROW_VALIDATION(field, message, expected, actual) \
    throw ValidationError(message, field, expected, actual)

#define NLM_THROW_RUNTIME(operation, message, errorCode) \
    throw RuntimeError(message, operation, errorCode)

#define NLM_THROW_SYSTEM(type, message, errorCode, operation) \
    throw SystemError(message, type, errorCode, operation)

#define NLM_THROW_CONFIG(type, message, key, filepath) \
    throw ConfigError(message, type, key, filepath)

/**
 * @brief Result utility macros
 */
#define NLM_RETURN_ERROR(error) return Result<decltype(std::declval())>(error)

/**
 * @brief Error context for debugging
 */
struct ErrorContext {
    std::string operation;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string additionalInfo;
    
    ErrorContext(const std::string& op, const std::string& file, int line,
                 const std::string& func)
        : operation(op), file(file), line(line), function(func),
          timestamp(std::chrono::system_clock::now()) {}
    
    std::string format() const {
        std::ostringstream oss;
        oss << "Operation: " << operation << "\n"
            << "File: " << file << ":" << line << "\n"
            << "Function: " << function << "\n"
            << "Time: " << std::chrono::system_clock::to_time_t(timestamp) << "\n";
        if (!additionalInfo.empty()) {
            oss << "Additional info: " << additionalInfo << "\n";
        }
        return oss.str();
    }
};

/**
 * @brief Global error context manager
 */
class ErrorContextManager {
public:
    static void setContext(const ErrorContext& context) {
        currentContext_ = context;
    }
    
    static ErrorContext getContext() {
        return currentContext_;
    }
    
    static void clearContext() {
        currentContext_ = ErrorContext("", "", 0, "");
    }
    
    static std::string getFormattedContext() {
        return currentContext_.format();
    }

private:
    static ErrorContext currentContext_;
};

/**
 * @brief Enhanced logging with error context
 */
#define NLM_LOG_ERROR_WITH_CONTEXT(operation, message) \
    do { \
        nlm::ErrorContextManager::setContext( \
            nlm::ErrorContext(operation, __FILE__, __LINE__, __func__) \
        ); \
        NLM_LOG_ERROR(nlm::ErrorContextManager::getFormattedContext() + "\n" + message); \
        nlm::ErrorContextManager::clearContext(); \
    } while(0)

/**
 * @brief Performance-optimized validation macro
 */
#define NLM_VALIDATE(condition, error) \
    do { \
        if (!(condition)) { \
            NLM_THROW_VALIDATION("validation", #condition, "", ""); \
        } \
    } while(0)

} // namespace nlm

// Explicit instantiations
namespace nlm {
    template std::string NLMError::formatDetails() const;
    template void NLMError::print(std::ostream&) const;
    template std::string ValidationError::formatDetails() const;
    template void ValidationError::print(std::ostream&) const;
    template std::string RuntimeError::formatDetails() const;
    template void RuntimeError::print(std::ostream&) const;
    template std::string SystemError::formatDetails() const;
    template void SystemError::print(std::ostream&) const;
    template std::string ConfigError::formatDetails() const;
    template void ConfigError::print(std::ostream&) const;
}
// NLM (熙然) - Neural Learning Machine
// Error handling system with Result<T> pattern
// 
// This file implements a comprehensive error handling system that provides
// consistent error reporting across all NLM subsystems. The Result<T> pattern
// offers type-safe error handling with detailed error information.

#pragma once

#include "../core/Constants/Constants.hpp"
#include <string>
#include <optional>
#include <variant>
#include <system_error>
#include <stdexcept>
#include <sstream>

namespace nlm {

/**
 * @enum ErrorCategory
 * @brief Categories of errors in the NLM system
 */
enum class ErrorCategory {
    // Configuration and initialization errors
    ConfigError,
    InitializationError,
    MemoryAllocationError,
    
    // Simulation and computation errors
    SimulationError,
    InvalidOperationError,
    ConvergenceError,
    
    // I/O and file errors
    FileError,
    CheckpointError,
    
    // Resource and performance errors
    OutOfMemoryError,
    TimeoutError,
    ResourceLimitError,
    
    // Network and connectivity errors
    ConnectionError,
    NetworkError,
    
    // System and platform errors
    SystemError,
    ThreadError,
    
    // Data validation errors
    ValidationError,
    SerializationError,
    
    // Development and plasticity errors
    DevelopmentalError,
    PlasticityError,
    
    // Cognitive and memory errors
    MemoryError,
    PerceptionError,
    PlanningError
};

/**
 * @brief Converts ErrorCategory to string for error reporting
 * 
 * @param category Error category to convert
 * @return std::string Human-readable error category name
 */
inline std::string toString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::ConfigError: return "Configuration Error";
        case ErrorCategory::InitializationError: return "Initialization Error";
        case ErrorCategory::MemoryAllocationError: return "Memory Allocation Error";
        case ErrorCategory::SimulationError: return "Simulation Error";
        case ErrorCategory::InvalidOperationError: return "Invalid Operation Error";
        case ErrorCategory::ConvergenceError: return "Convergence Error";
        case ErrorCategory::FileError: return "File Error";
        case ErrorCategory::CheckpointError: return "Checkpoint Error";
        case ErrorCategory::OutOfMemoryError: return "Out of Memory Error";
        case ErrorCategory::TimeoutError: return "Timeout Error";
        case ErrorCategory::ResourceLimitError: return "Resource Limit Error";
        case ErrorCategory::ConnectionError: return "Connection Error";
        case ErrorCategory::NetworkError: return "Network Error";
        case ErrorCategory::SystemError: return "System Error";
        case ErrorCategory::ThreadError: return "Thread Error";
        case ErrorCategory::ValidationError: return "Validation Error";
        case ErrorCategory::SerializationError: return "Serialization Error";
        case ErrorCategory::DevelopmentalError: return "Developmental Error";
        case ErrorCategory::PlasticityError: return "Plasticity Error";
        case ErrorCategory::MemoryError: return "Memory Error";
        case ErrorCategory::PerceptionError: return "Perception Error";
        case ErrorCategory::PlanningError: return "Planning Error";
        default: return "Unknown Error";
    }
}

/**
 * @class ErrorInfo
 * @brief Detailed error information with context and suggestions
 * 
 * Contains comprehensive error information including category, message,
 * location, and recovery suggestions. Provides detailed debugging
 * information for error analysis and recovery.
 */
class ErrorInfo {
public:
    /**
     * @brief Constructs error information
     * 
     * @param category Error category
     * @param message Error message
     * @param context Optional context information
     * @param suggestion Optional recovery suggestion
     * @param errorCode Optional error code for system errors
     */
    ErrorInfo(ErrorCategory category,
              const std::string& message,
              const std::string& context = "",
              const std::string& suggestion = "",
              int errorCode = 0)
        : category_(category)
        , message_(message)
        , context_(context)
        , suggestion_(suggestion)
        , errorCode_(errorCode)
        , timestamp_(std::chrono::system_clock::now()) {}
    
    /**
     * @brief Gets error category
     * 
     * @return ErrorCategory Error category
     */
    ErrorCategory getCategory() const { return category_; }
    
    /**
     * @brief Gets error message
     * 
     * @return const std::string& Error message
     */
    const std::string& getMessage() const { return message_; }
    
    /**
     * @brief Gets error context
     * 
     * @return const std::string& Error context
     */
    const std::string& getContext() const { return context_; }
    
    /**
     * @brief Gets recovery suggestion
     * 
     * @return const std::string& Recovery suggestion
     */
    const std::string& getSuggestion() const { return suggestion_; }
    
    /**
     * @brief Gets system error code
     * 
     * @return int System error code
     */
    int getErrorCode() const { return errorCode_; }
    
    /**
     * @brief Gets error timestamp
     * 
     * @return auto Error timestamp
     */
    auto getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Checks if error is recoverable
     * 
     * @return bool true if error has recovery suggestion
     */
    bool isRecoverable() const { return !suggestion_.empty(); }
    
    /**
     * @brief Gets formatted error report
     * 
     * @return std::string Formatted error report including all details
     */
    std::string getReport() const {
        std::ostringstream oss;
        oss << "=== Error Report ===\n";
        oss << "Category: " << toString(category_) << "\n";
        oss << "Message: " << message_ << "\n";
        if (!context_.empty()) {
            oss << "Context: " << context_ << "\n";
        }
        if (!suggestion_.empty()) {
            oss << "Suggestion: " << suggestion_ << "\n";
        }
        if (errorCode_ != 0) {
            oss << "System Error Code: " << errorCode_ << "\n";
        }
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(timestamp_);
        return oss.str();
    }
    
    /**
     * @brief Converts to string for logging
     * 
     * @return std::string Short error description
     */
    std::string toString() const {
        return "[" + ::nlm::toString(category_) + "] " + message_;
    }

private:
    ErrorCategory category_;
    std::string message_;
    std::string context_;
    std::string suggestion_;
    int errorCode_;
    std::chrono::system_clock::time_point timestamp_;
};

/**
 * @class Result
 * @brief Template for operation results with error handling
 * 
 * The Result<T> pattern provides type-safe error handling where operations
 * either succeed (returning a value of type T) or fail (returning an error).
 * This eliminates the need for exception handling in many cases and provides
 * detailed error information for debugging.
 * 
 * @tparam T Type of value returned on success
 * 
 * @code
 * Result<NeuronId> createNeuron(int type) {
 *     if (type == 0) return NeuronId(1);
 *     if (type == 1) return NeuronId(2);
 *     return ErrorInfo(ErrorCategory::ValidationError, 
 *                     "Invalid neuron type",
 *                     "Neuron creation",
 *                     "Use type 0 or 1");
 * }
 * 
 * auto result = createNeuron(2);
 * if (result.isSuccess()) {
 *     NeuronId id = result.value();
 * } else {
 *     // Handle error
 *     auto error = result.error();
 *     std::cout << error.getReport();
 * }
 * @endcode
 */
template<typename T>
class Result {
public:
    /**
     * @brief Constructs successful result
     * 
     * @tparam Args Types of constructor arguments
     * @param args Arguments to construct value from
     */
    template<typename... Args>
    Result(Args&&... args) : value_(std::forward<Args>(args)...), error_(std::nullopt) {}
    
    /**
     * @brief Constructs error result
     * 
     * @param error Error information
     */
    Result(const ErrorInfo& error) : value_(std::nullopt), error_(error) {}
    
    /**
     * @brief Checks if result represents success
     * 
     * @return bool true if operation succeeded
     */
    bool isSuccess() const { return error_.has_value(); }
    
    /**
     * @brief Checks if result represents failure
     * 
     * @return bool true if operation failed
     */
    bool isFailure() const { return !error_.has_value(); }
    
    /**
     * @brief Gets value if successful, throws if error
     * 
     * @return T Value from successful operation
     * @throws std::runtime_error if result contains error
     */
    T value() const {
        if (error_.has_value()) {
            throw std::runtime_error(error_->getReport());
        }
        return value_.value();
    }
    
    /**
     * @brief Gets value if successful, returns default if error
     * 
     * @tparam Default Default value type
     * @param defaultValue Default value to return on error
     * @return T Value or default
     */
    template<typename Default>
    T valueOr(Default&& defaultValue) const {
        if (error_.has_value()) {
            return std::forward<Default>(defaultValue);
        }
        return value_.value();
    }
    
    /**
     * @brief Gets error information if failed
     * 
     * @return const ErrorInfo& Error information
     * @throws std::runtime_error if result is successful
     */
    const ErrorInfo& error() const {
        if (!error_.has_value()) {
            throw std::runtime_error("No error in successful result");
        }
        return error_.value();
    }
    
    /**
     * @brief Gets error information if failed, returns default if successful
     * 
     * @tparam Default Default error type
     * @param defaultError Default error to return on success
     * @return const ErrorInfo& Error or default
     */
    template<typename Default>
    const ErrorInfo& errorOr(Default&& defaultError) const {
        if (error_.has_value()) {
            return error_.value();
        }
        return std::forward<Default>(defaultError);
    }
    
    /**
     * @brief Gets value pointer if successful
     * 
     * @return T* Pointer to value, or nullptr if error
     */
    T* valuePtr() {
        if (error_.has_value()) {
            return nullptr;
        }
        return &value_.value();
    }
    
    /**
     * @brief Gets const value pointer if successful
     * 
     * @return const T* Const pointer to value, or nullptr if error
     */
    const T* valuePtr() const {
        if (error_.has_value()) {
            return nullptr;
        }
        return &value_.value();
    }
    
    /**
     * @brief Converts to bool for success checking
     * 
     * @return bool true if successful
     */
    explicit operator bool() const { return isSuccess(); }
    
    /**
     * @brief Logical AND with another Result
     * 
     * @tparam U Type of other result's value
     * @param other Other result to AND with
     * @return Result<bool> AND of success flags
     */
    template<typename U>
    Result<bool> and(const Result<U>& other) const {
        return isSuccess() && other.isSuccess();
    }
    
    /**
     * @brief Logical OR with another Result
     * 
     * @tparam U Type of other result's value
     * @param other Other result to OR with
     * @return Result<bool> OR of success flags
     */
    template<typename U>
    Result<bool> or(const Result<U>& other) const {
        return isSuccess() || other.isSuccess();
    }
    
    /**
     * @brief Maps value transformation if successful
     * 
     * @tparam F Function type
     * @tparam U Result type of function
     * @param f Function to apply to value
     * @return Result<U> Transformed result
     */
    template<typename F>
    Result<decltype(std::declval<F>()(std::declval<T>()))> 
    map(F&& f) const {
        if (error_.has_value()) {
            return Result<decltype(f(value_.value()))>(error_.value());
        }
        return Result<decltype(f(value_.value()))>(f(value_.value()));
    }
    
    /**
     * @brief Binds operation with another Result
     * 
     * @tparam F Function type
     * @tparam U Result type of other result
     * @param f Function taking value from this and value from other
     * @param other Other result to bind with
     * @return Result<decltype(f(std::declval<T>(), std::declval<U>()))> Bound result
     */
    template<typename F, typename U>
    Result<decltype(std::declval<F>()(std::declval<T>(), std::declval<U>()))>
    andThen(F&& f, const Result<U>& other) const {
        if (error_.has_value() || other.error_.has_value()) {
            return Result<decltype(f(value_.value(), other.value_.value()))>(error_.has_value() ? error_.value() : other.error_.value());
        }
        return Result<decltype(f(value_.value(), other.value_.value()))>(f(value_.value(), other.value_.value()));
    }
    
    /**
     * @brief Gets underlying error if present
     * 
     * @return const std::optional<ErrorInfo>& Underlying error
     */
    const std::optional<ErrorInfo>& getError() const { return error_; }
    
    /**
     * @brief Checks if result contains specific error category
     * 
     * @param category Error category to check
     * @return bool true if error has specified category
     */
    bool hasCategory(ErrorCategory category) const {
        return error_.has_value() && error_->getCategory() == category;
    }

private:
    std::optional<T> value_;
    std::optional<ErrorInfo> error_;
};

/**
 * @brief Creates successful result from value
 * 
 * @tparam T Type of value
 * @tparam Args Types of constructor arguments
 * @param args Arguments to construct value from
 * @return Result<T> Successful result
 */
template<typename T, typename... Args>
Result<T> Success(Args&&... args) {
    return Result<T>(std::forward<Args>(args)...);
}

/**
 * @brief Creates error result from ErrorInfo
 * 
 * @param error Error information
 * @return Result<T> Error result (type deduced from context)
 */
template<typename T>
Result<T> Failure(const ErrorInfo& error) {
    return Result<T>(error);
}

/**
 * @brief Creates error result from error components
 * 
 * @param category Error category
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @param errorCode Optional system error code
 * @return Result<T> Error result
 */
template<typename T>
Result<T> Failure(ErrorCategory category,
                  const std::string& message,
                  const std::string& context = "",
                  const std::string& suggestion = "",
                  int errorCode = 0) {
    return Result<T>(ErrorInfo(category, message, context, suggestion, errorCode));
}

/**
 * @brief Creates error result for configuration errors
 * 
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @return Result<T> Configuration error
 */
template<typename T>
Result<T> ConfigFailure(const std::string& message,
                        const std::string& context = "",
                        const std::string& suggestion = "") {
    return Failure<T>(ErrorCategory::ConfigError, message, context, suggestion);
}

/**
 * @brief Creates error result for initialization errors
 * 
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @return Result<T> Initialization error
 */
template<typename T>
Result<T> InitFailure(const std::string& message,
                      const std::string& context = "",
                      const std::string& suggestion = "") {
    return Failure<T>(ErrorCategory::InitializationError, message, context, suggestion);
}

/**
 * @brief Creates error result for memory allocation errors
 * 
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @return Result<T> Memory error
 */
template<typename T>
Result<T> MemoryFailure(const std::string& message,
                        const std::string& context = "",
                        const std::string& suggestion = "") {
    return Failure<T>(ErrorCategory::MemoryAllocationError, message, context, suggestion);
}

/**
 * @brief Creates error result for validation errors
 * 
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @return Result<T> Validation error
 */
template<typename T>
Result<T> ValidationFailure(const std::string& message,
                            const std::string& context = "",
                            const std::string& suggestion = "") {
    return Failure<T>(ErrorCategory::ValidationError, message, context, suggestion);
}

/**
 * @brief Creates error result for file errors
 * 
 * @param message Error message
 * @param context Optional context
 * @param suggestion Optional suggestion
 * @return Result<T> File error
 */
template<typename T>
Result<T> FileFailure(const std::string& message,
                      const std::string& context = "",
                      const std::string& suggestion = "") {
    return Failure<T>(ErrorCategory::FileError, message, context, suggestion);
}

} // namespace nlm

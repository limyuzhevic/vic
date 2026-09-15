// Error handling utilities for NLM (熙然)
// Phase 2: Real Neural Computation

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <functional>

namespace nlm {

// Error categories for NLM
enum class ErrorCategory {
    None,
    Configuration,
    Initialization,
    Memory,
    I/O,
    Validation,
    Plasticity,
    Neuromodulation,
    Simulation,
    Checkpoint,
    Unknown
};

// Exception class for NLM errors
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message, 
                  ErrorCategory category = ErrorCategory::Unknown,
                  const std::string& context = "")
        : std::runtime_error(formatMessage(message, category, context)),
          category_(category),
          context_(context),
          timestamp_(std::chrono::system_clock::now()) {}
    
    // Accessors
    ErrorCategory getCategory() const { return category_; }
    const std::string& getContext() const { return context_; }
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp_; }
    
    // Create NLM exceptions with specific categories
    static NLMException ConfigurationError(const std::string& message, const std::string& context = "");
    static NLMException InitializationError(const std::string& message, const std::string& context = "");
    static NLMException MemoryError(const std::string& message, const std::string& context = "");
    static NLMException IOError(const std::string& message, const std::string& context = "");
    static NLMException ValidationError(const std::string& message, const std::string& context = "");
    static NLMException PlasticityError(const std::string& message, const std::string& context = "");
    static NLMException NeuromodulationError(const std::string& message, const std::string& context = "");
    static NLMException SimulationError(const std::string& message, const std::string& context = "");
    static NLMException CheckpointError(const std::string& message, const std::string& context = "");
    
    // Chain exceptions
    NLMException& addContext(const std::string& context) {
        context_ = context + " -> " + context_;
        return *this;
    }
    
    // Convert to log level for logging
    static std::string categoryToString(ErrorCategory category);
    
private:
    std::string formatMessage(const std::string& message, ErrorCategory category, const std::string& context);
    
    ErrorCategory category_;
    std::string context_;
    std::chrono::system_clock::time_point timestamp_;
};

// Result type for operations that can fail
template<typename T>
class Result {
public:
    Result(T value) : value_(std::move(value)), error_(std::nullopt) {}
    Result(const NLMException& error) : error_(error) {}
    
    bool success() const { return !error_.has_value(); }
    bool failure() const { return error_.has_value(); }
    
    const T& value() const {
        if (error_.has_value()) {
            throw *error_.value();
        }
        return value_;
    }
    
    const T& value() {  // const overload
        return const_cast<const T&>(const_cast<Result*>(this)->value());
    }
    
    const NLMException& error() const { return *error_.value(); }
    
    // Chain operations
    template<typename F>
    auto andThen(F&& f) const {
        if (error_.has_value()) {
            return Result<F>(error_.value());
        }
        try {
            auto result = f(value_);
            return Result<typename std::decay<decltype(result)>::type>(std::move(result));
        } catch (const NLMException& e) {
            return Result<typename std::decay<decltype(result)>::type>(e);
        } catch (const std::exception& e) {
            return Result<typename std::decay<decltype(result)>::type>(NLMException(e.what(), ErrorCategory::Unknown));
        }
    }
    
    template<typename F>
    auto map(F&& f) const {
        return andThen([f](const T& val) { return f(val); });
    }
    
    // Short-hand constructors
    static Result success(T value) { return Result(std::move(value)); }
    static Result failure(const NLMException& error) { return Result(error); }
    static Result failure(ErrorCategory category, const std::string& message) {
        return Result(NLMException(message, category));
    }
    
private:
    T value_;
    std::optional<NLMException> error_;
};

// Error handling utilities for common operations
namespace ErrorHandling {
    // Try-catch wrapper for NLM operations
    template<typename F>
    Result<typename std::decay<decltype(std::declval<F>()())>::type> 
    safeExecute(F&& f, const std::string& context = "") {
        try {
            return Result<typename std::decay<decltype(std::declval<F>()())>::type>(f());
        } catch (const NLMException& e) {
            if (!context.empty()) {
                return Result<typename std::decay<decltype(std::declval<F>()())>::type>(e.addContext(context));
            }
            return Result<typename std::decay<decltype(std::declval<F>()())>::type>(e);
        } catch (const std::exception& e) {
            return Result<typename std::decay<decltype(std::declval<F>()())>::type>(
                NLMException(std::string("Unexpected error: ") + e.what(), ErrorCategory::Unknown, context)
            );
        }
    }
    
    // Validate parameter ranges
    template<typename T>
    bool validateRange(const T& value, const T& min, const T& max, const std::string& paramName = "") {
        if (value < min || value > max) {
            NLM_LOG_ERROR("Invalid range for parameter '" + paramName + "': " + 
                         std::to_string(value) + " not in [" + 
                         std::to_string(min) + ", " + std::to_string(max) + "]");
            return false;
        }
        return true;
    }
    
    // Validate pointer is not null
    template<typename T>
    bool validateNotNull(T* ptr, const std::string& paramName = "") {
        if (!ptr) {
            NLM_LOG_ERROR("Null pointer for parameter '" + paramName + "'");
            return false;
        }
        return true;
    }
    
    // Validate container is not empty
    template<typename Container>
    bool validateNotEmpty(const Container& container, const std::string& paramName = "") {
        if (container.empty()) {
            NLM_LOG_ERROR("Empty container for parameter '" + paramName + "'");
            return false;
        }
        return true;
    }
    
    // Validate condition
    bool validateCondition(bool condition, const std::string& errorMessage, const std::string& context = "");
}

// Convenience error macros for NLM
#define NLM_THROW(category, message) throw NLMException::category##Error(message, std::string(#category) + ": " + __func__)
#define NLM_VALIDATE(condition, category, message) \
    if (!(condition)) { throw NLMException::category##Error(message, std::string(#category) + ": " + __func__); }
#define NLM_TRY_CATCH(operation, category, context) \
    try { operation; } catch (const std::exception& e) { \
        if (!context.empty()) { \
            throw NLMException::category##Error(std::string("Operation failed: ") + e.what(), ErrorCategory::category, context); \
        } else { \
            throw; \
        } \
    }

// Helper functions for creating specific error types
inline NLMException NLMException::ConfigurationError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Configuration, context);
}

inline NLMException NLMException::InitializationError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Initialization, context);
}

inline NLMException NLMException::MemoryError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Memory, context);
}

inline NLMException NLMException::IOError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::IO, context);
}

inline NLMException NLMException::ValidationError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Validation, context);
}

inline NLMException NLMException::PlasticityError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Plasticity, context);
}

inline NLMException NLMException::NeuromodulationError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Neuromodulation, context);
}

inline NLMException NLMException::SimulationError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Simulation, context);
}

inline NLMException NLMException::CheckpointError(const std::string& message, const std::string& context) {
    return NLMException(message, ErrorCategory::Checkpoint, context);
}

inline std::string NLMException::categoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::None: return "NONE";
        case ErrorCategory::Configuration: return "CONFIG";
        case ErrorCategory::Initialization: return "INIT";
        case ErrorCategory::Memory: return "MEMORY";
        case ErrorCategory::I/O: return "IO";
        case ErrorCategory::Validation: return "VALIDATION";
        case ErrorCategory::Plasticity: return "PLASTICITY";
        case ErrorCategory::Neuromodulation: return "NEUROMOD";
        case ErrorCategory::Simulation: return "SIMULATION";
        case ErrorCategory::Checkpoint: return "CHECKPOINT";
        case ErrorCategory::Unknown: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

inline std::string NLMException::formatMessage(const std::string& message, ErrorCategory category, const std::string& context) {
    std::ostringstream oss;
    oss << "[" << categoryToString(category) << "] ";
    if (!context.empty()) {
        oss << context << " -> ";
    }
    oss << message;
    return oss.str();
}

inline bool ErrorHandling::validateCondition(bool condition, const std::string& errorMessage, const std::string& context) {
    if (!condition) {
        NLM_LOG_ERROR("Validation failed: " + errorMessage + 
                     (context.empty() ? "" : (" in context: " + context)));
        return false;
    }
    return true;
}

} // namespace nlm

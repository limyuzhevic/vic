#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <system_error>

namespace nlm {

// Error categories for NLM
enum class NLMErrorCategory {
    System,           // Core system errors (memory, file I/O, etc.)
    Configuration,    // Configuration and parameter errors
    Memory,           // Memory allocation and management errors
    Simulation,       // Simulation runtime errors
    Development,      // Developmental stage errors
    Integration,      // Integration and connection errors
    User,             // User input and usage errors
    Network           // Network or communication errors
};

// Enhanced error base class
class NLMError : public std::runtime_error {
protected:
    NLMErrorCategory category_;
    std::string context_;
    int errorCode_;
    
public:
    NLMError(const std::string& message, NLMErrorCategory category, 
             const std::string& context = "", int errorCode = 0)
        : std::runtime_error(message), category_(category), 
          context_(context), errorCode_(errorCode) {}
    
    virtual ~NLMError() = default;
    
    NLMErrorCategory category() const noexcept { return category_; }
    const std::string& context() const noexcept { return context_; }
    int errorCode() const noexcept { return errorCode_; }
    
    virtual const char* categoryName() const noexcept {
        switch (category_) {
            case NLMErrorCategory::System: return "System";
            case NLMErrorCategory::Configuration: return "Configuration";
            case NLMErrorCategory::Memory: return "Memory";
            case NLMErrorCategory::Simulation: return "Simulation";
            case NLMErrorCategory::Development: return "Development";
            case NLMErrorCategory::Integration: return "Integration";
            case NLMErrorCategory::User: return "User";
            case NLMErrorCategory::Network: return "Network";
            default: return "Unknown";
        }
    }
    
    // Convert error to string representation
    std::string whatWithCategory() const {
        std::ostringstream oss;
        oss << "[" << categoryName() << ":" << errorCode_ << "] " << what();
        if (!context_.empty()) {
            oss << " (Context: " << context_ << ")";
        }
        return oss.str();
    }
};

// Specific error types
class NLMConfigurationError : public NLMError {
public:
    NLMConfigurationError(const std::string& message, 
                         const std::string& context = "",
                         int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Configuration, context, errorCode) {}
};

class NLMMemoryError : public NLMError {
public:
    NLMMemoryError(const std::string& message,
                  const std::string& context = "",
                  int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Memory, context, errorCode) {}
};

class NLMSimulationError : public NLMError {
public:
    NLMSimulationError(const std::string& message,
                      const std::string& context = "",
                      int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Simulation, context, errorCode) {}
};

class NLMDevelopmentError : public NLMError {
public:
    NLMDevelopmentError(const std::string& message,
                       const std::string& context = "",
                       int errorCode = 0)
        : NLMError(message, NLMErrorCategory::Development, context, errorCode) {}
};

class NLMUserError : public NLMError {
public:
    NLMUserError(const std::string& message,
                const std::string& context = "",
                int errorCode = 0)
        : NLMError(message, NLMErrorCategory::User, context, errorCode) {}
};

// Error handling utilities
namespace error_handling {

// Safe wrapper for memory allocation with error handling
template<typename T>
std::unique_ptr<T> safe_make_unique(const std::string& context = "") {
    try {
        return std::make_unique<T>();
    } catch (const std::bad_alloc& e) {
        throw NLMMemoryError("Memory allocation failed", context, 1);
    }
}

// Safe wrapper for vector allocation with error handling
template<typename T>
std::vector<T> safe_vector(const std::string& context = "") {
    try {
        return std::vector<T>();
    } catch (const std::bad_alloc& e) {
        throw NLMMemoryError("Memory allocation failed", context, 2);
    }
}

// Validate pointer and throw error if null
inline void validateNonNull(const void* ptr, const std::string& message, 
                           const std::string& context = "", int errorCode = 0) {
    if (ptr == nullptr) {
        throw NLMUserError(message, context, errorCode);
    }
}

// Check condition and throw error if not met
inline void validateCondition(bool condition, const std::string& message,
                               const std::string& context = "",
                               int errorCode = 0) {
    if (!condition) {
        throw NLMUserError(message, context, errorCode);
    }
}

// Convert standard exceptions to NLM errors
inline void convertStandardException(const std::string& context = "") {
    try {
        throw;
    } catch (const std::bad_alloc& e) {
        throw NLMMemoryError("Memory allocation failed", context, 1);
    } catch (const std::invalid_argument& e) {
        throw NLMUserError("Invalid argument", context, 2);
    } catch (const std::out_of_range& e) {
        throw NLMUserError("Out of range", context, 3);
    } catch (...) {
        throw NLMError("Unknown error occurred", NLMErrorCategory::System, context, 0);
    }
}

} // namespace error_handling

} // namespace nlm

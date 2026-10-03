// ErrorHandling.hpp - Simple error handling implementation
#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <cassert>
#include <stdarg.h>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// Base exception class
class NLMError : public std::runtime_error {
public:
    enum class ErrorType {
        None = 0,
        ConfigError = 1,
        ValidationError = 2,
        MemoryError = 3,
        BrainError = 4,
        PlasticityError = 5,
        SimulationError = 6,
        SystemError = 7
    };
    
    NLMError(ErrorType type, const std::string& message)
        : std::runtime_error(message), error_type(type) {}
    
    ErrorType getType() const { return error_type; }
    
private:
    ErrorType error_type;
};

// Validation utilities
namespace Validation {
    
    template<typename T>
    inline void validatePositive(const T& value, const std::string& paramName,
                                 const std::string& file = "", int line = 0) {
        if (!(value > 0)) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" must be positive, got: " + std::to_string(value));
        }
    }
    
    template<typename T>
    inline void validateNonNegative(const T& value, const std::string& paramName,
                                    const std::string& file = "", int line = 0) {
        if (!(value >= 0)) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" must be non-negative, got: " + std::to_string(value));
        }
    }
    
    template<typename T>
    inline void validateRange(const T& value, const T& min, const T& max,
                              const std::string& paramName,
                              const std::string& file = "", int line = 0) {
        if (!(value >= min && value <= max)) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" must be in range [" +
                          std::to_string(min) + ", " + std::to_string(max) +
                          "], got: " + std::to_string(value));
        }
    }
    
    inline void validateNotEmpty(const std::string& value, const std::string& paramName,
                                const std::string& file = "", int line = 0) {
        if (value.empty()) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" cannot be empty");
        }
    }
    
    inline void validateAlphanumeric(const std::string& value, const std::string& paramName,
                                    const std::string& file = "", int line = 0) {
        for (char c : value) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                throw NLMError(NLMError::ErrorType::ValidationError,
                              "Parameter \"" + paramName + "\" contains invalid characters");
            }
        }
    }
    
    inline void validateArrayBounds(size_t index, size_t size,
                                   const std::string& paramName,
                                   const std::string& file = "", int line = 0) {
        if (index >= size) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Array index out of bounds for parameter \"" + paramName + "\"");
        }
    }
    
    inline void validateSchema(const std::string& key, const std::string& type,
                              const std::string& file = "", int line = 0) {
        if (key.empty()) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Schema key cannot be empty");
        }
        
        if (type.empty()) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Schema type cannot be empty for key: " + key);
        }
        
        // Validate type string
        const std::string allowedTypes[] = {"int", "int64_t", "double", "bool", "string",
                                            "vector<int>", "vector<double>", "vector<string>", "map"};
        bool typeValid = false;
        for (const auto& allowed : allowedTypes) {
            if (type == allowed) {
                typeValid = true;
                break;
            }
        }
        
        if (!typeValid) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Invalid schema type \"" + type + "\"");
        }
    }
}

// Error handling utilities
inline void logAndRethrow(const std::exception& e, const std::string& context = "") {
    // Simple logging - in real implementation would use actual logging
    throw; // Re-throw the exception
}

inline bool tryCatchRethrow(const std::function<void()>& operation,
                           const std::string& context = "") {
    try {
        operation();
        return true;
    } catch (const std::exception& e) {
        logAndRethrow(e, context);
        return false;
    }
}

// State recovery utilities
class StateRestorer {
private:
    std::vector<std::string> savedStates;
    
public:
    StateRestorer() = default;
    
    template<typename T>
    void saveState(const T& state, const std::string& key) {
        savedStates.push_back(key);
    }
    
    template<typename T>
    void restoreState(const T& state, const std::string& key) {
        auto it = std::find(savedStates.begin(), savedStates.end(), key);
        if (it != savedStates.end()) {
            savedStates.erase(it);
        }
    }
    
    bool hasState(const std::string& key) const {
        return std::find(savedStates.begin(), savedStates.end(), key) != savedStates.end();
    }
    
    void clear() {
        savedStates.clear();
    }
};

// Error handling macros
#define NLM_VALIDATE(condition, error) \
    do { \
        if (!(condition)) { \
            throw error; \
        } \
    } while(0)

#define NLM_TRY_CATCH(operation, error) \
    try { \
        operation; \
    } catch (...) { \
        throw error; \
    }

// Convenience macros for error checking
#define NLM_CHECK_POINTER(ptr, error) NLM_VALIDATE(ptr != nullptr, error)
#define NLM_CHECK_RANGE(idx, size, error) NLM_VALIDATE(idx < size, error)

} // namespace nlm

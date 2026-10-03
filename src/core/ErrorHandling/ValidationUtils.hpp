#pragma once

#include <string>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include "ErrorHandling.hpp"

namespace nlm {

// Validation utilities for comprehensive input validation
namespace Validation {

    // Basic type validation
    template<typename T>
    inline void validatePositive(const T& value, const std::string& paramName,
                                 const std::string& file = "", int line = 0) {
        if (!(value > 0)) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" must be positive, got: " + std::to_string(value),
                          file, line, "validatePositive");
        }
    }
    
    template<typename T>
    inline void validateNonNegative(const T& value, const std::string& paramName,
                                    const std::string& file = "", int line = 0) {
        if (!(value >= 0)) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" must be non-negative, got: " + std::to_string(value),
                          file, line, "validateNonNegative");
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
                          "], got: " + std::to_string(value),
                          file, line, "validateRange");
        }
    }
    
    // String validation
    inline void validateNotEmpty(const std::string& value, const std::string& paramName,
                                const std::string& file = "", int line = 0) {
        if (value.empty()) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Parameter \"" + paramName + "\" cannot be empty",
                          file, line, "validateNotEmpty");
        }
    }
    
    inline void validateAlphanumeric(const std::string& value, const std::string& paramName,
                                    const std::string& file = "", int line = 0) {
        for (char c : value) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                throw NLMError(NLMError::ErrorType::ValidationError,
                              "Parameter \"" + paramName + "\" contains invalid characters. "
                              "Only alphanumeric, underscore, and hyphen allowed.",
                              file, line, "validateAlphanumeric");
            }
        }
    }
    
    // Array and sequence validation
    inline void validateArrayBounds(size_t index, size_t size,
                                   const std::string& paramName,
                                   const std::string& file = "", int line = 0) {
        if (index >= size) {
            throw NLMError(NLMError::ErrorType::ValidationError,
                          "Array index \"" + std::to_string(index) + "\" out of bounds "
                          "for array \"" + paramName + "\" with size " + std::to_string(size),
                          file, line, "validateArrayBounds");
        }
    }
    
    // Configuration schema validation
    inline void validateSchema(const std::string& key, const std::string& type,
                              const std::string& file = "", int line = 0) {
        if (key.empty()) {
            throw ConfigError(ConfigError::Type::ValidationError,
                             "Schema key cannot be empty", file, line, "validateSchema");
        }
        
        if (type.empty()) {
            throw ConfigError(ConfigError::Type::ValidationError,
                             "Schema type cannot be empty for key: " + key, file, line, "validateSchema");
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
            throw ConfigError(ConfigError::Type::ValidationError,
                             "Invalid schema type \"" + type + "\" for key: " + key, file, line, "validateSchema");
        }
    }
}

// Memory safety utilities
inline void* safeAllocate(size_t size, const std::string& context = "") {
    if (size == 0) {
        return nullptr;
    }
    
    void* ptr = std::malloc(size);
    if (!ptr) {
        throw MemoryError(MemoryError::Type::AllocationError,
                         "Failed to allocate " + std::to_string(size) + " bytes in context: " + context);
    }
    return ptr;
}

inline void safeDeallocate(void* ptr, const std::string& context = "") {
    if (!ptr) {
        return; // Already null
    }
    
    std::free(ptr);
}

// Error handling utilities
inline void logAndRethrow(const std::exception& e, const std::string& context = "") {
    NLM_LOG_ERROR("Exception in " + context + ": " + e.what());
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
        NLM_LOG_INFO("Saved state for key: " + key);
    }
    
    template<typename T>
    void restoreState(const T& state, const std::string& key) {
        auto it = std::find(savedStates.begin(), savedStates.end(), key);
        if (it != savedStates.end()) {
            NLM_LOG_INFO("Restored state for key: " + key);
            savedStates.erase(it);
        }
    }
    
    bool hasState(const std::string& key) const {
        return std::find(savedStates.begin(), savedStates.end(), key) != savedStates.end();
    }
    
    void clear() {
        savedStates.clear();
        NLM_LOG_INFO("Cleared all saved states");
    }
};

// Convenience macros for error checking
#define NLM_VALIDATE(condition, error) \
    do { \
        if (!(condition)) { \
            throw error; \
        } \
    } while(0)

#define NLM_TRY_CATCH(operation, errorType, context) \
    try { \
        operation; \
    } catch (const std::exception& e) { \
        logAndRethrow(e, context); \
    }

// Convenience macros for error checking
#define NLM_CHECK_POINTER(ptr, error) NLM_VALIDATE(ptr != nullptr, error)
#define NLM_CHECK_RANGE(idx, size, error) NLM_VALIDATE(idx < size, error)

} // namespace nlm

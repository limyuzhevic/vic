#pragma once

/**
 * Error handling and validation utilities for NLM
 */

#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

class NLMError : public std::runtime_error {
public:
    enum class ErrorCode {
        INVALID_PARAMETER = 1,
        FILE_NOT_FOUND = 2,
        INITIALIZATION_FAILED = 3,
        VALIDATION_FAILED = 4,
        OUT_OF_MEMORY = 5,
        CONFIGURATION_ERROR = 6,
        NETWORK_ERROR = 7
    };
    
    NLMError(ErrorCode code, const std::string& message)
        : std::runtime_error(message), code(code) {}
    
    ErrorCode getCode() const { return code; }
    
private:
    ErrorCode code;
};

class ValidationUtils {
public:
    template<typename T>
    static bool isValid(const T& value, T min, T max) {
        return value >= min && value <= max;
    }
    
    static bool isValid(const std::string& str, size_t minLength, size_t maxLength) {
        return str.length() >= minLength && str.length() <= maxLength;
    }
    
    static bool isValid(const std::vector<float>& vec, float minValue, float maxValue) {
        return std::all_of(vec.begin(), vec.end(), 
            [minValue, maxValue](float v) { return v >= minValue && v <= maxValue; });
    }
    
    static bool isValidPointer(void* ptr) {
        return ptr != nullptr;
    }
    
    static std::string validateConfig(const std::string& key, const std::string& value) {
        if (key.empty()) {
            throw NLMError(NLMError::ErrorCode::INVALID_PARAMETER, "Configuration key cannot be empty");
        }
        
        if (value.empty()) {
            throw NLMError(NLMError::ErrorCode::INVALID_PARAMETER, "Configuration value for key '" + key + "' cannot be empty");
        }
        
        return "";
    }
};

class ErrorHandler {
public:
    static void handleError(const std::string& operation, const std::string& details = "") {
        std::cerr << "[ERROR] " << operation << ": " << details << std::endl;
        throw NLMError(NLMError::ErrorCode::CONFIGURATION_ERROR, "Error in " + operation + ": " + details);
    }
    
    static void validateFileExists(const std::string& filepath) {
        // Note: This would need access to file system operations
        // For now, just check if the path is not empty
        if (filepath.empty()) {
            handleError("File validation", "File path is empty");
        }
    }
    
    static void validateRange(float value, float min, float max, const std::string& paramName) {
        if (std::isnan(value) || std::isinf(value)) {
            handleError("Parameter validation", paramName + " is not a valid number");
        }
        
        if (value < min || value > max) {
            handleError("Parameter validation", paramName + " " + std::to_string(value) + 
                       " is outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }
};

class SafeMemory {
public:
    static void checkMemoryAvailable(size_t requiredBytes) {
        // Note: This would need platform-specific memory checking
        // For now, provide a placeholder
        if (requiredBytes > 1024 * 1024 * 1024) {  // 1GB threshold
            throw NLMError(NLMError::ErrorCode::OUT_OF_MEMORY, 
                          "Requested " + std::to_string(requiredBytes) + " bytes exceeds available memory");
        }
    }
    
    template<typename T>
    static std::unique_ptr<T> allocateSafe(size_t count) {
        checkMemoryAvailable(count * sizeof(T));
        return std::make_unique<T>(count);
    }
};

} // namespace nlm

// NLM Error Handling and Validation
// Provides robust error handling and validation utilities for NLM

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <stdexcept>
#include <vector>

namespace nlm {

// NLM Error types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("Configuration Error: " + message) {}
};

class InitializationError : public NLMError {
public:
    explicit InitializationError(const std::string& message) : NLMError("Initialization Error: " + message) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& message) : NLMError("Validation Error: " + message) {}
};

// Validation utilities
class Validator {
public:
    template<typename T>
    static bool isValid(const T& value, const T& min, const T& max) {
        return value >= min && value <= max;
    }
    
    static bool isPositive(int64_t value) { return value > 0; }
    static bool isNonNegative(int64_t value) { return value >= 0; }
    static bool isPositive(float value) { return value > 0.0f; }
    static bool isNonNegative(float value) { return value >= 0.0f; }
    static bool isInRange(float value, float min, float max) { return value >= min && value <= max; }
    static bool isNonEmpty(const std::string& str) { return !str.empty(); }
    static bool isNonEmpty(const std::vector<float>& vec) { return !vec.empty(); }
    
    static std::vector<std::string> validateConfig(const std::vector<std::string>& requiredKeys, 
                                                   const std::vector<std::string>& actualKeys) {
        std::vector<std::string> missingKeys;
        for (const auto& key : requiredKeys) {
            if (std::find(actualKeys.begin(), actualKeys.end(), key) == actualKeys.end()) {
                missingKeys.push_back(key);
            }
        }
        return missingKeys;
    }
};

// Error reporting utilities
class ErrorReporter {
public:
    static void logError(const std::string& error, bool throwException = true) {
        // Log error using existing logging system
        // For now, output to stderr
        std::cerr << "NLM ERROR: " << error << std::endl;
        if (throwException) {
            throw NLMError(error);
        }
    }
    
    static void logWarning(const std::string& warning) {
        std::cerr << "NLM WARNING: " << warning << std::endl;
    }
    
    static void logInfo(const std::string& info) {
        std::cout << "NLM INFO: " << info << std::endl;
    }
};

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP

#include "Exceptions.hpp"
#include <memory>
#include <string>

namespace nlm {

// Exception factory initialization
std::map<std::string, std::function<std::exception_ptr()>> ExceptionManager::exceptionFactories_;

// Template implementation for validation functions
template<typename T>
T validateParameter(const T& value, const std::string& paramName,
                   const std::function<bool(const T&)>& validator,
                   const std::string& errorMessage = "Invalid parameter value") {
    if (!validator(value)) {
        throw std::invalid_argument(errorMessage + " for parameter: " + paramName);
    }
    return value;
}

// String validation utilities
class StringValidator {
public:
    static bool isValidKey(const std::string& key) {
        return !key.empty() && key.find('=') == std::string::npos && 
               key.find(' ') == std::string::npos &&
               key.find('\n') == std::string::npos &&
               key.find('\t') == std::string::npos;
    }
    
    static bool isValidFilePath(const std::string& path) {
        return !path.empty() && path.find('\0') == std::string::npos;
    }
    
    static bool isValidConfigValue(const std::variant<int, int64_t, double, bool, std::string,
                                    std::vector<int>, std::vector<double>, std::vector<std::string>>& value) {
        // Check for null/empty values where applicable
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Strings can be empty
                return true;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                // Vectors can be empty
                return true;
            } else {
                return true; // Other types are fine
            }
        }, value);
        return true;
    }
};

// Validation helper functions
class ValidationUtils {
public:
    static bool validateRange(double value, double min, double max, bool inclusive = true) {
        return inclusive ? (value >= min && value <= max) : (value > min && value < max);
    }
    
    static bool validatePositive(int64_t value) {
        return value > 0;
    }
    
    static bool validateNonNegative(double value) {
        return value >= 0.0;
    }
    
    static bool validateProbability(double value) {
        return value >= 0.0 && value <= 1.0;
    }
    
    static bool validateArrayBounds(size_t index, size_t size) {
        return index < size;
    }
};

// State checker utilities
class StateValidator {
public:
    static bool isBrainInitialized(const class Brain* brain) {
        return brain != nullptr && brain->getConfig() != nullptr;
    }
    
    static bool isSystemReady(const class Brain* brain) {
        return isBrainInitialized(brain) && brain->getRegionCount() > 0;
    }
};

} // namespace nlm
#include "ConfigurationValidator.hpp"
#include <sstream>

namespace nlm {

// Helper implementation for ConfigValue to string conversion
std::string ConfigurationValidator::valueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return vectorToString(std::get<std::vector<int>>(value));
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return vectorToString(std::get<std::vector<double>>(value));
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return vectorToString(std::get<std::vector<std::string>>(value));
    }
    return "<unknown>";
}

// Vector to string conversion helper implementations
template<typename T>
std::string ConfigurationValidator::vectorToString(const std::vector<T>& vec) {
    std::string result = "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) result += ", ";
        result += std::to_string(vec[i]);
    }
    result += "]";
    return result;
}

// Validation errors with specific error types
ValidationError::ValidationError(const std::string& message, ValidationErrorType type)
    : std::runtime_error(message), errorType(type) {}

ValidationErrorType ValidationError::getErrorType() const {
    return errorType;
}

} // namespace nlm
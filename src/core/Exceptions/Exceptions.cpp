#include "core/Exceptions/Exceptions.hpp"

namespace nlm::ExceptionHelpers {

void validatePositive(const std::string& key, double value, double max) {
    if (value <= 0.0) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value must be positive, got " + std::to_string(value)
        );
    }
    if (value > max) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value exceeds maximum allowed " + std::to_string(max) + ", got " + std::to_string(value)
        );
    }
}

void validateNonNegative(const std::string& key, double value, double max) {
    if (value < 0.0) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value must be non-negative, got " + std::to_string(value)
        );
    }
    if (value > max) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value exceeds maximum allowed " + std::to_string(max) + ", got " + std::to_string(value)
        );
    }
}

void validateRange(const std::string& key, double value, double min, double max) {
    if (value < min) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value must be at least " + std::to_string(min) + ", got " + std::to_string(value)
        );
    }
    if (value > max) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value must be at most " + std::to_string(max) + ", got " + std::to_string(value)
        );
    }
}

void validateEnum(const std::string& key, int value, int max) {
    if (value < 0 || value > max) {
        throw ConfigValidationException(
            "Invalid configuration value: " + key,
            "Value must be between 0 and " + std::to_string(max) + ", got " + std::to_string(value)
        );
    }
}

std::string errorCodeToMessage(int errorCode) {
    switch (errorCode) {
        case 0: return "Success";
        case 1: return "Configuration error";
        case 2: return "File not found";
        case 3: return "Invalid file format";
        case 4: return "Brain initialization failed";
        case 5: return "Memory allocation failed";
        case 6: return "Invalid parameter";
        case 7: return "Operation not supported";
        case 8: return "Resource not available";
        default: return "Unknown error";
    }
}

std::string getSystemErrorMessage() {
    std::error_code ec(errno, std::system_category());
    return ec.message();
}

} // namespace nlm::ExceptionHelpers
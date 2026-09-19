#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <functional>
#include <typeinfo>
#include <sstream>
#include <cmath>

namespace nlm {

// Configuration Error Types

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message)
        : std::runtime_error(message) {}
};

class ValidationError : public ConfigError {
public:
    explicit ValidationError(const std::string& message)
        : ConfigError("Validation Error: " + message) {}
};

class TypeConversionError : public ConfigError {
public:
    explicit TypeConversionError(const std::string& message)
        : ConfigError("Type Conversion Error: " + message) {}
};

class SchemaError : public ConfigError {
public:
    explicit SchemaError(const std::string& message)
        : ConfigError("Schema Error: " + message) {}
};

class PluginConfigError : public ConfigError {
public:
    explicit PluginConfigError(const std::string& message)
        : ConfigError("Plugin Configuration Error: " + message) {}
};

// Validation Result Type
struct ValidationResult {
    bool isValid = true;
    std::string errorMessage;
    std::map<std::string, std::string> details;
    
    static ValidationResult success() {
        return ValidationResult{true, "", {}};
    }
    
    static ValidationResult failure(const std::string& message) {
        return ValidationResult{false, message, {}};
    }
    
    static ValidationResult failure(const std::string& message,
                                   const std::map<std::string, std::string>& details) {
        return ValidationResult{false, message, details};
    }
};

} // namespace nlm
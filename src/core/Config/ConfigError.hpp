#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message)
        : ConfigError("Configuration parse error: " + message) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message)
        : ConfigError("Configuration validation error: " + message) {}
};

class ConfigUnknownKeyError : public ConfigError {
public:
    explicit ConfigUnknownKeyError(const std::string& key, const std::string& message)
        : ConfigError("Unknown configuration key '" + key + "': " + message) {}
};

class ConfigTypeError : public ConfigError {
public:
    explicit ConfigTypeError(const std::string& key, const std::string& expectedType, const std::string& actualType)
        : ConfigError("Type mismatch for key '" + key + "': expected " + expectedType + ", got " + actualType) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& filepath, const std::string& message)
        : ConfigError("Configuration file error for '" + filepath + "': " + message) {}
};

} // namespace nlm
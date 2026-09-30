#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <cstdint>

namespace nlm {

class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error("Config error: " + message) {}
};


class ConfigFileError : public ConfigException {
public:
    explicit ConfigFileError(const std::string& message)
        : ConfigException("File error: " + message) {}
};

class ConfigValidationError : public ConfigException {
public:
    explicit ConfigValidationError(const std::string& message)
        : ConfigException("Validation error: " + message) {}
};

class ConfigTypeError : public ConfigException {
public:
    explicit ConfigTypeError(const std::string& message)
        : ConfigException("Type error: " + message) {}
};

class ConfigSchemaError : public ConfigException {
public:
    explicit ConfigSchemaError(const std::string& message)
        : ConfigException("Schema error: " + message) {}
};

} // namespace nlm
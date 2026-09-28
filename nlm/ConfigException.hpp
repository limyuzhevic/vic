/*
 * nlm/ConfigException.hpp - Configuration-specific exception classes for the NLM system
 * Specialized exceptions for configuration-related errors with detailed validation context
 */

#pragma once

#include "nlm/Exceptions.hpp"
#include <string>

namespace nlm {

// Configuration-specific exception types with detailed validation context
class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath)
        : ConfigException("Configuration file not found: " + filepath) {
        addContext("filepath", filepath);
    }
    
    ConfigFileNotFoundException(const std::string& filepath,
                                const std::string& file,
                                int line,
                                const std::string& function)
        : ConfigException("Configuration file not found: " + filepath, file, line, function) {
        addContext("filepath", filepath);
    }
    
    virtual const char* type() const noexcept override { return "ConfigFileNotFoundException"; }
};

class ConfigSyntaxErrorException : public ConfigException {
public:
    explicit ConfigSyntaxErrorException(const std::string& details)
        : ConfigException("Configuration syntax error: " + details) {
        addContext("details", details);
    }
    
    ConfigSyntaxErrorException(const std::string& details,
                               const std::string& file,
                               int line,
                               const std::string& function)
        : ConfigException("Configuration syntax error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "ConfigSyntaxErrorException"; }
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& key, const std::string& value)
        : ConfigException("Configuration validation error for key: " + key) {
        addContext("key", key);
        addContext("value", value);
    }
    
    ConfigValidationException(const std::string& key,
                              const std::string& value,
                              const std::string& file,
                              int line,
                              const std::string& function)
        : ConfigException("Configuration validation error for key: " + key, file, line, function) {
        addContext("key", key);
        addContext("value", value);
    }
    
    virtual const char* type() const noexcept override { return "ConfigValidationException"; }
};

class InvalidConfigurationException : public ConfigException {
public:
    explicit InvalidConfigurationException(const std::string& reason)
        : ConfigException("Invalid configuration: " + reason) {
        addContext("reason", reason);
    }
    
    InvalidConfigurationException(const std::string& reason,
                                  const std::string& file,
                                  int line,
                                  const std::string& function)
        : ConfigException("Invalid configuration: " + reason, file, line, function) {
        addContext("reason", reason);
    }
    
    virtual const char* type() const noexcept override { return "InvalidConfigurationException"; }
};

class ConfigKeyNotFoundException : public ConfigException {
public:
    explicit ConfigKeyNotFoundException(const std::string& key)
        : ConfigException("Configuration key not found: " + key) {
        addContext("key", key);
    }
    
    ConfigKeyNotFoundException(const std::string& key,
                               const std::string& file,
                               int line,
                               const std::string& function)
        : ConfigException("Configuration key not found: " + key, file, line, function) {
        addContext("key", key);
    }
    
    virtual const char* type() const noexcept override { return "ConfigKeyNotFoundException"; }
};

class ConfigTypeMismatchException : public ConfigException {
public:
    explicit ConfigTypeMismatchException(const std::string& key, 
                                         const std::string& expectedType, 
                                         const std::string& actualType)
        : ConfigException("Configuration type mismatch for key '" + key + "': expected " + expectedType + ", got " + actualType) {
        addContext("key", key);
        addContext("expectedType", expectedType);
        addContext("actualType", actualType);
    }
    
    ConfigTypeMismatchException(const std::string& key,
                                const std::string& expectedType,
                                const std::string& actualType,
                                const std::string& file,
                                int line,
                                const std::string& function)
        : ConfigException("Configuration type mismatch for key '" + key + "': expected " + expectedType + ", got " + actualType, file, line, function) {
        addContext("key", key);
        addContext("expectedType", expectedType);
        addContext("actualType", actualType);
    }
    
    virtual const char* type() const noexcept override { return "ConfigTypeMismatchException"; }
};

} // namespace nlm

#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base NLM exception class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

// Exception for configuration parsing errors
class ConfigParseException : public NLMException {
public:
    explicit ConfigParseException(const std::string& message) 
        : NLMException("ConfigParseException: " + message) {}
};

// Exception for configuration validation errors
class ConfigValidationException : public NLMException {
public:
    explicit ConfigValidationException(const std::string& message) 
        : NLMException("ConfigValidationException: " + message) {}
};

// Exception for configuration file errors
class ConfigFileException : public NLMException {
public:
    explicit ConfigFileException(const std::string& message) 
        : NLMException("ConfigFileException: " + message) {}
};

// Exception for configuration type errors
class ConfigTypeException : public NLMException {
public:
    explicit ConfigTypeException(const std::string& message) 
        : NLMException("ConfigTypeException: " + message) {}
};

} // namespace nlm
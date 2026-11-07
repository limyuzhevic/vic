#pragma once

#include <stdexcept>
#include <string>

namespace nlm {

/**
 * Base exception class for all NLM exceptions
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Error: " + message) {}
};

/**
 * Exception thrown for configuration errors
 */
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Configuration Error: " + message) {}
};

/**
 * Exception thrown for invalid parameter values
 */
class InvalidParameterException : public NLMException {
public:
    InvalidParameterException(const std::string& param, const std::string& message)
        : NLMException("Invalid parameter '" + param + "': " + message) {}
};

/**
 * Exception thrown for operation failures
 */
class OperationException : public NLMException {
public:
    explicit OperationException(const std::string& message) 
        : NLMException("Operation failed: " + message) {}
};

/**
 * Exception thrown for memory allocation failures
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) 
        : NLMException("Memory allocation failed: " + message) {}
};

/**
 * Exception thrown for checkpoint/file I/O errors
 */
class FileIOException : public NLMException {
public:
    explicit FileIOException(const std::string& file, const std::string& message)
        : NLMException("File I/O error for '" + file + "': " + message) {}
};

} // namespace nlm

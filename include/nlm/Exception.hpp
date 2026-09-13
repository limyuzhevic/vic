#pragma once

#include <string>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace nlm {

/**
 * Exception hierarchy for NLM applications
 * Provides detailed error information for debugging and user feedback
 */
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        Unknown = 0,
        FileNotFound,
        FilePermissionDenied,
        FileInvalidFormat,
        ConfigInvalid,
        ConfigMissingValue,
        ConfigOutOfRange,
        BrainInitializationFailed,
        SimulationError,
        MemoryAllocationFailed,
        InvalidParameter,
        InvalidState,
        NetworkError,
        SerializationError,
        DeserializationError,
        IntegrationError,
        ValidationError
    };

    NLMException(ErrorCode code, const std::string& message);
    NLMException(ErrorCode code, const std::string& message, const std::string& context);
    NLMException(ErrorCode code, const std::string& message, const std::string& context, const std::string& suggestion);

    ErrorCode getErrorCode() const { return errorCode; }
    const std::string& getContext() const { return context; }
    const std::string& getSuggestion() const { return suggestion; }
    const std::string& getFullMessage() const;

    // Convert error code to human-readable string
    static std::string errorCodeToString(ErrorCode code);

private:
    ErrorCode errorCode;
    std::string context;
    std::string suggestion;
    mutable std::string fullMessage;
    bool messageCached;
};

// Convenience exception classes for common errors
class FileNotFoundException : public NLMException {
public:
    FileNotFoundException(const std::string& path) : 
        NLMException(ErrorCode::FileNotFound, "File not found", path) {}
};

class ConfigException : public NLMException {
public:
    ConfigException(const std::string& message) : 
        NLMException(ErrorCode::ConfigInvalid, message) {}
};

class ParameterException : public NLMException {
public:
    ParameterException(const std::string& param, const std::string& value) : 
        NLMException(ErrorCode::InvalidParameter, 
                    "Invalid parameter value: " + param + " = " + value) {}
};

class InitializationException : public NLMException {
public:
    InitializationException(const std::string& component) : 
        NLMException(ErrorCode::BrainInitializationFailed, 
                    "Failed to initialize " + component) {}
};

} // namespace nlm

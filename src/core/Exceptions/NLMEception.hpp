#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base exception class for all NLM exceptions
class NLMException : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        UNKNOWN_ERROR = 1,
        INVALID_ARGUMENT = 2,
        NOT_INITIALIZED = 3,
        ALREADY_INITIALIZED = 4,
        OUT_OF_BOUNDS = 5,
        MEMORY_ALLOCATION_FAILED = 6,
        INVALID_STATE = 7,
        NETWORK_ERROR = 8,
        FILE_NOT_FOUND = 9,
        INVALID_FORMAT = 10,
        TIMEOUT = 11,
        PERMISSION_DENIED = 12,
        CONFIGURATION_ERROR = 13,
        PLASTICITY_ERROR = 14,
        NEURAL_ERROR = 15,
        MEMORY_ERROR = 16,
        NEURON_ERROR = 17,
        SYNAPSE_ERROR = 18,
        BRAIN_ERROR = 19,
        CONNECTION_ERROR = 20
    };

    NLMException(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR)
        : std::runtime_error(message), errorCode_(code) {}

    NLMException(const char* message, ErrorCode code = ErrorCode::UNKNOWN_ERROR)
        : std::runtime_error(message), errorCode_(code) {}

    ErrorCode getErrorCode() const { return errorCode_; }

protected:
    ErrorCode errorCode_;
};

} // namespace nlm
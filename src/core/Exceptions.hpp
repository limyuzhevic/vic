#pragma once

#include "src/core/Exception.hpp"

// Re-export all exception types for backward compatibility and ease of use
namespace nlm {

// Base exception types
using Exception = NLMException;
using RuntimeError = NLMRuntimeError;
using MemoryError = NLMMemoryError;
using ConfigError = NLMConfigError;
using ValidationError = NLMValidationError;
using FileError = NLMFileError;
using CheckpointError = NLMCheckpointError;
using AssertionError = NLMAssertionError;
using TestException = NLMTestException;

// Convenience macros (defined in Exception.hpp)
// NLM_THROW_VALIDATION
// NLM_THROW_RUNTIME
// NLM_THROW_MEMORY
// NLM_THROW_FILE
// NLM_THROW_CHECKPOINT
// NLM_THROW_ASSERT
// NLM_THROW_TEST
// NLM_THROW_STREAM

// Error code shortcuts for common operations
namespace ErrorCodes {
    const NLMException::ErrorCode SUCCESS = NLMException::ErrorCode::SUCCESS;
    const NLMException::ErrorCode UNKNOWN = NLMException::ErrorCode::UNKNOWN;
    const NLMException::ErrorCode INVALID_ARGUMENT = NLMException::ErrorCode::INVALID_ARGUMENT;
    const NLMException::ErrorCode OUT_OF_RANGE = NLMException::ErrorCode::OUT_OF_RANGE;
    const NLMException::ErrorCode NOT_IMPLEMENTED = NLMException::ErrorCode::NOT_IMPLEMENTED;
    const NLMException::ErrorCode INVALID_STATE = NLMException::ErrorCode::INVALID_STATE;
    const NLMException::ErrorCode TIMEOUT = NLMException::ErrorCode::TIMEOUT;
    const NLMException::ErrorCode MEMORY_ALLOCATION = NLMException::ErrorCode::MEMORY_ALLOCATION;
    const NLMException::ErrorCode FILE_NOT_FOUND = NLMException::ErrorCode::FILE_NOT_FOUND;
    const NLMException::ErrorCode PERMISSION_DENIED = NLMException::ErrorCode::PERMISSION_DENIED;
    const NLMException::ErrorCode NETWORK_ERROR = NLMException::ErrorCode::NETWORK_ERROR;
    const NLMException::ErrorCode CONFIGURATION = NLMException::ErrorCode::CONFIGURATION;
    const NLMException::ErrorCode VALIDATION = NLMException::ErrorCode::VALIDATION;
    const NLMException::ErrorCode CHECKPOINT = NLMException::ErrorCode::CHECKPOINT;
    const NLMException::ErrorCode CONVERSION = NLMException::ErrorCode::CONVERSION;
    const NLMException::ErrorCode PLASTICITY = NLMException::ErrorCode::PLASTICITY;
    const NLMException::ErrorCode NEUROMODULATION = NLMException::ErrorCode::NEUROMODULATION;
    const NLMException::ErrorCode NEURAL_DYNAMICS = NLMException::ErrorCode::NEURAL_DYNAMICS;
    const NLMException::ErrorCode MOTOR = NLMException::ErrorCode::MOTOR;
    const NLMException::ErrorCode WORLD = NLMException::ErrorCode::WORLD;
    const NLMException::ErrorCode SENSORY = NLMException::ErrorCode::SENSORY;
    const NLMException::ErrorCode PREDICTION = NLMException::ErrorCode::PREDICTION;
    const NLMException::ErrorCode COGNITION = NLMException::ErrorCode::COGNITION;
    const NLMException::ErrorCode AGENT = NLMException::ErrorCode::AGENT;
    const NLMException::ErrorCode EXPERIMENT = NLMException::ErrorCode::EXPERIMENT;
    const NLMException::ErrorCode VISUALIZATION = NLMException::ErrorCode::VISUALIZATION;
    const NLMException::ErrorCode SYSTEM = NLMException::ErrorCode::SYSTEM;
}

// Custom exception for CheckpointTransaction compatibility
namespace CheckpointTransaction {
    class Exception : public std::runtime_error {
    public:
        enum class ErrorCode {
            IO_ERROR = 1,
            VALIDATION_ERROR = 2,
            ENCODING_ERROR = 3,
            RESOURCE_ERROR = 4,
            CHECKSUM_ERROR = 5
        };

        Exception(const std::string& message, ErrorCode code = ErrorCode::IO_ERROR)
            : std::runtime_error(message), errorCode_(code) {}

        ErrorCode getErrorCode() const { return errorCode_; }

    private:
        ErrorCode errorCode_;
    };
}

} // namespace nlm

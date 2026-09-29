"""
C++ exception definitions for NLM Python bindings.
This header defines the C++ exception classes that will be caught and translated to Python exceptions.
"""

#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base C++ exception that corresponds to the Python NLMException
class NLMException : public std::runtime_error {
public:
    NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMException() = default;
    virtual const char* getErrorCode() const { return "NLM_ERROR"; }
};

// File I/O exceptions
class FileNotFoundException : public NLMException {
public:
    FileNotFoundException(const std::string& filepath) 
        : NLMException("File not found: " + filepath) {}
    virtual const char* getErrorCode() const { return "FILE_NOT_FOUND"; }
};

class FilePermissionException : public NLMException {
public:
    FilePermissionException(const std::string& filepath) 
        : NLMException("Permission denied: " + filepath) {}
    virtual const char* getErrorCode() const { return "FILE_PERMISSION_ERROR"; }
};

class FileFormatException : public NLMException {
public:
    FileFormatException(const std::string& filepath, const std::string& details = "")
        : NLMException("Invalid file format: " + filepath + (details.empty() ? "" : (" - " + details))) {}
    virtual const char* getErrorCode() const { return "FILE_FORMAT_ERROR"; }
};

class FileWriteException : public NLMException {
public:
    FileWriteException(const std::string& filepath, const std::string& details = "")
        : NLMException("Failed to write file: " + filepath + (details.empty() ? "" : (" - " + details))) {}
    virtual const char* getErrorCode() const { return "FILE_WRITE_ERROR"; }
};

// Validation exceptions
class ValidationException : public NLMException {
public:
    ValidationException(const std::string& message) : NLMException(message) {}
    virtual const char* getErrorCode() const { return "VALIDATION_ERROR"; }
};

class InvalidDimensionsException : public ValidationException {
public:
    InvalidDimensionsException(const std::string& details)
        : ValidationException("Invalid dimensions: " + details) {}
    virtual const char* getErrorCode() const { return "INVALID_DIMENSIONS"; }
};

class InvalidRangeException : public ValidationException {
public:
    InvalidRangeException(const std::string& field, double value, double expected_min, double expected_max)
        : ValidationException("Invalid range for field '" + field + "': " + 
                            std::to_string(value) + " (expected: " + 
                            std::to_string(expected_min) + " to " + std::to_string(expected_max) + ")") {}
    virtual const char* getErrorCode() const { return "INVALID_RANGE"; }
};

class InvalidFormatException : public ValidationException {
public:
    InvalidFormatException(const std::string& field, const std::string& expected_format)
        : ValidationException("Invalid format for field '" + field + "': expected " + expected_format) {}
    virtual const char* getErrorCode() const { return "INVALID_FORMAT"; }
};

class NullOrEmptyException : public ValidationException {
public:
    NullOrEmptyException(const std::string& field)
        : ValidationException("Field cannot be null or empty: '" + field + "'") {}
    virtual const char* getErrorCode() const { return "NULL_OR_EMPTY"; }
};

// Configuration exceptions
class KeyNotFoundException : public NLMException {
public:
    KeyNotFoundException(const std::string& key)
        : NLMException("Configuration key not found: '" + key + "'") {}
    virtual const char* getErrorCode() const { return "KEY_NOT_FOUND"; }
};

class TypeMismatchException : public NLMException {
public:
    TypeMismatchException(const std::string& key, const std::string& expected_type, const std::string& actual_type)
        : NLMException("Type mismatch for key '" + key + "': expected " + expected_type + ", got " + actual_type) {}
    virtual const char* getErrorCode() const { return "TYPE_MISMATCH"; }
};

// Brain exceptions
class NeuronNotFoundException : public NLMException {
public:
    NeuronNotFoundException(uint64_t neuron_id)
        : NLMException("Neuron not found: " + std::to_string(neuron_id)) {}
    virtual const char* getErrorCode() const { return "NEURON_NOT_FOUND"; }
};

class InvalidCurrentException : public NLMException {
public:
    InvalidCurrentException(double current)
        : NLMException("Invalid current value: " + std::to_string(current)) {}
    virtual const char* getErrorCode() const { return "INVALID_CURRENT_VALUE"; }
};

class CurrentOutOfRangeException : public NLMException {
public:
    CurrentOutOfRangeException(double current, double min, double max)
        : NLMException("Current value " + std::to_string(current) + " out of range: " + 
                      std::to_string(min) + " to " + std::to_string(max)) {}
    virtual const char* getErrorCode() const { return "CURRENT_OUT_OF_RANGE"; }
};

// World exceptions
class PositionOutOfBoundsException : public NLMException {
public:
    PositionOutOfBoundsException(float x, float y, float width, float height)
        : NLMException("Position (" + std::to_string(x) + ", " + std::to_string(y) + ") out of bounds: " +
                      std::to_string(width) + "x" + std::to_string(height)) {}
    virtual const char* getErrorCode() const { return "POSITION_OUT_OF_BOUNDS"; }
};

class InvalidCommandException : public NLMException {
public:
    InvalidCommandException(const std::string& command)
        : NLMException("Invalid motor command: '" + command + "'") {}
    virtual const char* getErrorCode() const { return "INVALID_COMMAND"; }
};

// Sensory exceptions
class InvalidDataSizeException : public NLMException {
public:
    InvalidDataSizeException(const std::string& sensor_type, size_t expected, size_t actual)
        : NLMException("Invalid data size for " + sensor_type + ": expected " + 
                      std::to_string(expected) + ", got " + std::to_string(actual)) {}
    virtual const char* getErrorCode() const { return "INVALID_DATA_SIZE"; }
};

class InvalidDataFormatException : public NLMException {
public:
    InvalidDataFormatException(const std::string& sensor_type, const std::string& details)
        : NLMException("Invalid data format for " + sensor_type + ": " + details) {}
    virtual const char* getErrorCode() const { return "INVALID_DATA_FORMAT"; }
};

// Audio exceptions
class InvalidSampleRateException : public NLMException {
public:
    InvalidSampleRateException(int sample_rate, int min_sample_rate)
        : NLMException("Invalid sample rate: " + std::to_string(sample_rate) + 
                      " (minimum: " + std::to_string(min_sample_rate) + ")") {}
    virtual const char* getErrorCode() const { return "INVALID_SAMPLE_RATE"; }
};

class SampleRateMismatchException : public NLMException {
public:
    SampleRateMismatchException(int expected_sample_rate, int actual_sample_rate)
        : NLMException("Sample rate mismatch: expected " + std::to_string(expected_sample_rate) + 
                      ", got " + std::to_string(actual_sample_rate)) {}
    virtual const char* getErrorCode() const { return "SAMPLE_RATE_MISMATCH"; }
};

class AudioLengthMismatchException : public NLMException {
public:
    AudioLengthMismatchException(int expected_samples, int actual_samples)
        : NLMException("Audio length mismatch: expected " + std::to_string(expected_samples) + 
                      " samples, got " + std::to_string(actual_samples)) {}
    virtual const char* getErrorCode() const { return "AUDIO_LENGTH_MISMATCH"; }
};

// Action exceptions
class InvalidActionTypeException : public NLMException {
public:
    InvalidActionTypeException(const std::string& action_type)
        : NLMException("Invalid action type: '" + action_type + "'") {}
    virtual const char* getErrorCode() const { return "INVALID_ACTION_TYPE"; }
};

class InvalidParameterException : public NLMException {
public:
    InvalidParameterException(const std::string& parameter, const std::string& details)
        : NLMException("Invalid parameter '" + parameter + "': " + details) {}
    virtual const char* getErrorCode() const { return "INVALID_PARAMETER"; }
};

class ParameterOutOfRangeException : public NLMException {
public:
    ParameterOutOfRangeException(const std::string& parameter, double value, double min, double max)
        : NLMException("Parameter '" + parameter + "' value " + std::to_string(value) + 
                      " out of range: " + std::to_string(min) + " to " + std::to_string(max)) {}
    virtual const char* getErrorCode() const { return "PARAMETER_OUT_OF_RANGE"; }
};

// Agent exceptions
class SensorInputException : public NLMException {
public:
    SensorInputException(const std::string& details)
        : NLMException("Sensor input error: " + details) {}
    virtual const char* getErrorCode() const { return "SENSOR_INPUT_ERROR"; }
};

class MotorOutputException : public NLMException {
public:
    MotorOutputException(const std::string& details)
        : NLMException("Motor output error: " + details) {}
    virtual const char* getErrorCode() const { return "MOTOR_OUTPUT_ERROR"; }
};

// Memory exception
class MemoryAllocationException : public NLMException {
public:
    MemoryAllocationException(const std::string& details)
        : NLMException("Memory allocation error: " + details) {}
    virtual const char* getErrorCode() const { return "MEMORY_ALLOCATION_ERROR"; }
};

} // namespace nlm
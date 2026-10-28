#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace nlm {

/**
 * Base exception class for NLM framework
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
    
    explicit NLMException(const char* message)
        : std::runtime_error(message) {}
    
    virtual ~NLMException() = default;
};

/**
 * Configuration-related exceptions
 */
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Config: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message, const std::string& filepath = "")
        : ConfigException("Parse error" + (filepath.empty() ? "" : " in '" + filepath + "'") + ": " + message) {}
};

/**
 * Neural network exceptions
 */
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message)
        : NLMException("Brain: " + message) {}
};

class NeuronException : public BrainException {
public:
    explicit NeuronException(const std::string& message)
        : BrainException("Neuron: " + message) {}
};

class SynapseException : public BrainException {
public:
    explicit SynapseException(const std::string& message)
        : BrainException("Synapse: " + message) {}
};

/**
 * Performance system exceptions
 */
class PerformanceException : public NLMException {
public:
    explicit PerformanceException(const std::string& message)
        : NLMException("Performance: " + message) {}
};

class EventQueueException : public PerformanceException {
public:
    explicit EventQueueException(const std::string& message)
        : PerformanceException("EventQueue: " + message) {}
};

/**
 * Memory system exceptions
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory: " + message) {}
};

/**
 * Experiment exceptions
 */
class ExperimentException : public NLMException {
public:
    explicit ExperimentException(const std::string& message)
        : NLMException("Experiment: " + message) {}
};

/**
 * Utility exception for invalid parameters
 */
class InvalidParameterException : public NLMException {
public:
    template<typename T>
    static InvalidParameterException forValue(const std::string& param, const T& value) {
        std::ostringstream oss;
        oss << "Invalid value " << value << " for parameter '" << param << "'";
        return InvalidParameterException(oss.str());
    }
    
    template<typename T>
    static InvalidParameterException forRange(const std::string& param, const T& value, const T& min, const T& max) {
        std::ostringstream oss;
        oss << "Value " << value << " out of range [" << min << ", " << max << "] for parameter '" << param << "'";
        return InvalidParameterException(oss.str());
    }
    
private:
    explicit InvalidParameterException(const std::string& message)
        : NLMException(message) {}
};

/**
 * Memory allocation exception
 */
class MemoryAllocationException : public NLMException {
public:
    explicit MemoryAllocationException(size_t size, const std::string& context = "")
        : NLMException("Memory allocation failed for " + std::to_string(size) + 
                      " bytes" + (context.empty() ? "" : " in " + context)) {}
};

/**
 * File I/O exception
 */
class FileIOException : public NLMException {
public:
    explicit FileIOException(const std::string& message, const std::string& filepath = "")
        : NLMException("File I/O error" + (filepath.empty() ? "" : " in '" + filepath + "'") + ": " + message) {}
};

/**
 * Validation exception
 */
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message)
        : NLMException("Validation error: " + message) {}
};

} // namespace nlm
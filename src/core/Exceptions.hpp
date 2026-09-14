#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base exception class for all NLM exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
    
    NLMException(const std::string& message, const std::string& details)
        : std::runtime_error(message + ": " + details) {}
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Config error: " + message) {}
};

class InvalidConfigValueException : public ConfigException {
public:
    InvalidConfigValueException(const std::string& key, const std::string& expected)
        : ConfigException("Invalid value for key '" + key + "', expected: " + expected) {}
};

class MissingConfigEntryException : public ConfigException {
public:
    explicit MissingConfigEntryException(const std::string& key)
        : ConfigException("Missing required config entry: '" + key + "'") {}
};

// Brain-related exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message)
        : NLMException("Brain error: " + message) {}
};

class BrainInitializationException : public BrainException {
public:
    explicit BrainInitializationException(const std::string& reason)
        : BrainException("Brain initialization failed: " + reason) {}
};

class InvalidBrainStateException : public BrainException {
public:
    explicit InvalidBrainStateException(const std::string& reason)
        : BrainException("Invalid brain state: " + reason) {}
};

// Neural network parameter validation exceptions
class NeuralNetworkException : public NLMException {
public:
    explicit NeuralNetworkException(const std::string& message)
        : NLMException("Neural network error: " + message) {}
};

class InvalidNeuronCountException : public NeuralNetworkException {
public:
    explicit InvalidNeuronCountException(size_t count)
        : NeuralNetworkException("Invalid neuron count: " + std::to_string(count) + ", must be > 0") {}
};

class InvalidConnectionProbabilityException : public NeuralNetworkException {
public:
    explicit InvalidConnectionProbabilityException(float prob)
        : NeuralNetworkException("Invalid connection probability: " + std::to_string(prob) + ", must be in [0,1]") {}
};

// Memory system exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory error: " + message) {}
};

// Development system exceptions
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message)
        : NLMException("Development error: " + message) {}
};

// Integration test exceptions
class IntegrationTestException : public NLMException {
public:
    explicit IntegrationTestException(const std::string& message)
        : NLMException("Integration test error: " + message) {}
};

// System validation exceptions
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message)
        : NLMException("Validation error: " + message) {}
};

// Recovery operation exceptions
class RecoveryException : public NLMException {
public:
    explicit RecoveryException(const std::string& message)
        : NLMException("Recovery error: " + message) {}
};

} // namespace nlm

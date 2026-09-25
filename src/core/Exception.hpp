// NLM Exception Hierarchy
// Core error classes for the Neural Learning Machine

#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base exception for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message)
        : std::runtime_error(message) {}
    
    virtual ~NLMException() = default;
};

// Configuration-related errors
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message)
        : NLMException("Configuration Error: " + message) {}
    
    virtual ~ConfigException() = default;
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message, const std::string& filepath = "")
        : ConfigException("Failed to parse configuration" + (filepath.empty() ? "" : " from " + filepath) + ": " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigException("Configuration validation failed: " + message) {}
};

// Neural network related errors
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message)
        : NLMException("Neural Network Error: " + message) {}
    
    virtual ~NeuralException() = default;
};

class NeuronException : public NeuralException {
public:
    explicit NeuronException(const std::string& message)
        : NeuralException("Neuron Error: " + message) {}
};

class SynapseException : public NeuralException {
public:
    explicit SynapseException(const std::string& message)
        : NeuralException("Synapse Error: " + message) {}
};

// Plasticity-related errors
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message)
        : NLMException("Plasticity Error: " + message) {}
};

// Simulation errors
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message)
        : NLMException("Simulation Error: " + message) {}
};

// Memory system errors
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message)
        : NLMException("Memory System Error: " + message) {}
};

// System-level errors
class SystemException : public NLMException {
public:
    explicit SystemException(const std::string& message)
        : NLMException("System Error: " + message) {}
};

class InitializationException : public SystemException {
public:
    explicit InitializationException(const std::string& component, const std::string& reason)
        : SystemException("Failed to initialize " + component + ": " + reason) {}
};

// Convenience exception types for common scenarios
class InvalidNeuronStateException : public NeuronException {
public:
    explicit InvalidNeuronStateException(const std::string& reason)
        : NeuronException("Invalid neuron state: " + reason) {}
};

class InvalidSynapseConnectionException : public SynapseException {
public:
    explicit InvalidSynapseConnectionException(const std::string& reason)
        : SynapseException("Invalid synapse connection: " + reason) {}
};

class PlasticityRuleError : public PlasticityException {
public:
    explicit PlasticityRuleError(const std::string& rule, const std::string& reason)
        : PlasticityException("Plasticity rule " + rule + " failed: " + reason) {}
};

class MemoryAllocationError : public MemoryException {
public:
    explicit MemoryAllocationError(size_t size, const std::string& context)
        : MemoryException("Memory allocation failed: " + std::to_string(size) + " bytes in " + context) {}
};

class SimulationStepError : public SimulationException {
public:
    explicit SimulationStepError(const std::string& reason)
        : SimulationException("Simulation step failed: " + reason) {}
};

// Exception factory functions for convenient use
inline std::shared_ptr<NLMException> createException(const std::string& type, const std::string& message) {
    if (type == "Config") {
        return std::make_shared<ConfigException>(message);
    } else if (type == "ConfigParse") {
        return std::make_shared<ConfigParseException>(message);
    } else if (type == "ConfigValidation") {
        return std::make_shared<ConfigValidationException>(message);
    } else if (type == "Neural") {
        return std::make_shared<NeuralException>(message);
    } else if (type == "Neuron") {
        return std::make_shared<NeuronException>(message);
    } else if (type == "Synapse") {
        return std::make_shared<SynapseException>(message);
    } else if (type == "Plasticity") {
        return std::make_shared<PlasticityException>(message);
    } else if (type == "Simulation") {
        return std::make_shared<SimulationException>(message);
    } else if (type == "Memory") {
        return std::make_shared<MemoryException>(message);
    } else {
        return std::make_shared<NLMException>(message);
    }
}

} // namespace nlm

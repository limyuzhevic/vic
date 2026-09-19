#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Base exception class for all NLM exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error(message) {}
    virtual ~NLMException() = default;
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Config error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) 
        : ConfigException("Parse error: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) 
        : ConfigException("Validation error: " + message) {}
};

class ConfigFileNotFoundException : public ConfigException {
public:
    explicit ConfigFileNotFoundException(const std::string& filepath) 
        : ConfigException("File not found: " + filepath) {}
};

// Memory management exceptions
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) 
        : NLMException("Memory error: " + message) {}
};

class NullPointerException : public MemoryException {
public:
    explicit NullPointerException(const std::string& context) 
        : MemoryException("Null pointer in " + context) {}
};

// Neural network exceptions
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message) 
        : NLMException("Neural error: " + message) {}
};

class NeuronException : public NeuralException {
public:
    explicit NeuronException(const std::string& message) 
        : NeuralException("Neuron error: " + message) {}
};

class SynapseException : public NeuralException {
public:
    explicit SynapseException(const std::string& message) 
        : NeuralException("Synapse error: " + message) {}
};

class BrainException : public NeuralException {
public:
    explicit BrainException(const std::string& message) 
        : NeuralException("Brain error: " + message) {}
};

// Agent exceptions
class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message) 
        : NLMException("Agent error: " + message) {}
};

class AgentBrainException : public AgentException {
public:
    explicit AgentBrainException(const std::string& message) 
        : AgentException("AgentBrain error: " + message) {}
};

// World/environment exceptions
class WorldException : public NLMException {
public:
    explicit WorldException(const std::string& message) 
        : NLMException("World error: " + message) {}
};

// Plasticity exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) 
        : NLMException("Plasticity error: " + message) {}
};

// System exceptions
class SystemException : public NLMException {
public:
    explicit SystemException(const std::string& message) 
        : NLMException("System error: " + message) {}
};

class LoggerException : public SystemException {
public:
    explicit LoggerException(const std::string& message) 
        : SystemException("Logger error: " + message) {}
};

} // namespace nlm

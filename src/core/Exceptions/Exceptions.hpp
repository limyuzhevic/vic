#pragma once

#include <string>
#include <stdexcept>
#include <memory>

namespace nlm {

/**
 * Base exception class for all NLM-specific exceptions
 */
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    virtual ~NLMException() = default;
};

/**
 * Configuration-related exceptions
 */
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) : NLMException("Config: " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException("Validation error: " + message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException("Parse error: " + message) {}
};

/**
 * Neural network component exceptions
 */
class NeuralException : public NLMException {
public:
    explicit NeuralException(const std::string& message) : NLMException("Neural: " + message) {}
};

class NeuronException : public NeuralException {
public:
    explicit NeuronException(const std::string& message) : NeuralException("Neuron: " + message) {}
};

class SynapseException : public NeuralException {
public:
    explicit SynapseException(const std::string& message) : NeuralException("Synapse: " + message) {}
};

/**
 * Plasticity mechanism exceptions
 */
class PlasticityException : public NeuralException {
public:
    explicit PlasticityException(const std::string& message) : NeuralException("Plasticity: " + message) {}
};

/**
 * Memory system exceptions
 */
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException("Memory: " + message) {}
};

/**
 * Simulation and time-related exceptions
 */
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) : NLMException("Simulation: " + message) {}
};

/**
 * Exception factory for creating appropriate exceptions
 */
class ExceptionFactory {
public:
    static std::unique_ptr<NLMException> createValidationError(const std::string& component, const std::string& message) {
        return std::make_unique<NLMException>(component + ": Validation error - " + message);
    }
    
    static std::unique_ptr<NLMException> createNullPointerError(const std::string& component, const std::string& operation) {
        return std::make_unique<NLMException>(component + ": Null pointer error in " + operation);
    }
    
    static std::unique_ptr<NLMException> createRuntimeError(const std::string& component, const std::string& message) {
        return std::make_unique<NLMException>(component + ": " + message);
    }
};

} // namespace nlm

#ifdef SWIG
%inline {
    class NLMException : public std::runtime_error {
    public:
        explicit NLMException(const std::string& message) : std::runtime_error(message) {}
    };
    
    class ConfigException : public NLMException {
    public:
        explicit ConfigException(const std::string& message) : NLMException("Config: " + message) {}
    };
    
    class NeuronException : public NLMException {
    public:
        explicit NeuronException(const std::string& message) : NLMException("Neuron: " + message) {}
    };
    
    class SynapseException : public NLMException {
    public:
        explicit SynapseException(const std::string& message) : NLMException("Synapse: " + message) {}
    };
}
#endif

#pragma once

#include <string>
#include <stdexcept>
#include "Types/Types.hpp"

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
        : NLMException("Config error: " + message) {}
};

class ConfigLoadException : public ConfigException {
public:
    explicit ConfigLoadException(const std::string& message, const std::string& filepath = "")
        : ConfigException("Failed to load config" + (filepath.empty() ? "" : ": " + filepath) + ": " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigException("Invalid configuration: " + message) {}
};

class ConfigParameterException : public ConfigValidationException {
public:
    explicit ConfigParameterException(const std::string& param, const std::string& reason)
        : ConfigValidationException("Parameter '" + param + "': " + reason) {}
};

// Brain and neural system errors
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) 
        : NLMException("Brain error: " + message) {}
};

class NeuronException : public BrainException {
public:
    explicit NeuronException(const std::string& message) 
        : BrainException("Neuron error: " + message) {}
};

class SynapseException : public BrainException {
public:
    explicit SynapseException(const std::string& message) 
        : BrainException("Synapse error: " + message) {}
};

class NeuralRegionException : public BrainException {
public:
    explicit NeuralRegionException(const std::string& message) 
        : BrainException("Neural region error: " + message) {}
};

// Agent-related errors
class AgentException : public NLMException {
public:
    explicit AgentException(const std::string& message) 
        : NLMException("Agent error: " + message) {}
};

class AgentBrainException : public AgentException {
public:
    explicit AgentBrainException(const std::string& message) 
        : AgentException("Agent brain error: " + message) {}
};

class AgentBodyException : public AgentException {
public:
    explicit AgentBodyException(const std::string& message) 
        : AgentException("Agent body error: " + message) {}
};

// Simulation and system errors
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) 
        : NLMException("Simulation error: " + message) {}
};

class StepException : public SimulationException {
public:
    explicit StepException(const std::string& message)
        : SimulationException("Step error: " + message) {}
};

// Memory system errors
class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) 
        : NLMException("Memory error: " + message) {}
};

// Plasticity and learning errors
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) 
        : NLMException("Plasticity error: " + message) {}
};

// Neuromodulation errors
class NeuromodulationException : public NLMException {
public:
    explicit NeuromodulationException(const std::string& message) 
        : NLMException("Neuromodulation error: " + message) {}
};

// Event queue and system errors
class EventQueueException : public NLMException {
public:
    explicit EventQueueException(const std::string& message) 
        : NLMException("Event queue error: " + message) {}
};

// Performance and resource errors
class PerformanceException : public NLMException {
public:
    explicit PerformanceException(const std::string& message) 
        : NLMException("Performance error: " + message) {}
};

class ResourceException : public PerformanceException {
public:
    explicit ResourceException(const std::string& message)
        : PerformanceException("Resource error: " + message) {}
};

// Development system errors
class DevelopmentException : public NLMException {
public:
    explicit DevelopmentException(const std::string& message) 
        : NLMException("Development error: " + message) {}
};

} // namespace nlm

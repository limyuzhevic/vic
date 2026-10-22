#pragma once

#include <string>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Base exception class for all NLM errors
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) 
        : std::runtime_error("NLM Exception: " + message) {}
    
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

// Configuration-related exceptions
class ConfigException : public NLMException {
public:
    explicit ConfigException(const std::string& message) 
        : NLMException("Config Error: " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    explicit ConfigFileException(const std::string& filepath, const std::string& reason)
        : ConfigException("Cannot load config file '" + filepath + "': " + reason) {}
};

class ConfigParseException : public ConfigException {
public:
    ConfigParseException(const std::string& key, const std::string& value, const std::string& reason)
        : ConfigException("Cannot parse config value '" + value + "' for key '" + key + "': " + reason) {}
};

// Brain/neural component exceptions
class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message)
        : NLMException("Brain Error: " + message) {}
};

class NeuronException : public BrainException {
public:
    explicit NeuronException(const std::string& message)
        : BrainException("Neuron Error: " + message) {}
};

class SynapseException : public BrainException {
public:
    explicit SynapseException(const std::string& message)
        : BrainException("Synapse Error: " + message) {}
};

// Simulation and runtime exceptions
class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message)
        : NLMException("Simulation Error: " + message) {}
};

class MemoryException : public SimulationException {
public:
    explicit MemoryException(const std::string& message)
        : SimulationException("Memory Error: " + message) {}
};

// Plasticity and learning exceptions
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message)
        : NLMException("Plasticity Error: " + message) {}
};

// Environment and world exceptions
class EnvironmentException : public NLMException {
public:
    explicit EnvironmentException(const std::string& message)
        : NLMException("Environment Error: " + message) {}
};

// Validation exceptions
class ValidationException : public NLMException {
public:
    explicit ValidationException(const std::string& message)
        : NLMException("Validation Error: " + message) {}
};

// Utility class for exception formatting
class ExceptionFormatter {
public:
    template<typename... Args>
    static std::string format(const char* fmt, Args... args) {
        std::ostringstream oss;
        oss << fmt;
        // Simple formatting - for more complex formatting, consider using fmt library
        return oss.str();
    }
    
    template<typename... Args>
    static void throwWithContext(const std::string& context, const std::string& message, Args... args) {
        std::ostringstream oss;
        oss << context;
        if (!message.empty()) {
            oss << ": " << message;
        }
        throw NLMException(oss.str());
    }
};

} // namespace nlm

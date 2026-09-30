#pragma once

#include "Exceptions.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Custom exception for configuration validation errors
 */
class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) 
        : ConfigException("Validation error: " + message) {}
    
    explicit ConfigValidationException(const std::string& key, const std::string& expected, const std::string& actual)
        : ConfigException("Invalid value for key '" + key + "': expected '" + expected + "', got '" + actual + "'") {}
};

/**
 * Custom exception for neuron-related errors
 */
class NeuronException : public NLMException {
public:
    explicit NeuronException(const std::string& message) 
        : NLMException("Neuron: " + message) {}
};

class NeuronNullPointerException : public NeuronException {
public:
    explicit NeuronNullPointerException(const std::string& operation) 
        : NeuronException("Null pointer error in " + operation) {}
};

class NeuronInvalidParameterException : public NeuronException {
public:
    explicit NeuronInvalidParameterException(const std::string& param, const std::string& expected, const std::string& actual)
        : NeuronException("Invalid parameter '" + param + "': expected '" + expected + "', got '" + actual + "'") {}
};

/**
 * Custom exception for synapse-related errors
 */
class SynapseException : public NLMException {
public:
    explicit SynapseException(const std::string& message) 
        : NLMException("Synapse: " + message) {}
};

class SynapseNullPointerException : public SynapseException {
public:
    explicit SynapseNullPointerException(const std::string& operation) 
        : SynapseException("Null pointer error in " + operation) {}
};

/**
 * Custom exception for plasticity mechanism errors
 */
class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) 
        : NLMException("Plasticity: " + message) {}
};

/**
 * Custom exception for memory system errors
 */
class NeuralEpisodicMemoryException : public NLMException {
public:
    explicit NeuralEpisodicMemoryException(const std::string& message) 
        : NLMException("NeuralEpisodicMemory: " + message) {}
};

class NeuralAssociativeMemoryException : public NLMException {
public:
    explicit NeuralAssociativeMemoryException(const std::string& message) 
        : NLMException("NeuralAssociativeMemory: " + message) {}
};

/**
 * Error handling utility functions
 */
namespace error {
    
    // Macro for consistent null pointer checking
    #define CHECK_NULL_PTR(ptr, operation) \
        do { \
            if ((ptr) == nullptr) { \
                throw nlm::ExceptionFactory::createNullPointerError(#operation, operation); \
            } \
        } while(0)
    
    // Macro for consistent parameter validation
    #define VALIDATE_PARAMETER(condition, message) \
        do { \
            if (!(condition)) { \
                throw nlm::ExceptionFactory::createValidationError(#condition, message); \
            } \
        } while(0)
    
    // Macro for consistent error handling with logging
    #define HANDLE_ERROR(component, operation, error) \
        do { \
            auto exc = nlm::ExceptionFactory::createRuntimeError(component, operation + ": " + error); \
            // In a real implementation, you would log the error here \
            throw exc; \
        } while(0)
    
} // namespace error

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
    
    class ConfigValidationException : public ConfigException {
    public:
        explicit ConfigValidationException(const std::string& message) 
            : ConfigException("Validation error: " + message) {}
    };
    
    class NeuronException : public NLMException {
    public:
        explicit NeuronException(const std::string& message) 
            : NLMException("Neuron: " + message) {}
    };
    
    class SynapseException : public NLMException {
    public:
        explicit SynapseException(const std::string& message) 
            : NLMException("Synapse: " + message) {}
    };
    
    class PlasticityException : public NLMException {
    public:
        explicit PlasticityException(const std::string& message) 
            : NLMException("Plasticity: " + message) {}
    };
    
    class NeuralEpisodicMemoryException : public NLMException {
    public:
        explicit NeuralEpisodicMemoryException(const std::string& message) 
            : NLMException("NeuralEpisodicMemory: " + message) {}
    };
}
#endif

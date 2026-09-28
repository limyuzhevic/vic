/*
 * nlm/NeuralException.hpp - Neural computation exception classes for the NLM system
 * Specialized exceptions for neural computation and plasticity errors
 */

#pragma once

#include "nlm/Exceptions.hpp"
#include <string>

namespace nlm {

// Neural computation-specific exception types
class NeuralParameterException : public NeuralException {
public:
    explicit NeuralParameterException(const std::string& parameter, float value)
        : NeuralException("Neural parameter error: " + parameter + " = " + std::to_string(value)) {
        addContext("parameter", parameter);
        addContext("value", std::to_string(value));
    }
    
    NeuralParameterException(const std::string& parameter,
                             float value,
                             const std::string& file,
                             int line,
                             const std::string& function)
        : NeuralException("Neural parameter error: " + parameter + " = " + std::to_string(value), file, line, function) {
        addContext("parameter", parameter);
        addContext("value", std::to_string(value));
    }
    
    virtual const char* type() const noexcept override { return "NeuralParameterException"; }
};

class PlasticityErrorException : public NeuralException {
public:
    explicit PlasticityErrorException(const std::string& details)
        : NeuralException("Plasticity error: " + details) {
        addContext("details", details);
    }
    
    PlasticityErrorException(const std::string& details,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : NeuralException("Plasticity error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "PlasticityErrorException"; }
};

class MemoryAllocationException : public NeuralException {
public:
    explicit MemoryAllocationException(size_t size, const std::string& type)
        : NeuralException("Memory allocation error: " + std::to_string(size) + " bytes of type " + type) {
        addContext("size", std::to_string(size));
        addContext("type", type);
    }
    
    MemoryAllocationException(size_t size,
                              const std::string& type,
                              const std::string& file,
                              int line,
                              const std::string& function)
        : NeuralException("Memory allocation error: " + std::to_string(size) + " bytes of type " + type, file, line, function) {
        addContext("size", std::to_string(size));
        addContext("type", type);
    }
    
    virtual const char* type() const noexcept override { return "MemoryAllocationException"; }
};

class NeuralComputationException : public NeuralException {
public:
    explicit NeuralComputationException(const std::string& details)
        : NeuralException("Neural computation error: " + details) {
        addContext("details", details);
    }
    
    NeuralComputationException(const std::string& details,
                               const std::string& file,
                               int line,
                               const std::string& function)
        : NeuralException("Neural computation error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "NeuralComputationException"; }
};

class NeuralDynamicsException : public NeuralException {
public:
    explicit NeuralDynamicsException(const std::string& details)
        : NeuralException("Neural dynamics error: " + details) {
        addContext("details", details);
    }
    
    NeuralDynamicsException(const std::string& details,
                            const std::string& file,
                            int line,
                            const std::string& function)
        : NeuralException("Neural dynamics error: " + details, file, line, function) {
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "NeuralDynamicsException"; }
};

class SynapticErrorException : public NeuralException {
public:
    explicit SynapticErrorException(const std::string& synapseId, const std::string& details)
        : NeuralException("Synaptic error for synapse '" + synapseId + "': " + details) {
        addContext("synapseId", synapseId);
        addContext("details", details);
    }
    
    SynapticErrorException(const std::string& synapseId,
                          const std::string& details,
                          const std::string& file,
                          int line,
                          const std::string& function)
        : NeuralException("Synaptic error for synapse '" + synapseId + "': " + details, file, line, function) {
        addContext("synapseId", synapseId);
        addContext("details", details);
    }
    
    virtual const char* type() const noexcept override { return "SynapticErrorException"; }
};

} // namespace nlm

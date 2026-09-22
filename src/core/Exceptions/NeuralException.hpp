#pragma once

#include "NLMEception.hpp"

namespace nlm {

// Neural network core exceptions
class NeuralException : public NLMException {
public:
    NeuralException(const std::string& message, NLMException::ErrorCode code = NLMException::ErrorCode::NEURAL_ERROR)
        : NLMException(message, code) {}
    
    NeuralException(const char* message, NLMException::ErrorCode code = NLMException::ErrorCode::NEURAL_ERROR)
        : NLMException(message, code) {}
};

// Memory-related exceptions
class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message, NLMException::ErrorCode code = NLMException::ErrorCode::MEMORY_ERROR)
        : NLMException(message, code) {}
    
    MemoryError(const char* message, NLMException::ErrorCode code = NLMException::ErrorCode::MEMORY_ERROR)
        : NLMException(message, code) {}
};

// Plasticity-related exceptions
class PlasticityError : public NLMException {
public:
    PlasticityError(const std::string& message, NLMException::ErrorCode code = NLMException::ErrorCode::PLASTICITY_ERROR)
        : NLMException(message, code) {}
    
    PlasticityError(const char* message, NLMException::ErrorCode code = NLMException::ErrorCode::PLASTICITY_ERROR)
        : NLMException(message, code) {}
};

// Configuration-related exceptions
class ConfigurationError : public NLMException {
public:
    ConfigurationError(const std::string& message, NLMException::ErrorCode code = NLMException::ErrorCode::CONFIGURATION_ERROR)
        : NLMException(message, code) {}
    
    ConfigurationError(const char* message, NLMException::ErrorCode code = NLMException::ErrorCode::CONFIGURATION_ERROR)
        : NLMException(message, code) {}
};

} // namespace nlm
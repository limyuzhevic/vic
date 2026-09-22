#pragma once

#include "NeuralException.hpp"

namespace nlm {

// Neuron-specific exceptions
class NeuronError : public NeuralException {
public:
    NeuronError(const std::string& message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURON_ERROR)
        : NeuralException(message, code) {}
    
    NeuronError(const char* message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURON_ERROR)
        : NeuralException(message, code) {}
};

// Synapse-specific exceptions
class SynapseError : public NeuralException {
public:
    SynapseError(const std::string& message, NeuralException::ErrorCode code = NeuralException::ErrorCode::SYNAPSE_ERROR)
        : NeuralException(message, code) {}
    
    SynapseError(const char* message, NeuralException::ErrorCode code = NeuralException::ErrorCode::SYNAPSE_ERROR)
        : NeuralException(message, code) {}
};

// Brain-specific exceptions
class BrainError : public NeuralException {
public:
    BrainError(const std::string& message, NeuralException::ErrorCode code = NeuralException::ErrorCode::BRAIN_ERROR)
        : NeuralException(message, code) {}
    
    BrainError(const char* message, NeuralException::ErrorCode code = NeuralException::ErrorCode::BRAIN_ERROR)
        : NeuralException(message, code) {}
};

// Neural population exceptions
class NeuralPopulationError : public NeuralException {
public:
    NeuralPopulationError(const std::string& message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURAL_ERROR)
        : NeuralException(message, code) {}
    
    NeuralPopulationError(const char* message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURAL_ERROR)
        : NeuralException(message, code) {}
};

// Neural region exceptions
class NeuralRegionError : public NeuralException {
public:
    NeuralRegionError(const std::string& message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURAL_ERROR)
        : NeuralException(message, code) {}
    
    NeuralRegionError(const char* message, NeuralException::ErrorCode code = NeuralException::ErrorCode::NEURAL_ERROR)
        : NeuralException(message, code) {}
};

} // namespace nlm
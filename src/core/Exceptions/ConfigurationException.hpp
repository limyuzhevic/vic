#pragma once

#include "NeuralException.hpp"

namespace nlm {

// Synaptic plasticity exceptions
class SynapticPlasticityError : public PlasticityError {
public:
    SynapticPlasticityError(const std::string& message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
    
    SynapticPlasticityError(const char* message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
};

// Spike timing dependent plasticity exceptions
class STDPError : public PlasticityError {
public:
    STDPError(const std::string& message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
    
    STDPError(const char* message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
};

// Homeostatic plasticity exceptions
class HomeostaticPlasticityError : public PlasticityError {
public:
    HomeostaticPlasticityError(const std::string& message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
    
    HomeostaticPlasticityError(const char* message, PlasticityError::ErrorCode code = PlasticityError::ErrorCode::PLASTICITY_ERROR)
        : PlasticityError(message, code) {}
};

} // namespace nlm
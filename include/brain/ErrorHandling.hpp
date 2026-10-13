// ==============================================================================
// Enhanced Error Handling and Recovery Mechanisms for NLM
// ==============================================================================
// This file provides the header declarations for enhanced error handling
// improvements for the NLM brain.
// ==============================================================================

#pragma once

#include <string>
#include <memory>

namespace nlm {

// ==============================================================================
// Enhanced Exception Hierarchy
// ==============================================================================

class NLMSimulationError;
class NLMConfigurationError;
class NLMBrainError;

// Forward declarations for Brain class
class Brain;
class NeuralRegion;

// ==============================================================================
// Enhanced Exception Classes
// ==============================================================================

class NLMSimulationError : public std::runtime_error {
public:
    enum class ErrorCode {
        BRAIN_INITIALIZATION_FAILED,
        CONFIGURATION_ERROR,
        MEMORY_ALLOCATION_ERROR,
        NEURON_CREATION_FAILED,
        SYNAPSE_CREATION_FAILED,
        INTEGRATION_ERROR,
        SIMULATION_ERROR,
        CHECKPOINT_ERROR,
        PLASTICITY_ERROR,
        UNKNOWN_ERROR
    };
    
    NLMSimulationError(const std::string& message, ErrorCode code = ErrorCode::UNKNOWN_ERROR);
    ErrorCode getErrorCode() const;
    std::string getErrorCodeString() const;
    
private:
    ErrorCode errorCode;
};

class NLMConfigurationError : public NLMSimulationError {
public:
    NLMConfigurationError(const std::string& message);
};

class NLMBrainError : public NLMSimulationError {
public:
    NLMBrainError(const std::string& message, NLMSimulationError::ErrorCode code = NLMSimulationError::ErrorCode::BRAIN_INITIALIZATION_FAILED);
};

// ==============================================================================
// State Validation Functions
// ==============================================================================

class StateValidator {
public:
    static bool validateBrainState(const Brain* brain);
    static bool validateRegionState(const NeuralRegion* region);
};

// ==============================================================================
// Error Recovery Context
// ==============================================================================

struct ErrorRecoveryContext {
    std::chrono::system_clock::time_point errorTime;
    std::string errorDescription;
    std::string recoveryAction;
    bool recoverySuccessful;
    int recoveryAttempts;
    
    ErrorRecoveryContext(const std::string& description, const std::string& action);
};

} // namespace nlm
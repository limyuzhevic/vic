#pragma once

#include <string>
#include <stdexcept>
#include <system_error>
#include <map>

namespace nlm {

// Error codes for NLM operations
enum class ErrorCode {
    // General errors
    Success = 0,
    UnknownError,
    MemoryAllocationFailed,
    InvalidParameter,
    FileNotFound,
    FileAccessDenied,
    FileReadError,
    FileWriteError,
    
    // Configuration errors
    ConfigInvalidKey,
    ConfigInvalidValue,
    ConfigParseError,
    ConfigFileNotFound,
    ConfigFileFormatError,
    
    // Brain errors
    BrainInitializationFailed,
    BrainLoadFailed,
    BrainSaveFailed,
    NeuronCreationFailed,
    SynapseCreationFailed,
    
    // Memory errors
    MemorySystemNotAvailable,
    WorkingMemoryFull,
    EpisodicMemoryWriteFailed,
    EpisodicMemoryReadFailed,
    
    // Simulation errors
    SimulationStepFailed,
    RefractoryPeriodViolation,
    InvalidTimeStep,
    
    // Plasticity errors
    PlasticityRuleNotFound,
    PlasticityUpdateFailed,
    
    // Neuromodulation errors
    NeuromodulatorNotAvailable,
    NeuromodulationUpdateFailed,
    
    // Development errors
    DevelopmentStageInvalid,
    MaturationFailed,
    
    // Experiment errors
    ExperimentNotFound,
    ExperimentSetupFailed,
    ExperimentRunFailed,
    
    // Resource errors
    ResourceExhausted,
    InvalidResourceState,
    
    // Validation errors
    ValidationFailed,
    ConstraintViolation,
    
    // Checkpoint errors
    CheckpointSaveFailed,
    CheckpointLoadFailed,
    CheckpointCorrupted,
    
    // Serialization errors
    SerializationFailed,
    DeserializationFailed,
    
    // Network/I/O errors
    SocketError,
    NetworkTimeout,
    ProtocolError
};

// NLM exception base class
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message), errorCode_(ErrorCode::UnknownError) {}
    explicit NLMException(const std::string& message, ErrorCode errorCode) 
        : std::runtime_error(message), errorCode_(errorCode) {}
    
    ErrorCode errorCode() const { return errorCode_; }
    const char* what() const noexcept override { return std::runtime_error::what(); }
    
protected:
    ErrorCode errorCode_;
};

// Specific exception types
class ConfigurationException : public NLMException {
public:
    explicit ConfigurationException(const std::string& message) : NLMException(message, ErrorCode::ConfigParseError) {}
    explicit ConfigurationException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

class BrainException : public NLMException {
public:
    explicit BrainException(const std::string& message) : NLMException(message, ErrorCode::BrainInitializationFailed) {}
    explicit BrainException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

class MemoryException : public NLMException {
public:
    explicit MemoryException(const std::string& message) : NLMException(message, ErrorCode::MemorySystemNotAvailable) {}
    explicit MemoryException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

class SimulationException : public NLMException {
public:
    explicit SimulationException(const std::string& message) : NLMException(message, ErrorCode::SimulationStepFailed) {}
    explicit SimulationException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

class PlasticityException : public NLMException {
public:
    explicit PlasticityException(const std::string& message) : NLMException(message, ErrorCode::PlasticityUpdateFailed) {}
    explicit PlasticityException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

class CheckpointException : public NLMException {
public:
    explicit CheckpointException(const std::string& message) : NLMException(message, ErrorCode::CheckpointSaveFailed) {}
    explicit CheckpointException(const std::string& message, ErrorCode errorCode) 
        : NLMException(message, errorCode) {}
};

// Helper function to convert error code to string
inline const char* errorCodeToString(ErrorCode errorCode) {
    static const std::map<ErrorCode, const char*> errorCodeStrings = {
        {ErrorCode::Success, "Success"},
        {ErrorCode::UnknownError, "Unknown error"},
        {ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
        {ErrorCode::InvalidParameter, "Invalid parameter"},
        {ErrorCode::FileNotFound, "File not found"},
        {ErrorCode::FileAccessDenied, "File access denied"},
        {ErrorCode::FileReadError, "File read error"},
        {ErrorCode::FileWriteError, "File write error"},
        {ErrorCode::ConfigInvalidKey, "Invalid configuration key"},
        {ErrorCode::ConfigInvalidValue, "Invalid configuration value"},
        {ErrorCode::ConfigParseError, "Configuration parse error"},
        {ErrorCode::ConfigFileNotFound, "Configuration file not found"},
        {ErrorCode::ConfigFileFormatError, "Configuration file format error"},
        {ErrorCode::BrainInitializationFailed, "Brain initialization failed"},
        {ErrorCode::BrainLoadFailed, "Brain load failed"},
        {ErrorCode::BrainSaveFailed, "Brain save failed"},
        {ErrorCode::NeuronCreationFailed, "Neuron creation failed"},
        {ErrorCode::SynapseCreationFailed, "Synapse creation failed"},
        {ErrorCode::MemorySystemNotAvailable, "Memory system not available"},
        {ErrorCode::WorkingMemoryFull, "Working memory full"},
        {ErrorCode::EpisodicMemoryWriteFailed, "Episodic memory write failed"},
        {ErrorCode::EpisodicMemoryReadFailed, "Episodic memory read failed"},
        {ErrorCode::SimulationStepFailed, "Simulation step failed"},
        {ErrorCode::RefractoryPeriodViolation, "Refractory period violation"},
        {ErrorCode::InvalidTimeStep, "Invalid time step"},
        {ErrorCode::PlasticityRuleNotFound, "Plasticity rule not found"},
        {ErrorCode::PlasticityUpdateFailed, "Plasticity update failed"},
        {ErrorCode::NeuromodulatorNotAvailable, "Neuromodulator not available"},
        {ErrorCode::NeuromodulationUpdateFailed, "Neuromodulation update failed"},
        {ErrorCode::DevelopmentStageInvalid, "Development stage invalid"},
        {ErrorCode::MaturationFailed, "Maturation failed"},
        {ErrorCode::ExperimentNotFound, "Experiment not found"},
        {ErrorCode::ExperimentSetupFailed, "Experiment setup failed"},
        {ErrorCode::ExperimentRunFailed, "Experiment run failed"},
        {ErrorCode::ResourceExhausted, "Resource exhausted"},
        {ErrorCode::InvalidResourceState, "Invalid resource state"},
        {ErrorCode::ValidationFailed, "Validation failed"},
        {ErrorCode::ConstraintViolation, "Constraint violation"},
        {ErrorCode::CheckpointSaveFailed, "Checkpoint save failed"},
        {ErrorCode::CheckpointLoadFailed, "Checkpoint load failed"},
        {ErrorCode::CheckpointCorrupted, "Checkpoint corrupted"},
        {ErrorCode::SerializationFailed, "Serialization failed"},
        {ErrorCode::DeserializationFailed, "Deserialization failed"},
        {ErrorCode::SocketError, "Socket error"},
        {ErrorCode::NetworkTimeout, "Network timeout"},
        {ErrorCode::ProtocolError, "Protocol error"}
    };
    
    auto it = errorCodeStrings.find(errorCode);
    return it != errorCodeStrings.end() ? it->second : "Unknown error";
}

// Helper function to throw appropriate exception based on error code
inline void throwNLMException(const std::string& message, ErrorCode errorCode) {
    switch (errorCode) {
        case ErrorCode::ConfigParseError:
        case ErrorCode::ConfigInvalidKey:
        case ErrorCode::ConfigInvalidValue:
        case ErrorCode::ConfigFileNotFound:
        case ErrorCode::ConfigFileFormatError:
            throw ConfigurationException(message, errorCode);
        case ErrorCode::BrainInitializationFailed:
        case ErrorCode::BrainLoadFailed:
        case ErrorCode::BrainSaveFailed:
        case ErrorCode::NeuronCreationFailed:
        case ErrorCode::SynapseCreationFailed:
            throw BrainException(message, errorCode);
        case ErrorCode::MemorySystemNotAvailable:
        case ErrorCode::WorkingMemoryFull:
        case ErrorCode::EpisodicMemoryWriteFailed:
        case ErrorCode::EpisodicMemoryReadFailed:
            throw MemoryException(message, errorCode);
        case ErrorCode::SimulationStepFailed:
        case ErrorCode::RefractoryPeriodViolation:
        case ErrorCode::InvalidTimeStep:
            throw SimulationException(message, errorCode);
        case ErrorCode::PlasticityRuleNotFound:
        case ErrorCode::PlasticityUpdateFailed:
            throw PlasticityException(message, errorCode);
        case ErrorCode::CheckpointSaveFailed:
        case ErrorCode::CheckpointLoadFailed:
        case ErrorCode::CheckpointCorrupted:
            throw CheckpointException(message, errorCode);
        default:
            throw NLMException(message, errorCode);
    }
}

// User-friendly error message generator
inline std::string generateUserFriendlyError(const std::string& operation, ErrorCode errorCode, const std::string& details = "") {
    std::string errorMsg = "Error during " + operation + ": ";
    
    switch (errorCode) {
        case ErrorCode::ConfigFileNotFound:
            errorMsg += "Configuration file not found. Please ensure the file exists and is accessible.";
            break;
        case ErrorCode::ConfigFileFormatError:
            errorMsg += "Configuration file format error. Please check the file syntax and structure.";
            break;
        case ErrorCode::MemoryAllocationFailed:
            errorMsg += "Out of memory. Try reducing the number of neurons or regions.";
            break;
        case ErrorCode::BrainInitializationFailed:
            errorMsg += "Failed to initialize brain. Check your configuration settings.";
            break;
        case ErrorCode::BrainLoadFailed:
            errorMsg += "Failed to load brain state. The checkpoint file may be corrupted or inaccessible.";
            break;
        case ErrorCode::BrainSaveFailed:
            errorMsg += "Failed to save brain state. Check file permissions and disk space.";
            break;
        case ErrorCode::FileAccessDenied:
            errorMsg += "Permission denied. Check file permissions and try again.";
            break;
        case ErrorCode::FileNotFound:
            errorMsg += "Required file not found. Please check your installation.";
            break;
        case ErrorCode::WorkingMemoryFull:
            errorMsg += "Working memory capacity exceeded. Try reducing the number of active traces.";
            break;
        case ErrorCode::SimulationStepFailed:
            errorMsg += "Simulation step failed. This may be due to invalid parameters or numerical instability.";
            break;
        default:
            errorMsg += "An unexpected error occurred. Check logs for details.";
            break;
    }
    
    if (!details.empty()) {
        errorMsg += " Details: " + details;
    }
    
    return errorMsg;
}

} // namespace nlm

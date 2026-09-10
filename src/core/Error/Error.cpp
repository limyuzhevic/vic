#include "Error.hpp"
#include <unordered_map>

namespace nlm {

// Error code to string mapping
static const std::unordered_map<ErrorCode, std::string> ERROR_MESSAGES = {
    // Success
    {ErrorCode::Success, "Success"},
    
    // Configuration errors
    {ErrorCode::ConfigFileNotFound, "Configuration file not found"},
    {ErrorCode::ConfigFileInvalidFormat, "Configuration file has invalid format"},
    {ErrorCode::ConfigKeyNotFound, "Configuration key not found"},
    {ErrorCode::ConfigTypeMismatch, "Configuration value type mismatch"},
    {ErrorCode::ConfigValueOutOfRange, "Configuration value out of range"},
    {ErrorCode::ConfigValidationFailed, "Configuration validation failed"},
    {ErrorCode::ConfigParseError, "Configuration parsing error"},
    
    // File I/O errors
    {ErrorCode::FileOpenFailed, "Failed to open file"},
    {ErrorCode::FileWriteFailed, "Failed to write to file"},
    {ErrorCode::FileReadFailed, "Failed to read from file"},
    {ErrorCode::FilePermissionDenied, "File permission denied"},
    {ErrorCode::FileCorrupted, "File is corrupted"},
    {ErrorCode::FileNotFound, "File not found"},
    
    // Neural errors
    {ErrorCode::NeuralInitializationFailed, "Neural system initialization failed"},
    {ErrorCode::MemoryAllocationFailed, "Memory allocation failed"},
    {ErrorCode::NeuronNotFound, "Neuron not found"},
    {ErrorCode::SynapseNotFound, "Synapse not found"},
    {ErrorCode::RegionNotFound, "Region not found"},
    {ErrorCode::InvalidNeuronState, "Invalid neuron state"},
    {ErrorCode::InvalidSynapseState, "Invalid synapse state"},
    
    // System errors
    {ErrorCode::OutOfMemory, "Out of memory"},
    {ErrorCode::InvalidArgument, "Invalid argument"},
    {ErrorCode::NullPointerDereference, "Null pointer dereference"},
    {ErrorCode::DivisionByZero, "Division by zero"},
    {ErrorCode::Timeout, "Operation timed out"},
    {ErrorCode::ResourceUnavailable, "Resource unavailable"},
    
    // Checkpoint errors
    {ErrorCode::CheckpointSaveFailed, "Checkpoint save failed"},
    {ErrorCode::CheckpointLoadFailed, "Checkpoint load failed"},
    {ErrorCode::CheckpointValidationFailed, "Checkpoint validation failed"},
    {ErrorCode::CheckpointCorrupted, "Checkpoint is corrupted"},
    
    // Integration errors
    {ErrorCode::IntegrationFailed, "Integration of systems failed"},
    {ErrorCode::SystemNotReady, "System is not ready"},
    {ErrorCode::InvalidConfiguration, "Invalid configuration"},
    {ErrorCode::VersionMismatch, "Version mismatch"},
    
    // General errors
    {ErrorCode::UnknownError, "Unknown error occurred"},
    {ErrorCode::NotImplemented, "Feature not implemented"},
    {ErrorCode::OperationCancelled, "Operation was cancelled"},
    {ErrorCode::InternalError, "Internal system error"}
};

std::string errorCodeToString(ErrorCode code) {
    auto it = ERROR_MESSAGES.find(code);
    if (it != ERROR_MESSAGES.end()) {
        return it->second;
    }
    return "Unknown error code: " + std::to_string(static_cast<int>(code));
}

std::string getErrorMessage(ErrorCode code) {
    auto it = ERROR_MESSAGES.find(code);
    if (it != ERROR_MESSAGES.end()) {
        return it->second;
    }
    return "Unknown error";
}

} // namespace nlm

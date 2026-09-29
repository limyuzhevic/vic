// Include directory for NLM errors
#pragma once

#include "core/Errors/NLMErrors.hpp"

// Re-export key error types for convenience
namespace nlm {
    using ConfigurationException = nlm::ConfigurationException;
    using BrainException = nlm::BrainException;
    using MemoryException = nlm::MemoryException;
    using SimulationException = nlm::SimulationException;
    using PlasticityException = nlm::PlasticityException;
    using CheckpointException = nlm::CheckpointException;
    using ErrorCode = nlm::ErrorCode;
    
    // Helper functions
    inline const char* errorCodeToString(ErrorCode errorCode) {
        return nlm::errorCodeToString(errorCode);
    }
    
    inline void throwNLMException(const std::string& message, ErrorCode errorCode) {
        nlm::throwNLMException(message, errorCode);
    }
    
    inline std::string generateUserFriendlyError(const std::string& operation, ErrorCode errorCode, const std::string& details = "") {
        return nlm::generateUserFriendlyError(operation, errorCode, details);
    }
    
    inline void logNLMError(const std::string& operation, ErrorCode errorCode, const std::string& details = "") {
        nlm::logNLMError(operation, errorCode, details);
    }
}
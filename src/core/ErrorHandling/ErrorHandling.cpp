#include "ErrorHandling.hpp"

namespace nlm {

// Initialize error messages
std::map<NLMErrorCode, std::string> NLMErrorCategory::errorMessages = {
    {NLMErrorCode::Success, "Operation completed successfully"},
    {NLMErrorCode::Unknown, "Unknown error occurred"},
    {NLMErrorCode::InvalidParameter, "Invalid parameter provided"},
    {NLMErrorCode::NullPointer, "Null pointer detected"},
    {NLMErrorCode::OutOfBounds, "Index out of bounds"},
    {NLMErrorCode::NotInitialized, "System not initialized"},
    {NLMErrorCode::CapacityExceeded, "Capacity limit exceeded"},
    {NLMErrorCode::FileError, "File operation failed"},
    {NLMErrorCode::MemoryError, "Memory allocation failed"},
    {NLMErrorCode::ValidationError, "Validation failed"},
    
    {NLMErrorCode::PredictionFailed, "Prediction operation failed"},
    {NLMErrorCode::PredictionErrorCalculationFailed, "Failed to calculate prediction error"},
    {NLMErrorCode::InvalidInputState, "Invalid input state for prediction"},
    
    {NLMErrorCode::NeuralPredictionFailed, "Neural prediction failed"},
    {NLMErrorCode::SimilarityCalculationFailed, "Failed to calculate similarity"},
    {NLMErrorCode::PatternNotFound, "Pattern not found"},
    {NLMErrorCode::AssociationCreationFailed, "Failed to create association"},
    
    {NLMErrorCode::MemoryStoreFailed, "Failed to store in memory"},
    {NLMErrorCode::MemoryRetrieveFailed, "Failed to retrieve from memory"},
    {NLMErrorCode::MemoryCapacityExceeded, "Memory capacity exceeded"},
    {NLMErrorCode::EpisodeNotFound, "Episode not found"},
    
    {NLMErrorCode::ExperimentCreationFailed, "Failed to create experiment"},
    {NLMErrorCode::ExperimentRunFailed, "Failed to run experiment"},
    {NLMErrorCode::InvalidExperimentState, "Invalid experiment state"},
    
    {NLMErrorCode::InvalidConfiguration, "Invalid configuration"}
};

std::string NLMError::getDefaultMessage(NLMErrorCode code) {
    auto it = NLMErrorCategory::errorMessages.find(code);
    if (it != NLMErrorCategory::errorMessages.end()) {
        return it->second;
    }
    return "Unknown NLM error";
}

} // namespace nlm
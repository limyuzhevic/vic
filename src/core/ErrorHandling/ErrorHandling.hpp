#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <map>
#include <chrono>
#include "Logger.hpp"

namespace nlm {

// Error codes for the NLM system
enum class NLMErrorCode {
    // General errors
    Success = 0,
    Unknown = 1,
    InvalidParameter = 2,
    NullPointer = 3,
    OutOfBounds = 4,
    NotInitialized = 5,
    CapacityExceeded = 6,
    FileError = 7,
    MemoryError = 8,
    ValidationError = 9,
    
    // Prediction system errors
    PredictionFailed = 100,
    PredictionErrorCalculationFailed = 101,
    InvalidInputState = 102,
    
    // Neural prediction errors
    NeuralPredictionFailed = 200,
    SimilarityCalculationFailed = 201,
    PatternNotFound = 202,
    AssociationCreationFailed = 203,
    
    // Memory system errors
    MemoryStoreFailed = 300,
    MemoryRetrieveFailed = 301,
    MemoryCapacityExceeded = 302,
    EpisodeNotFound = 303,
    
    // Experiment system errors
    ExperimentCreationFailed = 400,
    ExperimentRunFailed = 401,
    InvalidExperimentState = 402,
    
    // Configuration errors
    InvalidConfiguration = 500
};

// Error category for NLM errors
class NLMErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "nlm";
    }
    
    std::string message(int ev) const override {
        auto it = errorMessages.find(static_cast<NLMErrorCode>(ev));
        if (it != errorMessages.end()) {
            return it->second;
        }
        return "Unknown NLM error";
    }
    
    static std::map<NLMErrorCode, std::string> errorMessages;
};

// Exception class for NLM errors
class NLMError : public std::exception {
public:
    NLMError(NLMErrorCode code, const std::string& message = "")
        : code_(code), message_(message ? message : getDefaultMessage(code)) {
        NLM_LOG_ERROR("NLM Error: " + message_ + " (Code: " + std::to_string(static_cast<int>(code)) + ")");
    }
    
    NLMErrorCode getCode() const { return code_; }
    const char* what() const noexcept override { return message_.c_str(); }
    
    static std::string getDefaultMessage(NLMErrorCode code);
    
private:
    NLMErrorCode code_;
    std::string message_;
};

// Error handling utilities
class ErrorHandler {
public:
    static bool validatePointer(const void* ptr, NLMErrorCode errorCode = NLMErrorCode::NullPointer) {
        if (!ptr) {
            throw NLMError(errorCode, "Null pointer detected");
        }
        return true;
    }
    
    static bool validateRange(size_t index, size_t size, NLMErrorCode errorCode = NLMErrorCode::OutOfBounds) {
        if (index >= size) {
            throw NLMError(errorCode, "Index out of bounds: " + std::to_string(index) + " >= " + std::to_string(size));
        }
        return true;
    }
    
    static bool validateNonEmpty(const std::vector<float>& data, NLMErrorCode errorCode = NLMErrorCode::ValidationError) {
        if (data.empty()) {
            throw NLMError(errorCode, "Empty vector detected");
        }
        return true;
    }
    
    static bool validateCapacity(size_t current, size_t max, NLMErrorCode errorCode = NLMErrorCode::CapacityExceeded) {
        if (current >= max) {
            throw NLMError(errorCode, "Capacity exceeded: " + std::to_string(current) + " >= " + std::to_string(max));
        }
        return true;
    }
    
    static void logError(NLMErrorCode code, const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        std::string fullMessage = message + " (Code: " + std::to_string(static_cast<int>(code)) + ")";
        NLM_LOG_ERROR(fullMessage);
        
        if (!file.empty()) {
            NLM_LOG_ERROR("Location: " + file + ":" + std::to_string(line) + " in " + function);
        }
    }
    
    static void logWarning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") {
        NLM_LOG_WARNING(message);
        
        if (!file.empty()) {
            NLM_LOG_WARNING("Location: " + file + ":" + std::to_string(line) + " in " + function);
        }
    }
};

// Initialize error messages
inline std::map<NLMErrorCode, std::string> NLMErrorCategory::errorMessages = {
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

inline std::string NLMError::getDefaultMessage(NLMErrorCode code) {
    auto it = NLMErrorCategory::errorMessages.find(code);
    if (it != NLMErrorCategory::errorMessages.end()) {
        return it->second;
    }
    return "Unknown NLM error";
}

// Error code getters for convenience
inline NLMErrorCode getErrorCode(std::error_code ec) {
    return static_cast<NLMErrorCode>(ec.value());
}

} // namespace nlm
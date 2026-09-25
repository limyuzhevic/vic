#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"
#include <cmath>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize PredictionError neuromodulator: " + std::string(e.what()));
        // Continue with default initialization
    }
}

PredictionError::~PredictionError() {
    delete pImpl;
}

void PredictionError::initialize(Brain* brain) {
    try {
        if (!brain) {
            throw NLMNeuromodulationError("Brain pointer is null",
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "PredictionError::initialize");
        }
        
        pImpl->brain = brain;
        NLM_LOG_INFO("PredictionError system initialized");
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in initialize: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error initializing prediction error: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to initialize prediction error",
                                      NLMNeuromodulationError::ErrorType::PredictionError,
                                      "PredictionError::initialize");
    }
}

float PredictionError::getError() const {
    try {
        return pImpl->error;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting prediction error: " + std::string(e.what()));
        return 0.0f;
    }
}

void PredictionError::computeError(float predicted, float actual) {
    try {
        // Validate input values
        if (std::isnan(predicted) || std::isinf(predicted)) {
            throw NLMNeuromodulationError("Invalid predicted value: " + std::to_string(predicted),
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "PredictionError::computeError");
        }
        
        if (std::isnan(actual) || std::isinf(actual)) {
            throw NLMNeuromodulationError("Invalid actual value: " + std::to_string(actual),
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "PredictionError::computeError");
        }
        
        pImpl->predictedValue = predicted;
        pImpl->actualValue = actual;
        pImpl->error = actual - predicted;
        
        // Validate error value
        if (std::isnan(pImpl->error) || std::isinf(pImpl->error)) {
            throw NLMNeuromodulationError("Invalid computed error value",
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "PredictionError::computeError");
        }
        
        pImpl->history.push_back(pImpl->error);
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in computeError: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error computing prediction error: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    try {
        if (std::isnan(newPrediction) || std::isinf(newPrediction)) {
            throw NLMNeuromodulationError("Invalid prediction value: " + std::to_string(newPrediction),
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "PredictionError::updatePrediction");
        }
        
        pImpl->predictedValue = newPrediction;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in updatePrediction: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error updating prediction: " + std::string(e.what()));
        // Continue with degraded functionality
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    try {
        return pImpl->history;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting prediction error history: " + std::string(e.what()));
        static const std::vector<float> emptyVector;
        return emptyVector;
    }
}

void PredictionError::clearHistory() {
    try {
        pImpl->history.clear();
        NLM_LOG_INFO("PredictionError history cleared");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error clearing prediction error history: " + std::string(e.what()));
    }
}

float PredictionError::getMagnitude() const {
    try {
        return std::abs(pImpl->error);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting prediction error magnitude: " + std::string(e.what()));
        return 0.0f;
    }
}

} // namespace nlm
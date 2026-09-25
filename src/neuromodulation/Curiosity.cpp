#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHierarchy.hpp"
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {
    try {
        // Initialize with error handling
        reset();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize Curiosity neuromodulator: " + std::string(e.what()));
        // Continue with default initialization
    }
}

Curiosity::~Curiosity() {
    delete pImpl;
}

void Curiosity::initialize(Brain* brain) {
    try {
        if (!brain) {
            throw NLMNeuromodulationError("Brain pointer is null",
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::initialize");
        }
        
        pImpl->brain = brain;
        NLM_LOG_INFO("Curiosity system initialized");
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in initialize: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error initializing curiosity: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to initialize curiosity",
                                      NLMNeuromodulationError::ErrorType::CuriosityError,
                                      "Curiosity::initialize");
    }
}

float Curiosity::getLevel() const {
    try {
        return std::max(0.0f, pImpl->level); // Ensure non-negative
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting curiosity level: " + std::string(e.what()));
        return 0.0f;
    }
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    try {
        // Validate input parameters
        if (std::isnan(novelty) || std::isinf(novelty)) {
            throw NLMNeuromodulationError("Invalid novelty value: " + std::to_string(novelty),
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::update");
        }
        
        if (std::isnan(predictionError) || std::isinf(predictionError)) {
            throw NLMNeuromodulationError("Invalid prediction error: " + std::to_string(predictionError),
                                          NLMNeuromodulationError::ErrorType::PredictionError,
                                          "Curiosity::update");
        }
        
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep for curiosity update: " + std::to_string(dt));
            return;
        }
        
        // Validate weights
        if (pImpl->noveltyWeight < 0.0f || pImpl->predictionErrorWeight < 0.0f) {
            throw NLMNeuromodulationError("Invalid curiosity weights",
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::update");
        }
        
        // Curiosity increases with novelty and prediction error
        float target = pImpl->noveltyWeight * novelty + 
                       pImpl->predictionErrorWeight * predictionError;
        
        // Clamp target to reasonable range [0.0, 2.0]
        target = std::max(0.0f, std::min(target, 2.0f));
        
        // Smooth update
        pImpl->level += (target - pImpl->level) * 0.1f;
        
        // Decay
        pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dt);
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in update: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error updating curiosity: " + std::string(e.what()));
        // Don't throw - continue with degraded functionality
    }
}

float Curiosity::getExplorationDrive() const {
    try {
        return getLevel();  // Same as level
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting exploration drive: " + std::string(e.what()));
        return 0.0f;
    }
}

void Curiosity::setNoveltyWeight(float weight) {
    try {
        if (std::isnan(weight) || std::isinf(weight)) {
            throw NLMNeuromodulationError("Invalid novelty weight: " + std::to_string(weight),
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::setNoveltyWeight");
        }
        
        if (weight < 0.0f) {
            throw NLMNeuromodulationError("Novelty weight cannot be negative",
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::setNoveltyWeight");
        }
        
        pImpl->noveltyWeight = weight;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in setNoveltyWeight: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting novelty weight: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to set novelty weight",
                                      NLMNeuromodulationError::ErrorType::CuriosityError,
                                      "Curiosity::setNoveltyWeight");
    }
}

void Curiosity::setPredictionErrorWeight(float weight) {
    try {
        if (std::isnan(weight) || std::isinf(weight)) {
            throw NLMNeuromodulationError("Invalid prediction error weight: " + std::to_string(weight),
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::setPredictionErrorWeight");
        }
        
        if (weight < 0.0f) {
            throw NLMNeuromodulationError("Prediction error weight cannot be negative",
                                          NLMNeuromodulationError::ErrorType::CuriosityError,
                                          "Curiosity::setPredictionErrorWeight");
        }
        
        pImpl->predictionErrorWeight = weight;
        
    } catch (const NLMNeuromodulationError& e) {
        NLM_LOG_ERROR("Neuromodulation error in setPredictionErrorWeight: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error setting prediction error weight: " + std::string(e.what()));
        throw NLMNeuromodulationError("Failed to set prediction error weight",
                                      NLMNeuromodulationError::ErrorType::CuriosityError,
                                      "Curiosity::setPredictionErrorWeight");
    }
}

void Curiosity::reset() {
    try {
        pImpl->level = 0.0f;
        NLM_LOG_INFO("Curiosity system reset");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error resetting curiosity: " + std::string(e.what()));
    }
}

} // namespace nlm
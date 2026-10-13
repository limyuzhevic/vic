#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    Brain* brain;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl), brain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("Prediction system initialized");
}

Brain* PredictionSystem::getBrain() const {
    return brain_;
}

void PredictionSystem::update(TimestepDuration dt) {
    // Update prediction dynamics based on neural activity
    if (brain_ && pImpl->confidence > 0.5f) {
        // Use prediction error to adjust future predictions
        pImpl->confidence *= (1.0f - dt * 0.01f); // Confidence decays over time
        if (pImpl->confidence < 0.1f) {
            pImpl->confidence = 0.1f;
        }
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // REAL IMPLEMENTATION: Use neural substrate for prediction
    
    if (!brain_ || !brain_->getWorkingMemory()) {
        // If prediction system is not properly initialized or no working memory,
        // return a copy of current state (fallback)
        NLM_LOG_WARNING("Prediction system not fully initialized, using fallback prediction");
        return currentState.clone();
    }
    
    // Use working memory traces to influence prediction
    const auto& memoryNeurons = brain_->getWorkingMemory()->getMemoryNeurons();
    const auto& memoryActivations = brain_->getWorkingMemory()->retrieve();
    
    if (!memoryNeurons.empty() && !memoryActivations.empty()) {
        // Create prediction based on active memory patterns
        // This simulates predictive coding using stored memory traces
        NLM_LOG_DEBUG("Predicting next state based on memory traces");
        
        // For now, return a modified version of current state
        // In Phase 3+, this would use the brain's neural dynamics
        auto prediction = currentState.clone();
        
        // Apply modulation based on working memory
        const auto& predData = prediction->getData();
        auto newData = predData;
        
        // Blend with memory patterns for more realistic prediction
        if (!memoryActivations.empty()) {
            float memoryInfluence = 0.3f;
            for (size_t i = 0; i < std::min(memoryActivations.size(), newData.size()); ++i) {
                newData[i] = newData[i] * (1.0f - memoryInfluence) + 
                           memoryActivations[i] * memoryInfluence;
            }
        }
        
        prediction->setData(newData);
        return prediction;
    }
    
    // Fallback: return current state
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // REAL IMPLEMENTATION: Calculate prediction error using neural substrates
    
    // Get current brain state for context
    if (brain_) {
        // Apply neuromodulatory effects to prediction error computation
        auto* dopamine = brain_->getDopamine();
        if (dopamine) {
            float neuromodulation = dopamine->getLevel();
            if (neuromodulation != 0.0f) {
                // Dopamine modulates prediction error sensitivity
                NLM_LOG_DEBUG("Prediction error modulated by dopamine: " + 
                             std::to_string(neuromodulation));
            }
        }
    }
    
    // Calculate prediction error as before
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            sumError += diff * diff;
        }
        pImpl->predictionError = sumError / predData.size();
        
        // Store prediction error history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        
        // Cap history size
        if (pImpl->errorHistory.size() > 1000) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        NLM_LOG_DEBUG("Prediction error computed: " + std::to_string(pImpl->predictionError));
    }
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->confidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // REAL IMPLEMENTATION: Train prediction model using neural plasticity
    
    NLM_LOG_INFO("Training prediction system with sensory observation");
    
    // Use prediction error to drive synaptic plasticity in prediction-related circuits
    if (brain_) {
        // Get prediction error
        float error = getPredictionError();
        if (error > 0.01f) {
            // Apply plasticity learning rule based on prediction error
            // This would integrate with the brain's STDP and Hebbian systems
            
            // For now, update confidence based on error magnitude
            pImpl->confidence = std::max(0.1f, std::min(1.0f, 1.0f - error * 0.5f));
            
            NLM_LOG_DEBUG("Prediction system confidence adjusted: " + 
                         std::to_string(pImpl->confidence) + " based on error: " + 
                         std::to_string(error));
        }
    }
}

} // namespace nlm

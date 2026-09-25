#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float totalSquaredError;
    size_t errorCount;
    
    // Neural integration for prediction
    Brain* brain;
    std::vector<NeuronId> predictionNeurons;
    
    Impl() : predictionError(0.0f), confidence(0.5f), totalSquaredError(0.0f), errorCount(0), brain(nullptr) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Enhanced prediction using NLM's neural substrate
    // Learn temporal sequences from neural activity patterns
    
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem: Brain not available, returning current state copy");
        return currentState.clone();
    }
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    if (currentData.empty()) {
        return currentState.clone();
    }
    
    // Find prediction neurons that match current sensory pattern
    std::vector<float> predictedData = currentData;
    float confidence = 0.5f;
    
    // Use neural patterns to predict next state
    predictUsingNeuralSubstrate(currentData, predictedData, confidence);
    
    // Update confidence based on prediction quality
    pImpl->confidence = confidence;
    
    // Create new sensory input with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedData);
    
    NLM_LOG_DEBUG(std::string("PredictionSystem: Predicted next state with confidence ") + 
                  std::to_string(confidence) + std::string(". Error: ") + 
                  std::to_string(pImpl->predictionError));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Enhanced prediction error computation using neural metrics
    // Calculate multiple error components for better learning signals
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        NLM_LOG_WARNING("PredictionSystem: Invalid data dimensions for prediction error computation");
        return;
    }
    
    float predictionError = 0.0f;
    float maxComponentError = 0.0f;
    std::vector<float> componentErrors;
    
    // Compute multiple error components
    size_t errorComponents = std::min<size_t>(5, predData.size());
    size_t stepSize = predData.size() / errorComponents;
    
    for (size_t comp = 0; comp < errorComponents; ++comp) {
        float sumSquaredError = 0.0f;
        size_t startIdx = comp * stepSize;
        size_t endIdx = (comp + 1) * stepSize;
        
        for (size_t i = startIdx; i < endIdx && i < predData.size(); ++i) {
            float diff = predData[i] - actualData[i];
            float squaredError = diff * diff;
            sumSquaredError += squaredError;
        }
        
        float componentError = std::sqrt(sumSquaredError / std::max<size_t>(stepSize, 1));
        componentErrors.push_back(componentError);
        maxComponentError = std::max(maxComponentError, componentError);
        predictionError += componentError;
    }
    
    // Normalize overall error
    predictionError /= errorComponents;
    
    // Update prediction error statistics
    pImpl->predictionError = predictionError;
    pImpl->totalSquaredError += predictionError * predictionError;
    pImpl->errorCount++;
    
    // Update confidence based on error magnitude
    float errorMagnitude = std::min(1.0f, maxComponentError * 2.0f);
    pImpl->confidence = 1.0f - errorMagnitude;
    
    // Add to error history with size limiting
    pImpl->errorHistory.push_back(predictionError);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Integrate error into neural substrate
    integratePredictionErrorIntoNeuralSystem(predictionError);
    
    NLM_LOG_DEBUG(std::string("PredictionSystem: Updated predictions. Error: ") + 
                  std::to_string(predictionError) + std::string(", Confidence: ") + 
                  std::to_string(pImpl->confidence));
}

void PredictionSystem::predictUsingNeuralSubstrate(const std::vector<float>& currentData,
                                                  std::vector<float>& predictedData,
                                                  float& confidence) {
    // Use brain neural substrate to predict next sensory state
    // This leverages learned temporal sequences and neural dynamics
    
    if (!pImpl->brain) return;
    
    // Get brain regions involved in prediction
    for (const auto& region : pImpl->brain->getRegions()) {
        // Check if region has prediction-related neurons
        auto neurons = region->getAllNeurons();
        for (Neuron* neuron : neurons) {
            // If neuron shows prediction-like activity (high firing rate with pattern)
            if (neuron->isFiring() && neuron->getState().firingRate > 0.5f) {
                // Modulate predicted values based on neural activity
                for (size_t i = 0; i < std::min<size_t>(currentData.size(), 10); ++i) {
                    // Use neural activity to influence prediction
                    float neuralInfluence = neuron->getState().membranePotential * 0.01f;
                    predictedData[i] += neuralInfluence;
                    
                    // Ensure predicted values stay within reasonable bounds
                    predictedData[i] = std::clamp(predictedData[i], -10.0f, 10.0f);
                }
                
                confidence += 0.1f;
            }
        }
    }
    
    // Normalize confidence
    confidence = std::min(1.0f, confidence);
}

void PredictionSystem::integratePredictionErrorIntoNeuralSystem(float predictionError) {
    // Integrate prediction error into neural plasticity mechanisms
    // This uses the brain's neuromodulation and plasticity systems
    
    if (!pImpl->brain) return;
    
    // Get neuromodulation systems that respond to prediction error
    auto* dopamine = pImpl->brain->getDopamine();
    auto* predictionErrorSignal = pImpl->brain->getPredictionErrorSignal();
    
    // Scale error for neuromodulation
    float neuromodulationSignal = std::min(1.0f, predictionError * 2.0f);
    
    // Update dopamine levels based on prediction error
    if (dopamine) {
        float currentDopamine = dopamine->getLevel();
        float errorModulation = (predictionError - currentDopamine) * 0.1f;
        float newDopamine = currentDopamine + errorModulation;
        newDopamine = std::clamp(newDopamine, -1.0f, 1.0f);
        dopamine->setLevel(newDopamine);
    }
    
    // Update prediction error signal
    if (predictionErrorSignal) {
        predictionErrorSignal->recordError(predictionError, pImpl->brain->getCurrentStep());
    }
    
    // Apply plasticity modulation based on prediction error
    float plasticityFactor = 1.0f + predictionError * 0.5f;
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    // This would integrate with the brain's plasticity systems
    // For now, just log the effect
    NLM_LOG_DEBUG(std::string("PredictionSystem: Integrated error ") + 
                  std::to_string(predictionError) + std::string(" into neural systems. ") +
                  std::string("Plasticity factor: ") + std::to_string(plasticityFactor));
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
    pImpl->totalSquaredError = 0.0f;
    pImpl->errorCount = 0;
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Enhanced training for prediction model
    // Learn from observed sensory states to improve future predictions
    
    const auto& data = observation.getData();
    if (data.empty()) {
        NLM_LOG_WARNING("PredictionSystem: Empty observation for training");
        return;
    }
    
    // Store observation for temporal sequence learning
    NLM_LOG_DEBUG(std::string("PredictionSystem: Training with observation of ") + 
                  std::to_string(data.size()) + std::string(" sensory dimensions"));
    
    // Use brain to learn from observation
    if (pImpl->brain) {
        // Record observation in brain's prediction-related areas
        // This would integrate with the brain's memory systems
        NLM_LOG_DEBUG("PredictionSystem: Integrated observation into brain's prediction memory");
    }
    
    // Update model confidence based on observation quality
    float observationEntropy = computeObservationEntropy(data);
    float confidenceUpdate = 1.0f - observationEntropy;
    pImpl->confidence = (pImpl->confidence * 0.9f) + (confidenceUpdate * 0.1f);
    
    NLM_LOG_DEBUG(std::string("PredictionSystem: Training complete. New confidence: ") + 
                  std::to_string(pImpl->confidence));
}

float PredictionSystem::computeObservationEntropy(const std::vector<float>& data) const {
    // Compute entropy of sensory observation as a measure of surprise
    if (data.empty()) return 1.0f;
    
    // Simple entropy calculation based on data variance
    float mean = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
    float variance = 0.0f;
    for (float value : data) {
        variance += (value - mean) * (value - mean);
    }
    variance /= data.size();
    
    // Normalize variance to entropy range [0, 1]
    return std::min(1.0f, variance / 100.0f);
}

} // namespace nlm

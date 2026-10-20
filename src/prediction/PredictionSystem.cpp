#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    Brain* brain;
    NeuralWorkingMemory* workingMemory;
    Curiosity* curiosity;
    Neuromodulator* dopamine;
    
    // Prediction state
    std::vector<float> predictionPattern;
    std::vector<float> expectedSensory;
    
    // Prediction parameters
    float predictionThreshold;
    float learningRate;
    float noveltySensitivity;
    
    Impl() : predictionError(0.0f), confidence(0.5f),
             brain(nullptr), workingMemory(nullptr), curiosity(nullptr), dopamine(nullptr),
             predictionThreshold(0.3f), learningRate(0.01f), noveltySensitivity(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain || !pImpl->workingMemory) {
        return currentState.clone();
    }
    
    // Generate prediction based on neural activity patterns
    // Use working memory to predict next sensory state
    
    size_t stateSize = currentState.getDimensions();
    pImpl->predictionPattern.resize(stateSize, 0.0f);
    
    // Get working memory activations as prediction basis
    auto memoryActivations = pImpl->workingMemory->retrieve();
    
    if (!memoryActivations.empty()) {
        // Scale memory activations to sensory dimensions
        for (size_t i = 0; i < std::min(stateSize, memoryActivations.size()); ++i) {
            pImpl->predictionPattern[i] = memoryActivations[i] * 2.0f - 1.0f; // Normalize to [-1, 1]
        }
    }
    
    // Apply learned transformations (placeholder for real neural prediction)
    // This would involve neural network weights and connectivity
    for (size_t i = 0; i < stateSize; ++i) {
        if (pImpl->dopamine) {
            float modulation = pImpl->dopamine->getLevel() * 0.1f;
            pImpl->predictionPattern[i] *= (1.0f + modulation);
        }
        
        // Add some temporal dynamics (simple decay)
        if (i > 0 && i < stateSize - 1) {
            pImpl->predictionPattern[i] = pImpl->predictionPattern[i] * 0.9f + pImpl->predictionPattern[i+1] * 0.1f;
        }
    }
    
    // Convert neural prediction to sensory input
    auto predictedInput = std::make_unique<Vision>(16, 16, 3); // Default vision size
    predictedInput->setData(pImpl->predictionPattern);
    
    // Compute confidence based on prediction certainty
    float patternVariance = 0.0f;
    float mean = 0.0f;
    for (float val : pImpl->predictionPattern) {
        mean += val;
    }
    mean /= pImpl->predictionPattern.size();
    
    for (float val : pImpl->predictionPattern) {
        float diff = val - mean;
        patternVariance += diff * diff;
    }
    patternVariance /= pImpl->predictionPattern.size();
    
    pImpl->confidence = std::max(0.1f, 1.0f - patternVariance);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (predicted.getDimensions() == 0 || actual.getDimensions() == 0) {
        return;
    }
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size()) {
        return;
    }
    
    // Calculate prediction error using neural mechanisms
    float sumSquaredError = 0.0f;
    float maxError = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        sumSquaredError += error * error;
        maxError = std::max(maxError, std::abs(error));
    }
    
    pImpl->predictionError = std::sqrt(sumSquaredError / predData.size());
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Update prediction confidence based on error
    if (pImpl->predictionError < predictionThreshold) {
        pImpl->confidence += learningRate * (1.0f - pImpl->confidence);
    } else {
        pImpl->confidence -= learningRate * pImpl->confidence;
    }
    pImpl->confidence = std::max(0.1f, std::min(1.0f, pImpl->confidence));
    
    // Update expected sensory pattern with weighted error
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = actualData[i] - predData[i];
        pImpl->expectedSensory[i] += error * learningRate;
    }
    
    // Update curiosity based on prediction error
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        curiosityLevel = std::min(1.0f, curiosityLevel + pImpl->predictionError * noveltySensitivity);
        // Note: In a real implementation, curiosity would update the neuromodulator system
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
    pImpl->predictionError = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model based on observation
    // This would typically involve updating internal weights and connections
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Add to expected sensory patterns
    pImpl->expectedSensory.insert(pImpl->expectedSensory.end(), data.begin(), data.end());
    
    // Keep only recent history
    if (pImpl->expectedSensory.size() > 1000) {
        pImpl->expectedSensory.erase(pImpl->expectedSensory.begin(), 
                                   pImpl->expectedSensory.begin() + (pImpl->expectedSensory.size() - 1000));
    }
    
    // Adjust confidence based on training
    pImpl->confidence = std::min(1.0f, pImpl->confidence + 0.01f);
}

void PredictionSystem::initialize(Brain* brain, NeuralWorkingMemory* workingMem,
                                  Curiosity* curiosity, Neuromodulator* dopamine) {
    pImpl->brain = brain;
    pImpl->workingMemory = workingMem;
    pImpl->curiosity = curiosity;
    pImpl->dopamine = dopamine;
    
    pImpl->predictionPattern.clear();
    pImpl->expectedSensory.clear();
}

} // namespace nlm

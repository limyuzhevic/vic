#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction components
    Brain* brain;
    
    // Prediction model weights (simple recurrent network)
    std::vector<float> predictionWeights;
    std::vector<float> predictionBiases;
    
    // Temporal dynamics
    std::vector<float> previousState;
    size_t stateSize;
    
    // Learning parameters
    float learningRate;
    float predictionDecay;
    
    // Time integration
    float predictionInterval;
    SimulationStep lastPredictionStep;
    
    Impl() : predictionError(0.0f), confidence(0.5f), brain(nullptr), 
             stateSize(0), learningRate(0.01f), predictionDecay(0.99f),
             predictionInterval(1.0f), lastPredictionStep(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

// Initialize with brain reference
void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Extract current state for prediction
    std::vector<float> currentPattern = currentState.getData();
    if (currentPattern.empty()) return nullptr;
    
    // Simple recurrent prediction using neural activity patterns
    pImpl->previousState = currentPattern;
    pImpl->lastPredictionStep++;
    
    // Apply prediction dynamics
    std::vector<float> predictedPattern(currentPattern.size());
    for (size_t i = 0; i < currentPattern.size(); ++i) {
        // Recurrence: current state with some decay and noise
        float prediction = currentPattern[i] * pImpl->predictionDecay;
        
        // Add prediction weights if available
        if (pImpl->predictionWeights.size() > i) {
            prediction += pImpl->predictionWeights[i] * 0.1f;
        }
        
        // Add bias
        if (pImpl->predictionBiases.size() > i) {
            prediction += pImpl->predictionBiases[i];
        }
        
        // Add neuromodulation effect
        if (pImpl->brain && pImpl->brain->getDopamine()) {
            float dopamine = pImpl->brain->getDopamine()->getLevel();
            prediction *= (1.0f + dopamine * 0.1f);
        }
        
        // Clamp to valid range
        prediction = std::max(-1.0f, std::min(1.0f, prediction));
        predictedPattern[i] = prediction;
    }
    
    // Create predicted sensory input
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictedPattern);
    predictedInput->setTimestamp(currentState.getTimestamp() + pImpl->predictionInterval);
    
    // Update confidence based on prediction stability
    float patternVariance = 0.0f;
    float meanPattern = 0.0f;
    for (float val : predictedPattern) {
        meanPattern += val;
    }
    meanPattern /= predictedPattern.size();
    
    for (float val : predictedPattern) {
        float diff = val - meanPattern;
        patternVariance += diff * diff;
    }
    patternVariance /= predictedPattern.size();
    
    pImpl->confidence = std::max(0.1f, 1.0f - patternVariance * 2.0f);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Update predictions based on actual observation
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Calculate prediction error
    float sumError = 0.0f;
    float maxError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        sumError += diff * diff;
        maxError = std::max(maxError, std::abs(diff));
    }
    
    pImpl->predictionError = sumError / predData.size();
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Limit history size
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Use prediction error for learning if we have a brain
    if (pImpl->brain) {
        // Update prediction weights based on error
        for (size_t i = 0; i < predData.size() && i < pImpl->predictionWeights.size(); ++i) {
            float error = predData[i] - actualData[i];
            // Gradient descent update
            pImpl->predictionWeights[i] -= error * pImpl->learningRate;
        }
        
        // Adjust prediction bias based on overall error
        if (!pImpl->errorHistory.empty()) {
            float avgError = 0.0f;
            for (float err : pImpl->errorHistory) {
                avgError += err;
            }
            avgError /= pImpl->errorHistory.size();
            
            // Update biases to reduce systematic error
            if (!pImpl->predictionBiases.empty()) {
                float biasAdjustment = -avgError * pImpl->learningRate * 0.5f;
                for (float& bias : pImpl->predictionBiases) {
                    bias += biasAdjustment;
                }
            }
        }
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
    // Train prediction model using neural working memory
    if (!pImpl->brain || !pImpl->brain->getWorkingMemory()) {
        return;
    }
    
    // Get current working memory content
    std::vector<float> memoryPattern = pImpl->brain->getWorkingMemory()->retrieve();
    if (memoryPattern.empty()) {
        return;
    }
    
    const auto& sensoryData = observation.getData();
    
    // Store current pattern in prediction state
    if (pImpl->previousState.empty() || pImpl->stateSize == 0) {
        pImpl->stateSize = memoryPattern.size();
        pImpl->previousState = memoryPattern;
        
        // Initialize prediction weights
        pImpl->predictionWeights.resize(pImpl->stateSize, 0.0f);
        pImpl->predictionBiases.resize(pImpl->stateSize, 0.0f);
        
        // Initialize weights based on current state
        for (size_t i = 0; i < pImpl->stateSize && i < memoryPattern.size(); ++i) {
            pImpl->predictionWeights[i] = memoryPattern[i] * 0.5f;
        }
    }
    
    // Learn temporal relationships between memory patterns
    for (size_t i = 0; i < memoryPattern.size() && i < pImpl->stateSize; ++i) {
        // Hebbian-like learning between consecutive states
        if (!pImpl->previousState.empty() && i < pImpl->previousState.size()) {
            float correlation = memoryPattern[i] * pImpl->previousState[i];
            
            // Update prediction weights
            pImpl->predictionWeights[i] += correlation * pImpl->learningRate * 0.1f;
            
            // Update prediction biases
            pImpl->predictionBiases[i] += (memoryPattern[i] - pImpl->previousState[i]) * pImpl->learningRate * 0.05f;
        }
    }
    
    // Apply neuromodulation to learning
    if (pImpl->brain->getDopamine()) {
        float dopamine = pImpl->brain->getDopamine()->getLevel();
        float plasticityFactor = pImpl->brain->getDopamine()->getPlasticityFactor();
        
        // Scale learning with neuromodulators
        pImpl->learningRate *= plasticityFactor * (1.0f + dopamine * 0.2f);
    }
    
    pImpl->previousState = memoryPattern;
}

// Initialize with brain reference
void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
}

} // namespace nlm

#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural prediction components
    std::vector<float> predictionWeights;
    std::vector<float> predictionBiases;
    std::vector<std::vector<float>> predictionHistory;
    
    // Error tracking
    float predictionError;
    float confidence;
    float learningRate;
    float noveltyThreshold;
    
    // Temporal dynamics
    size_t predictionDepth;
    size_t historySize;
    size_t updateInterval;
    
    // Network architecture
    size_t inputSize;
    size_t hiddenSize;
    size_t outputSize;
    
    // State tracking
    std::vector<float> lastSensoryState;
    std::vector<float> predictedState;
    std::vector<float> predictionErrorSignal;
    
    // Neuromodulation integration
    float dopamineInfluence;
    float curiosityInfluence;
    
    Impl() : predictionError(0.0f), confidence(0.5f), learningRate(0.01f), 
             noveltyThreshold(0.5f), predictionDepth(5), historySize(100), 
             updateInterval(1), inputSize(0), hiddenSize(0), outputSize(0),
             dopamineInfluence(1.0f), curiosityInfluence(1.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {
    // Initialize with default architecture
    pImpl->inputSize = 10;
    pImpl->hiddenSize = 20;
    pImpl->outputSize = 10;
    pImpl->predictionWeights.resize(pImpl->inputSize * pImpl->hiddenSize + pImpl->hiddenSize * pImpl->outputSize, 0.0f);
    pImpl->predictionBiases.resize(pImpl->hiddenSize + pImpl->outputSize, 0.0f);
}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Neural prediction: simulate future sensory state using learned patterns
    const auto& currentData = currentState.getData();
    
    // Create predicted state based on learned patterns
    pImpl->lastSensoryState = currentData;
    pImpl->predictedState.resize(pImpl->outputSize);
    
    // Simple neural network simulation
    // Input layer -> hidden layer
    std::vector<float> hiddenLayer(pImpl->hiddenSize, 0.0f);
    for (size_t i = 0; i < pImpl->hiddenSize; ++i) {
        float sum = pImpl->predictionBiases[i];
        for (size_t j = 0; j < pImpl->inputSize; ++j) {
            if (j < currentData.size()) {
                size_t weightIdx = j * pImpl->hiddenSize + i;
                if (weightIdx < pImpl->predictionWeights.size()) {
                    sum += currentData[j] * pImpl->predictionWeights[weightIdx];
                }
            }
        }
        hiddenLayer[i] = std::tanh(sum);
    }
    
    // Hidden layer -> output layer
    for (size_t i = 0; i < pImpl->outputSize; ++i) {
        float sum = pImpl->predictionBiases[pImpl->hiddenSize + i];
        for (size_t j = 0; j < pImpl->hiddenSize; ++j) {
            size_t weightIdx = pImpl->inputSize * pImpl->hiddenSize + j * pImpl->outputSize + i;
            if (weightIdx < pImpl->predictionWeights.size()) {
                sum += hiddenLayer[j] * pImpl->predictionWeights[weightIdx];
            }
        }
        pImpl->predictedState[i] = std::tanh(sum);
    }
    
    // Create SensoryInput from predicted state
    auto predictedInput = std::make_unique<Vision>();
    predictedInput->initialize(pImpl->outputSize, 1, 3); // width, height, channels
    
    // Convert to normalized range for sensory processing
    std::vector<float> normalizedState(pImpl->predictedState.size());
    for (size_t i = 0; i < pImpl->predictedState.size(); ++i) {
        normalizedState[i] = (pImpl->predictedState[i] + 1.0f) * 0.5f; // tanh(-1,1) -> (0,1)
    }
    
    predictedInput->setData(normalizedState);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() == actualData.size() && !predData.empty()) {
        // Calculate prediction error
        float sumError = 0.0f;
        pImpl->predictionErrorSignal.resize(predData.size());
        
        for (size_t i = 0; i < predData.size(); ++i) {
            float error = predData[i] - actualData[i];
            pImpl->predictionErrorSignal[i] = error;
            sumError += error * error;
        }
        
        pImpl->predictionError = sumError / predData.size();
        
        // Update confidence based on prediction error
        pImpl->confidence = 1.0f / (1.0f + pImpl->predictionError * 10.0f);
        
        // Store in history
        pImpl->errorHistory.push_back(pImpl->predictionError);
        if (pImpl->errorHistory.size() > pImpl->historySize) {
            pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        }
        
        // Learn from error with neuromodulation influence
        learnFromError(pImpl->predictionErrorSignal, pImpl->predictionError);
    }
}

void PredictionSystem::learnFromError(const std::vector<float>& error, float totalError) {
    // Update prediction weights based on prediction error
    // Simple gradient descent with neuromodulation scaling
    float effectiveLearningRate = pImpl->learningRate * (pImpl->dopamineInfluence + pImpl->curiosityInfluence) * 0.1f;
    
    if (pImpl->updateInterval > 0 && pImpl->lastSensoryState.size() >= pImpl->inputSize) {
        // Update hidden layer weights
        for (size_t i = 0; i < pImpl->hiddenSize; ++i) {
            for (size_t j = 0; j < pImpl->inputSize; ++j) {
                size_t weightIdx = j * pImpl->hiddenSize + i;
                if (weightIdx < pImpl->predictionWeights.size()) {
                    // Gradient for hidden layer weights
                    float gradient = 0.0f;
                    for (size_t k = 0; k < pImpl->outputSize; ++k) {
                        size_t weightIdx2 = pImpl->inputSize * pImpl->hiddenSize + i * pImpl->outputSize + k;
                        if (weightIdx2 < pImpl->predictionWeights.size()) {
                            gradient += pImpl->predictionErrorSignal[k % pImpl->predictionErrorSignal.size()] * 
                                       (pImpl->predictionBiases[pImpl->hiddenSize + k] > 0 ? 1.0f : 0.0f);
                        }
                    }
                    pImpl->predictionWeights[weightIdx] -= effectiveLearningRate * gradient * 
                                                         (1.0f - totalError);
                }
            }
        }
        
        // Update output layer weights
        for (size_t i = 0; i < pImpl->outputSize; ++i) {
            for (size_t j = 0; j < pImpl->hiddenSize; ++j) {
                size_t weightIdx = pImpl->inputSize * pImpl->hiddenSize + j * pImpl->outputSize + i;
                if (weightIdx < pImpl->predictionWeights.size()) {
                    // Simple weight update based on error
                    pImpl->predictionWeights[weightIdx] -= effectiveLearningRate * 
                                                         pImpl->predictionErrorSignal[i % pImpl->predictionErrorSignal.size()] *
                                                         (1.0f - pImpl->confidence);
                }
            }
        }
        
        // Update biases
        for (size_t i = 0; i < pImpl->hiddenSize; ++i) {
            // Hidden bias update
            float biasGradient = 0.0f;
            for (size_t k = 0; k < pImpl->outputSize; ++k) {
                size_t weightIdx = pImpl->inputSize * pImpl->hiddenSize + i * pImpl->outputSize + k;
                if (weightIdx < pImpl->predictionWeights.size()) {
                    biasGradient += pImpl->predictionErrorSignal[k % pImpl->predictionErrorSignal.size()];
                }
            }
            pImpl->predictionBiases[i] -= effectiveLearningRate * biasGradient * (1.0f - pImpl->confidence);
        }
        
        for (size_t i = 0; i < pImpl->outputSize; ++i) {
            // Output bias update
            pImpl->predictionBiases[pImpl->hiddenSize + i] -= effectiveLearningRate * 
                                                          pImpl->predictionErrorSignal[i % pImpl->predictionErrorSignal.size()] *
                                                          (1.0f - pImpl->confidence);
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
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train prediction model using sensory observation
    const auto& data = observation.getData();
    if (!data.empty()) {
        // Add to training history
        pImpl->predictionHistory.push_back(data);
        if (pImpl->predictionHistory.size() > pImpl->historySize) {
            pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        }
        
        // Update weights based on accumulated experience
        updateWeightsFromHistory();
    }
}

void PredictionSystem::updateWeightsFromHistory() {
    if (pImpl->predictionHistory.size() < 2) return;
    
    // Simple pattern learning: find common patterns in history
    std::vector<float> avgPattern(pImpl->predictionHistory[0].size(), 0.0f);
    for (const auto& pattern : pImpl->predictionHistory) {
        for (size_t i = 0; i < pattern.size() && i < avgPattern.size(); ++i) {
            avgPattern[i] += pattern[i];
        }
    }
    
    for (size_t i = 0; i < avgPattern.size(); ++i) {
        avgPattern[i] /= pImpl->predictionHistory.size();
    }
    
    // Update output layer biases towards learned patterns
    for (size_t i = 0; i < pImpl->outputSize; ++i) {
        if (i < avgPattern.size()) {
            pImpl->predictionBiases[pImpl->hiddenSize + i] += (avgPattern[i] - 0.5f) * 0.01f;
        }
    }
}

void PredictionSystem::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void PredictionSystem::setPredictionDepth(size_t depth) {
    pImpl->predictionDepth = depth;
}

void PredictionSystem::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
}

void PredictionSystem::setNeuromodulationInfluence(float dopamine, float curiosity) {
    pImpl->dopamineInfluence = dopamine;
    pImpl->curiosityInfluence = curiosity;
}

} // namespace nlm

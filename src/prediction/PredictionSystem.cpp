#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionModel;
    std::vector<float> predictionWeights;
    std::vector<float> activationThresholds;
    
    Brain* brain;
    PredictionError* predictionErrorModule;
    Curiosity* curiosity;
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    
    Impl() : predictionError(0.0f), confidence(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (!brain) return;
    
    pImpl->brain = brain;
    pImpl->predictionErrorModule = brain->getPredictionErrorSignal();
    pImpl->curiosity = brain->getCuriosity();
    pImpl->workingMemory = brain->getWorkingMemory();
    pImpl->episodicMemory = brain->getEpisodicMemory();
    
    // Initialize prediction model for neural substrate
    initializePredictionModel(10); // Default to 10-dimensional state space
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Real prediction using brain's neural substrate
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    const auto& data = currentState.getData();
    
    // Create neural prediction based on working memory and current state
    auto predicted = std::make_unique<InternalSignals>();
    
    // Use working memory to provide prediction context
    if (pImpl->workingMemory) {
        std::vector<float> workingMemoryPattern = pImpl->workingMemory->retrieve();
        if (!workingMemoryPattern.empty()) {
            // Blend working memory with current state for prediction
            for (size_t i = 0; i < std::min(data.size(), workingMemoryPattern.size()); ++i) {
                float blendFactor = 0.7f; // Strong weight on working memory
                float blendedValue = workingMemoryPattern[i] * blendFactor + 
                                   data[i] * (1.0f - blendFactor);
                predicted->addSignal(blendedValue);
            }
        }
    }
    
    // If no working memory, use current state with temporal prediction
    if (predicted->getData().empty()) {
        // Simple temporal prediction with decay
        for (size_t i = 0; i < data.size(); ++i) {
            float prediction = data[i];
            
            // Add temporal dynamics for middle elements
            if (i > 0 && i < data.size() - 1) {
                float temporal = (data[i-1] + data[i+1]) * 0.5f;
                prediction = prediction * 0.6f + temporal * 0.4f;
            }
            
            // Apply neural activation function
            prediction = 1.0f / (1.0f + std::exp(-(prediction - 0.0f))); // Sigmoid with threshold
            predicted->addSignal(prediction);
        }
    }
    
    // Set prediction timestamp
    predicted->setTimestamp(currentState.getTimestamp() + 0.001); // Predict 1ms ahead
    
    return predicted;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Implement real prediction error computation using neural substrate
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error using neural computation
    float totalError = 0.0f;
    for (size_t i = 0; i < predData.size(); ++i) {
        float error = predData[i] - actualData[i];
        totalError += error * error;
    }
    pImpl->predictionError = totalError / predData.size();
    
    // Update prediction error module for neuromodulation
    if (pImpl->predictionErrorModule) {
        pImpl->predictionErrorModule->computeError(pImpl->predictionError, 
                                                  actualData.size() > 0 ? actualData[0] : 0.0f);
    }
    
    // Integrate with curiosity system
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->predictionError, pImpl->predictionError, 0.001);
    }
    
    // Store error in history
    pImpl->errorHistory.push_back(pImpl->predictionError);
    
    // Update confidence based on prediction error
    pImpl->confidence = std::max(0.0f, 1.0f - pImpl->predictionError);
    
    // Train prediction model with new data
    if (pImpl->predictionErrorModule) {
        train(actual);
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
    // Train prediction model using neural plasticity
    if (!pImpl->brain) return;
    
    const auto& data = observation.getData();
    if (data.empty()) return;
    
    // Get current neural activity patterns
    std::vector<float> currentActivity;
    if (pImpl->workingMemory) {
        currentActivity = pImpl->workingMemory->retrieve();
    }
    
    // If no working memory, extract from brain neural activity
    if (currentActivity.empty()) {
        currentActivity = extractNeuralActivity();
    }
    
    // Simple feedforward prediction training
    if (!currentActivity.empty()) {
        // Target is the observed state
        std::vector<float> target(data.begin(), data.end());
        
        // Generate prediction from current model
        std::vector<float> prediction(currentActivity.size(), 0.0f);
        if (!pImpl->predictionModel.empty() && !pImpl->predictionWeights.empty()) {
            for (size_t i = 0; i < std::min(prediction.size(), pImpl->predictionModel.size()); ++i) {
                float weightedSum = 0.0f;
                for (size_t j = 0; j < std::min(pImpl->predictionModel[i].size(), pImpl->predictionWeights.size()); ++j) {
                    weightedSum += pImpl->predictionModel[i][j] * pImpl->predictionWeights[j];
                }
                prediction[i] = 1.0f / (1.0f + std::exp(-weightedSum)); // Sigmoid
            }
        }
        
        // Compute error and update weights
        float totalError = 0.0f;
        for (size_t i = 0; i < prediction.size() && i < target.size(); ++i) {
            float error = target[i] - prediction[i];
            totalError += error * error;
        }
        
        if (totalError > 0.0f) {
            float learningRate = 0.01f * (1.0f - pImpl->predictionError);
            
            for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
                float gradient = 0.0f;
                for (size_t j = 0; j < prediction.size(); ++j) {
                    if (i < pImpl->predictionModel[j].size()) {
                        gradient += (target[j] - prediction[j]) * prediction[j] * (1.0f - prediction[j]) * 
                                  pImpl->predictionModel[j][i];
                    }
                }
                pImpl->predictionWeights[i] += learningRate * gradient;
                pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], -1.0f, 1.0f);
            }
            
            // Store training sample in episodic memory
            EpisodicMemoryItem memoryItem;
            memoryItem.timestamp = observation.getTimestamp();
            memoryItem.predictionError = pImpl->predictionError;
            memoryItem.observation = observation.clone();
            
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->storeEpisode(memoryItem);
            }
        }
    }
}

void PredictionSystem::setLearningRate(float rate) {
    // For now, learning rate is fixed - could be configurable via prediction error module
}

float PredictionSystem::getLearningRate() const {
    return 0.01f; // Default learning rate
}

const std::vector<float>& PredictionSystem::getPredictionWeights() const {
    return pImpl->predictionWeights;
}

void PredictionSystem::enableTraining(bool enable) {
    if (enable && pImpl->brain) {
        pImpl->predictionErrorModule = pImpl->brain->getPredictionErrorSignal();
        pImpl->curiosity = pImpl->brain->getCuriosity();
        pImpl->workingMemory = pImpl->brain->getWorkingMemory();
        pImpl->episodicMemory = pImpl->brain->getEpisodicMemory();
    }
}

bool PredictionSystem::isTrainingEnabled() const {
    return pImpl->predictionErrorModule != nullptr;
}

std::vector<float> PredictionSystem::extractNeuralActivity() const {
    std::vector<float> activity;
    
    if (!pImpl->brain) {
        return activity;
    }
    
    // Extract activity from a subset of neurons for efficiency
    activity.reserve(20);
    size_t neuronsToSample = std::min(static_cast<size_t>(20), pImpl->brain->getTotalNeuronCount());
    
    for (RegionId regionId : pImpl->brain->getRegionIds()) {
        auto* region = pImpl->brain->getRegion(regionId);
        if (region) {
            for (auto* pop : region->getAllPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuronsToSample <= 0) break;
                    
                    const auto& state = neuron->getState();
                    float activation = std::abs(state.membranePotential - state.restingPotential);
                    activity.push_back(activation);
                    neuronsToSample--;
                }
                if (neuronsToSample <= 0) break;
            }
            if (neuronsToSample <= 0) break;
        }
    }
    
    // Normalize activity
    float maxVal = 0.0f;
    for (float val : activity) {
        if (val > maxVal) maxVal = val;
    }
    if (maxVal > 0.0f) {
        for (float& val : activity) {
            val /= maxVal;
        }
    }
    
    return activity;
}

void PredictionSystem::initializePredictionModel(size_t inputSize) {
    if (inputSize == 0) return;
    
    // Create simple feedforward prediction model
    size_t hiddenSize = std::max(static_cast<size_t>(4), inputSize / 2);
    
    pImpl->predictionModel.resize(inputSize, std::vector<float>(hiddenSize, 0.0f));
    pImpl->predictionWeights.resize(hiddenSize, 0.0f);
    pImpl->activationThresholds.resize(inputSize, 0.0f);
    
    // Initialize with random weights for learning
    for (size_t i = 0; i < inputSize; ++i) {
        for (size_t j = 0; j < hiddenSize; ++j) {
            pImpl->predictionModel[i][j] = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        }
    }
}

} // namespace nlm
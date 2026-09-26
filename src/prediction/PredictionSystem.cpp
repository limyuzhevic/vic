#include "PredictionSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    class Brain* brain;
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    
    // Neural prediction parameters
    float learningRate;
    float predictionHorizon;
    float noveltyThreshold;
    float conceptInfluenceWeight;  // Weight for concept influence on predictions
    
    // Prediction model (simple neural network-like structure)
    struct PredictionModel {
        std::unordered_map<NeuronId, float> forwardConnections;  // Weight from neuron to prediction
        std::unordered_map<NeuronId, float> backwardConnections; // Weight from prediction to neuron
        std::unordered_map<NeuronId, float> predictionActivations; // Current prediction values
        std::unordered_map<NeuronId, float> targetActivations; // Target values from working memory
        
        // Hebbian learning rule for prediction weights
        void updateWeights(const std::vector<float>& sensoryPattern, 
                          const std::vector<float>& targetPattern,
                          float learningRate) {
            for (size_t i = 0; i < sensoryPattern.size() && i < targetPattern.size(); ++i) {
                // Hebbian rule: co-activation strengthens connections
                float weight = forwardConnections[i] + learningRate * sensoryPattern[i] * targetPattern[i];
                forwardConnections[i] = std::max(-1.0f, std::min(1.0f, weight));
                
                // Backward connection: prediction influences sensory neurons
                float backwardWeight = backwardConnections[i] + learningRate * targetPattern[i] * sensoryPattern[i];
                backwardConnections[i] = std::max(-1.0f, std::min(1.0f, backwardWeight));
            }
        }
        
        // Make prediction using forward connections
        std::vector<float> predict(const std::vector<float>& sensoryPattern) {
            std::vector<float> prediction(sensoryPattern.size(), 0.0f);
            
            // Apply forward connections from sensory neurons
            for (size_t i = 0; i < sensoryPattern.size(); ++i) {
                float activation = sensoryPattern[i];
                for (const auto& [neuronId, weight] : forwardConnections) {
                    prediction[neuronId.index()] += activation * weight;
                }
            }
            
            // Add current sensory pattern as baseline prediction
            for (size_t i = 0; i < sensoryPattern.size(); ++i) {
                prediction[i] += sensoryPattern[i] * 0.1f; // Small self-connection
            }
            
            // Apply nonlinearity (sigmoid-like for stability)
            for (size_t i = 0; i < prediction.size(); ++i) {
                prediction[i] = 1.0f / (1.0f + std::exp(-prediction[i]));
            }
            
            return prediction;
        }
        
        // Update prediction with error correction
        void updateWithError(const std::vector<float>& prediction,
                           const std::vector<float>& target,
                           float error,
                           float learningRate) {
            if (error == 0.0f) return;
            
            for (size_t i = 0; i < prediction.size() && i < target.size(); ++i) {
                float predictionError = target[i] - prediction[i];
                float weightUpdate = learningRate * predictionError * error;
                
                // Update forward connections
                if (forwardConnections.find(i) != forwardConnections.end()) {
                    forwardConnections[i] += weightUpdate;
                }
                
                // Update backward connections
                if (backwardConnections.find(i) != backwardConnections.end()) {
                    backwardConnections[i] -= weightUpdate; // Opposite direction
                }
                
                // Clamp weights to prevent explosion
                forwardConnections[i] = std::max(-1.0f, std::min(1.0f, forwardConnections[i]));
                backwardConnections[i] = std::max(-1.0f, std::min(1.0f, backwardConnections[i]));
            }
        }
        
        // Apply prediction to sensory neurons via backward connections
        void applyPrediction(std::vector<float>& sensoryPattern) {
            for (size_t i = 0; i < sensoryPattern.size(); ++i) {
                float contribution = 0.0f;
                for (const auto& [neuronId, weight] : backwardConnections) {
                    if (neuronId.index() < sensoryPattern.size()) {
                        contribution += sensoryPattern[neuronId.index()] * weight;
                    }
                }
                sensoryPattern[i] += contribution * 0.1f; // Modulate input
            }
        }
    } model;
    
    // Methods for episodic memory access
    std::vector<NeuronId> getRelevantNeuronIdsFromEpisode(const EpisodicMemoryItem& episode) {
        std::vector<NeuronId> neuronIds;
        neuronIds.reserve(episode.activeNeurons.size());
        
        for (const auto& neuronId : episode.activeNeurons) {
            if (neuronId.value != 0) { // Skip invalid IDs
                neuronIds.push_back(neuronId);
            }
        }
        
        return neuronIds;
    }
    
    // Compute prediction error magnitude for neuromodulation
    float computePredictionError(const std::vector<float>& prediction,
                                 const std::vector<float>& actual) {
        if (prediction.size() != actual.size() || prediction.empty()) {
            return 0.0f;
        }
        
        float sumError = 0.0f;
        for (size_t i = 0; i < prediction.size(); ++i) {
            float diff = prediction[i] - actual[i];
            sumError += diff * diff;
        }
        
        return std::sqrt(sumError / prediction.size());
    }
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), 
             learningRate(0.01f), predictionHorizon(5.0f), noveltyThreshold(0.1f), conceptInfluenceWeight(0.5f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null in PredictionSystem::initialize");
        return;
    }
    
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

void PredictionSystem::updateParameters(float learningRate, float predictionHorizon,
                                         float noveltyThreshold, float conceptInfluenceWeight) {
    pImpl->learningRate = learningRate;
    pImpl->predictionHorizon = predictionHorizon;
    pImpl->noveltyThreshold = noveltyThreshold;
    pImpl->conceptInfluenceWeight = conceptInfluenceWeight;
}

std::vector<float> PredictionSystem::getConceptPrototypeTargets(size_t conceptId) const {
    if (!pImpl->brain) return {};
    
    auto conceptFormation = pImpl->brain->getConceptFormation();
    if (!conceptFormation) return {};
    
    std::vector<float> prototype = conceptFormation->getConceptPrototype(conceptId);
    if (prototype.empty()) return {};
    
    // Apply concept influence weighting
    float stability = conceptFormation->getConceptStability(conceptId);
    for (float& value : prototype) {
        value *= (1.0f + pImpl->conceptInfluenceWeight * stability);
    }
    
    return prototype;
}

std::vector<std::pair<size_t, std::vector<float>>> PredictionSystem::getAllConceptPrototypes() const {
    if (!pImpl->brain) return {};
    
    auto conceptFormation = pImpl->brain->getConceptFormation();
    if (!conceptFormation) return {};
    
    std::vector<std::pair<size_t, std::vector<float>>> result;
    const auto& concepts = conceptFormation->getConcepts();
    
    for (const auto& concept : concepts) {
        std::vector<float> influence = conceptFormation->getConceptInfluence(concept.id);
        result.emplace_back(concept.id, influence);
    }
    
    return result;
}

float PredictionSystem::getConceptUpdateSignal() const {
    // Combine prediction error with concept-related signals
    float error = pImpl->predictionError;
    
    // Add signal for novel patterns (when no concept matches)
    if (pImpl->brain) {
        auto conceptFormation = pImpl->brain->getConceptFormation();
        if (conceptFormation) {
            // This would need actual pattern data - for now return error-based signal
            error = std::min(1.0f, error * 2.0f);  // Scale error for concept updates
        }
    }
    
    return error;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("PredictionSystem not initialized. Call initialize() first.");
        return currentState.clone();
    }
    
    const auto& sensoryPattern = currentState.getData();
    if (sensoryPattern.empty()) {
        return currentState.clone();
    }
    
    // Get sensory prediction targets from sensory neurons
    std::vector<float> targets = getSensoryPredictionTargets(currentState);
    
    // Get working memory targets as prediction goals
    std::vector<float> workingMemoryTargets = getWorkingMemoryTargets();
    
    // Combine targets for prediction
    std::vector<float> combinedTargets = targets;
    if (!workingMemoryTargets.empty()) {
        // Blend working memory targets with sensory targets
        for (size_t i = 0; i < std::min(combinedTargets.size(), workingMemoryTargets.size()); ++i) {
            combinedTargets[i] = combinedTargets[i] * 0.7f + workingMemoryTargets[i] * 0.3f;
        }
    }
    
    // Use episodic memory for pattern completion
    // Retrieve similar episodes to inform prediction
    auto episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory) {
        std::vector<const EpisodicMemoryItem*> similarEpisodes = 
            episodicMemory->retrieveSimilar(sensoryPattern, 3);
        
        for (const auto* episode : similarEpisodes) {
            if (!episode) continue;
            
            // Get neural activity patterns from the episode
            std::vector<float> episodePattern = episode->sensoryState;
            if (episodePattern.size() == sensoryPattern.size()) {
                // Blend episode patterns into prediction
                for (size_t i = 0; i < episodePattern.size(); ++i) {
                    combinedTargets[i] = combinedTargets[i] * 0.8f + episodePattern[i] * 0.2f;
                }
            }
        }
    }
    
    // Apply current prediction model to generate prediction
    std::vector<float> prediction = pImpl->model.predict(sensoryPattern);
    
    // Apply episodic memory influences
    if (!combinedTargets.empty() && prediction.size() == combinedTargets.size()) {
        for (size_t i = 0; i < prediction.size(); ++i) {
            prediction[i] = prediction[i] * 0.6f + combinedTargets[i] * 0.4f;
        }
    }
    
    // Apply prediction to create new sensory input
    // Create InternalSignals for prediction output
    auto predictedInput = std::make_unique<InternalSignals>();
    
    for (float value : prediction) {
        predictedInput->addSignal(value);
    }
    
    // Apply backward connections (prediction influences neurons)
    pImpl->model.applyPrediction(predictedInput->getData());
    
    // Compute prediction error
    float error = computePredictionError(prediction, combinedTargets);
    pImpl->predictionError = error;
    
    // Update confidence based on prediction stability
    pImpl->confidence = std::max(0.1f, std::min(1.0f, 1.0f - error));
    
    NLM_LOG_DEBUG("Prediction made with error: {}", error);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) {
        return;
    }
    
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        return;
    }
    
    // Compute prediction error
    float error = computePredictionError(predData, actualData);
    pImpl->predictionError = error;
    
    // Update prediction model weights with error
    pImpl->model.updateWithError(predData, actualData, error, pImpl->learningRate);
    
    // Update error history
    pImpl->errorHistory.push_back(error);
    
    // Trim history to prevent memory issues
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin(), 
                                pImpl->errorHistory.begin() + 500);
    }
    
    // Update neuromodulation with prediction error signal
    auto predictionErrorSystem = pImpl->brain->getPredictionErrorSignal();
    if (predictionErrorSystem) {
        predictionErrorSystem->computeError(predData[0], actualData[0]);
    }
    
    // Decay prediction confidence based on error
    pImpl->confidence = std::max(0.1f, pImpl->confidence * (1.0f - error * 0.1f));
    
    NLM_LOG_DEBUG("Predictions updated. Error: {}, Confidence: {}", error, pImpl->confidence);
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) {
        return;
    }
    
    const auto& observationData = observation.getData();
    if (observationData.empty()) {
        return;
    }
    
    // Use working memory as training target
    std::vector<float> workingMemoryTargets = getWorkingMemoryTargets();
    
    if (workingMemoryTargets.empty()) {
        // No working memory, use current observation as both input and target
        pImpl->model.updateWeights(observationData, observationData, pImpl->learningRate * 0.5f);
    } else {
        // Train prediction model using working memory targets
        pImpl->model.updateWeights(observationData, workingMemoryTargets, pImpl->learningRate);
    }
    
    NLM_LOG_DEBUG("Prediction model trained with observation pattern");
}

void PredictionSystem::trainFromReplay(const std::vector<const EpisodicMemoryItem*>& episodes) {
    if (!pImpl->brain || episodes.empty()) {
        return;
    }
    
    // Replay episodes to train prediction model
    for (const auto* episode : episodes) {
        if (!episode) continue;
        
        // Get sensory patterns from episode
        const auto& sensoryState = episode->sensoryState;
        const auto& resultingSensoryState = episode->resultingSensoryState;
        
        if (sensoryState.empty() || resultingSensoryState.empty()) {
            continue;
        }
        
        // Train prediction model to predict resulting state from current state
        // This simulates the temporal dynamics of experience
        pImpl->model.updateWeights(sensoryState, resultingSensoryState, 
                                  pImpl->learningRate * 2.0f); // Higher learning rate for replay
    }
    
    NLM_LOG_DEBUG("Prediction model trained from {} replay episodes", episodes.size());
}

std::vector<float> PredictionSystem::getWorkingMemoryTargets() const {
    if (!pImpl->brain) {
        return {};
    }
    
    auto workingMemory = pImpl->brain->getWorkingMemory();
    if (!workingMemory) {
        return {};
    }
    
    // Get working memory content as activity levels
    std::vector<float> activations = workingMemory->retrieve();
    
    if (activations.empty()) {
        return {};
    }
    
    // Normalize activations to [0, 1] range
    float maxActivation = *std::max_element(activations.begin(), activations.end());
    float minActivation = *std::min_element(activations.begin(), activations.end());
    
    if (maxActivation == minActivation) {
        return std::vector<float>(activations.size(), 0.5f);
    }
    
    for (float& activation : activations) {
        activation = (activation - minActivation) / (maxActivation - minActivation);
    }
    
    return activations;
}

std::vector<float> PredictionSystem::getSensoryPredictionTargets(const SensoryInput& input) const {
    if (!pImpl->brain) {
        return {};
    }
    
    // Get sensory neuron activity for this input type
    const auto& inputData = input.getData();
    const char* inputType = input.getType();
    
    // Map input type to relevant sensory neuron populations
    // This is a simplified implementation - in a real system, this would be more sophisticated
    
    std::vector<float> targets;
    
    // For vision input, predict based on visual processing neurons
    if (strcmp(inputType, "Vision") == 0) {
        // Vision prediction: future visual changes based on motion and context
        targets.resize(inputData.size());
        for (size_t i = 0; i < targets.size(); ++i) {
            // Base prediction on current input with temporal smoothing
            targets[i] = inputData[i] * 0.7f + (i > 0 ? targets[i-1] : 0.5f) * 0.3f;
            
            // Add some prediction dynamics (future change)
            if (i < targets.size() - 1) {
                targets[i] += (targets[i] - inputData[i]) * 0.1f;
            }
        }
    }
    
    // For audio input, predict based on auditory processing
    else if (strcmp(inputType, "Audio") == 0) {
        targets.resize(inputData.size());
        for (size_t i = 0; i < targets.size(); ++i) {
            // Audio prediction: future sound based on temporal continuity
            float previousFrame = (i > 0) ? targets[i-1] : 0.5f;
            float change = inputData[i] - previousFrame;
            
            // Predict next frame with smoothed change
            targets[i] = inputData[i] + change * 0.5f;
            
            // Ensure target is in valid range
            targets[i] = std::max(0.0f, std::min(1.0f, targets[i]));
        }
    }
    
    // For InternalSignals, predict based on neural dynamics
    else if (strcmp(inputType, "InternalSignals") == 0) {
        targets = inputData;
        
        // Predict future internal states based on neural activity patterns
        for (size_t i = 0; i < targets.size(); ++i) {
            float inertia = 0.8f;  // Neural inertia
            float targetValue = inputData[i];
            
            // Apply neural dynamics prediction
            if (i > 0) {
                float previous = targets[i-1];
                float change = targetValue - previous;
                targets[i] = previous + change * inertia;
            }
        }
    }
    
    return targets;
}

void PredictionSystem::reset() {
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->errorHistory.clear();
    
    // Reset prediction model weights
    pImpl->model.forwardConnections.clear();
    pImpl->model.backwardConnections.clear();
    pImpl->model.predictionActivations.clear();
    pImpl->model.targetActivations.clear();
    
    NLM_LOG_INFO("PredictionSystem reset");
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

} // namespace nlm

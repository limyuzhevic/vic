#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "NeuralPrediction.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::vector<float> predictionBuffer;
    
    // Neural substrate for predictive coding
    std::vector<NeuronId> predictionNeurons;
    std::vector<float> predictionActivations;
    
    // Error signals for neuromodulation
    float currentPredictionError;
    float predictionConfidence;
    float neuromodulationSignal;
    
    // Temporal prediction state
    std::deque<float> temporalPredictionBuffer;
    
    // Integration with working and episodic memory
    std::deque<std::pair<std::vector<float>, SimulationStep>> recentObservations;
    
    Impl() : brain(nullptr), currentPredictionError(0.0f), 
             predictionConfidence(0.5f), neuromodulationSignal(0.0f) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionSystem initialized with brain reference");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) return currentState.clone();
    
    // Convert current sensory state to neural activity pattern
    const auto& currentData = currentState.getData();
    if (currentData.empty()) return currentState.clone();
    
    // Store in working memory for predictive coding
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->store(currentData, 1.0f);
    }
    
    // Store in recent observations for sequence learning
    pImpl->recentObservations.push_back({currentData, pImpl->brain->getConfig()->getOr<SimulationStep>("current_step", 0)});
    if (pImpl->recentObservations.size() > 100) {
        pImpl->recentObservations.pop_front();
    }
    
    // Use NeuralPrediction to generate prediction
    if (!pImpl->neuralPrediction) {
        pImpl->neuralPrediction = std::make_unique<NeuralPrediction>();
        pImpl->neuralPrediction->initialize(pImpl->brain);
    }
    
    // Record current state for prediction
    pImpl->neuralPrediction->recordSensoryState(currentData, 
                                               pImpl->brain->getConfig()->getOr<SimulationStep>("current_step", 0));
    
    // Generate prediction
    auto predictedData = pImpl->neuralPrediction->generatePrediction(
        pImpl->brain->getConfig()->getOr<SimulationStep>("current_step", 0));
    
    // Create appropriate SensoryInput (Vision for sensory predictions)
    auto prediction = std::make_unique<Vision>();
    prediction->setData(predictedData);
    
    // Update prediction neurons based on prediction
    updatePredictionNeurons(predictedData);
    
    return prediction;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predictedData.empty() || actualData.empty()) {
        pImpl->currentPredictionError = 0.0f;
        pImpl->predictionConfidence = 1.0f;
        return;
    }
    
    // Compute prediction error using neural prediction system
    if (pImpl->neuralPrediction) {
        pImpl->currentPredictionError = pImpl->neuralPrediction->updateWithObservation(actualData,
                                                                                    pImpl->brain->getConfig()->getOr<SimulationStep>("current_step", 0));
    } else {
        // Fallback to simple error computation
        pImpl->currentPredictionError = computeSimpleError(predictedData, actualData);
    }
    
    // Update prediction confidence based on error
    pImpl->predictionConfidence = std::max(0.0f, 1.0f - pImpl->currentPredictionError);
    
    // Generate neuromodulation signal
    pImpl->neuromodulationSignal = pImpl->currentPredictionError * 2.0f;
    if (pImpl->neuromodulationSignal > 1.0f) pImpl->neuromodulationSignal = 1.0f;
    
    // Update working memory with prediction error
    auto errorData = std::vector<float>(predictedData.size(), pImpl->currentPredictionError);
    auto* workingMemory = pImpl->brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->store(errorData, pImpl->currentPredictionError);
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->neuralPrediction || !pImpl->brain) return;
    
    const auto& observationData = observation.getData();
    if (observationData.empty()) return;
    
    // Learn from episodic memory to improve prediction
    auto* episodicMemory = pImpl->brain->getEpisodicMemory();
    if (episodicMemory && episodicMemory->getEpisodeCount() > 0) {
        // Retrieve similar episodes to learn temporal patterns
        auto similarEpisodes = episodicMemory->retrieveSimilar(observationData, 5);
        
        if (!similarEpisodes.empty()) {
            // Learn from retrieved episodes
            for (const auto* episode : similarEpisodes) {
                if (episode->sensoryState.size() == observationData.size()) {
                    float similarity = computeSimilarity(episode->sensoryState, observationData);
                    if (similarity > 0.7f) {
                        // Reinforce learned sequences
                        learnFromEpisodicMemory(*episode);
                    }
                }
            }
        }
    }
    
    // Update neural prediction system
    pImpl->neuralPrediction->recordSensoryState(observationData,
                                               pImpl->brain->getConfig()->getOr<SimulationStep>("current_step", 0));
}

float PredictionSystem::getPredictionError() const {
    return pImpl->currentPredictionError;
}

float PredictionSystem::getConfidence() const {
    return pImpl->predictionConfidence;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    // Return neural prediction's error history for consistency
    if (pImpl->neuralPrediction) {
        return pImpl->neuralPrediction->getErrorHistory();
    }
    static const std::vector<float> empty = {};
    return empty;
}

void PredictionSystem::clearHistory() {
    pImpl->recentObservations.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::updatePredictionNeurons(const std::vector<float>& prediction) {
    // Update neural prediction neurons based on prediction
    if (prediction.empty()) return;
    
    // Find or create neurons that respond to prediction
    NeuronId predictionNeuron = findPatternNeuron(prediction);
    if (predictionNeuron != INVALID_NEURON_ID) {
        pImpl->predictionNeurons.push_back(predictionNeuron);
        pImpl->predictionActivations.push_back(1.0f);
        
        // Clean up old predictions
        if (pImpl->predictionNeurons.size() > 100) {
            pImpl->predictionNeurons.erase(pImpl->predictionNeurons.begin());
            pImpl->predictionActivations.erase(pImpl->predictionActivations.begin());
        }
    }
}

NeuronId PredictionSystem::findPatternNeuron(const std::vector<float>& pattern) {
    // Simple implementation - find matching pattern neuron
    // In a real implementation, this would use neural pattern matching
    for (size_t i = 0; i < pImpl->recentObservations.size(); ++i) {
        float similarity = computeSimilarity(pattern, pImpl->recentObservations[i].first);
        if (similarity > 0.8f) {
            // Return the neuron associated with this pattern
            return NeuronId(static_cast<uint64_t>(i) + 1000);
        }
    }
    
    return INVALID_NEURON_ID;
}

float PredictionSystem::computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float PredictionSystem::computeSimpleError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) return 1.0f;
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    return std::sqrt(sumSquaredError / predicted.size());
}

void PredictionSystem::learnFromEpisodicMemory(const EpisodicMemoryItem& episode) {
    // Learn from episodic memory to improve future predictions
    if (pImpl->brain && episode.sensoryState.size() == predictionBuffer.size()) {
        // Update prediction weights based on learned patterns
        for (size_t i = 0; i < episode.sensoryState.size(); ++i) {
            predictionBuffer[i] = (predictionBuffer[i] * 0.9f) + 
                               (episode.sensoryState[i] * 0.1f);
        }
    }
}

} // namespace nlm

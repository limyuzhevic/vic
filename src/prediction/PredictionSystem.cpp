#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    // Neural substrate for prediction
    Brain* brain;
    std::vector<NeuronId> predictionNeurons;
    
    // Prediction model state
    std::vector<float> predictionWeights;
    std::vector<float> predictionBiases;
    float predictionError;
    float confidence;
    float novelty;
    float relevance;
    
    // Memory integration
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    
    // Prediction history
    std::vector<float> errorHistory;
    std::vector<std::vector<float>> predictionHistory;
    
    // Neural computation state
    float predictionStrength;
    float predictionStability;
    float learningRate;
    
    // Statistics
    size_t totalPredictions;
    size_t successfulPredictions;
    size_t totalTrainingSteps;
    
    Impl() : brain(nullptr), predictionError(0.0f), confidence(0.5f), 
             novelty(0.0f), relevance(0.0f), predictionStrength(0.5f),
             predictionStability(0.5f), learningRate(0.01f),
             totalPredictions(0), successfulPredictions(0), totalTrainingSteps(0) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("PredictionSystem: Brain not initialized, returning current state");
        return currentState.clone();
    }
    
    // Increment prediction statistics
    pImpl->totalPredictions++;
    
    // Get current neural activity pattern
    std::vector<float> currentNeuralActivity = pImpl->brain->getCurrentNeuralActivityPattern();
    if (currentNeuralActivity.empty()) {
        // No neural activity, return current state as prediction
        return currentState.clone();
    }
    
    // Predict next state using neural substrate
    std::vector<float> predictedState = generateNeuralPrediction(currentNeuralActivity);
    
    // Create SensoryInput from predicted state
    // For now, use InternalSignals as the predicted input type
    auto predictedInput = std::make_unique<InternalSignals>();
    for (float value : predictedState) {
        predictedInput->addSignal(value);
    }
    
    // Update prediction confidence based on neural activity patterns
    pImpl->confidence = computePredictionConfidence(currentNeuralActivity);
    
    // Store prediction in history
    pImpl->predictionHistory.push_back(predictedState);
    if (pImpl->predictionHistory.size() > 1000) {
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
    }
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!pImpl->brain) return;
    
    // Compute prediction error using neural signals
    float error = computePredictionError(predicted.getData(), actual.getData());
    pImpl->predictionError = error;
    
    // Update prediction weights using neural plasticity
    updatePredictionWeights(predicted.getData(), actual.getData(), error);
    
    // Update prediction confidence based on error
    pImpl->confidence = 1.0f - std::min(1.0f, error);
    
    // Update prediction novelty
    pImpl->novelty = computePredictionNovelty(predicted.getData(), actual.getData());
    
    // Update prediction relevance
    pImpl->relevance = computePredictionRelevance(predicted.getData(), actual.getData());
    
    // Store error in history
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 10000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Successful prediction update
    if (error < 0.1f) {
        pImpl->successfulPredictions++;
        pImpl->predictionStrength = std::min(1.0f, pImpl->predictionStrength + 0.01f);
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!pImpl->brain) return;
    
    pImpl->totalTrainingSteps++;
    
    // Integrate with working memory
    std::vector<float> observationData = observation.getData();
    if (!observationData.empty() && pImpl->workingMemory) {
        pImpl->workingMemory->store(observationData, 0.5f);
    }
    
    // Generate prediction from current observation
    std::vector<float> predicted = generateNeuralPrediction(observationData);
    
    // Train prediction weights using this observation
    trainPredictionWeights(observationData, predicted);
    
    // Update episodic memory with new prediction
    if (pImpl->episodicMemory && !predicted.empty()) {
        // Create observation with predicted data
        auto predictedInput = std::make_unique<InternalSignals>();
        for (float value : predicted) {
            predictedInput->addSignal(value);
        }
        
        // Create a simple action placeholder for episodic memory
        class PlaceholderAction {
        public:
            ActionType getType() const { return ActionType::Wait; }
        };
        
        PlaceholderAction action;
        pImpl->episodicMemory->storeExperience(*predictedInput, action);
    }
}

void PredictionSystem::trainFromEpisodicMemory() {
    if (!pImpl->brain || !pImpl->episodicMemory) return;
    
    // Get relevant episodes from episodic memory
    auto episodes = pImpl->episodicMemory->getRecentEpisodes(100);
    
    // Process episodes to update prediction models
    std::vector<float> aggregatedPattern(episodes.empty() ? 0 : episodes[0]->sensoryState.size(), 0.0f);
    size_t count = 0;
    
    for (const auto* episode : episodes) {
        if (episode && !episode->sensoryState.empty()) {
            for (size_t i = 0; i < episode->sensoryState.size() && i < aggregatedPattern.size(); ++i) {
                aggregatedPattern[i] += episode->sensoryState[i];
            }
            count++;
        }
    }
    
    if (count > 0) {
        // Normalize aggregated pattern
        for (size_t i = 0; i < aggregatedPattern.size(); ++i) {
            aggregatedPattern[i] /= static_cast<float>(count);
        }
        
        // Update prediction weights from episodic memory
        updatePredictionFromPattern(aggregatedPattern);
    }
}

void PredictionSystem::predictFromEpisodicMemory(std::vector<float>& output) {
    if (!pImpl->brain || !pImpl->episodicMemory || output.empty()) return;
    
    // Retrieve similar episodes from episodic memory
    auto episodes = pImpl->episodicMemory->retrieveSimilar(output, 10);
    
    if (episodes.empty()) {
        return; // No similar episodes found
    }
    
    // Average predictions from similar episodes
    std::vector<float> weightedPrediction(output.size(), 0.0f);
    float totalWeight = 0.0f;
    
    for (size_t i = 0; i < episodes.size(); ++i) {
        const auto* episode = episodes[i];
        if (episode && !episode->sensoryState.empty()) {
            float weight = 1.0f / (episode->age + 1); // More recent episodes have higher weight
            totalWeight += weight;
            
            for (size_t j = 0; j < output.size() && j < episode->sensoryState.size(); ++j) {
                weightedPrediction[j] += episode->sensoryState[j] * weight;
            }
        }
    }
    
    if (totalWeight > 0.0f) {
        for (size_t j = 0; j < output.size(); ++j) {
            weightedPrediction[j] /= totalWeight;
        }
        
        // Store in output
        for (size_t j = 0; j < output.size() && j < weightedPrediction.size(); ++j) {
            output[j] = weightedPrediction[j];
        }
    }
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 1.0f; // Maximum error when prediction is invalid
    }
    
    // Compute error using cosine similarity
    float dotProduct = 0.0f;
    float normPredicted = 0.0f;
    float normActual = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        dotProduct += predicted[i] * actual[i];
        normPredicted += predicted[i] * predicted[i];
        normActual += actual[i] * actual[i];
    }
    
    if (normPredicted < 0.0001f || normActual < 0.0001f) {
        return 1.0f;
    }
    
    float similarity = dotProduct / (std::sqrt(normPredicted) * std::sqrt(normActual));
    return 1.0f - similarity; // Convert to error (0 = perfect prediction, 1 = no prediction)
}

float PredictionSystem::computePredictionConfidence(const std::vector<float>& neuralActivity) const {
    if (neuralActivity.empty()) return 0.5f;
    
    // Compute confidence based on neural activity variance
    float mean = std::accumulate(neuralActivity.begin(), neuralActivity.end(), 0.0f) / neuralActivity.size();
    float variance = 0.0f;
    for (float value : neuralActivity) {
        variance += (value - mean) * (value - mean);
    }
    variance /= neuralActivity.size();
    
    // Higher variance = lower confidence (uncertainty)
    float confidence = 1.0f - std::min(1.0f, std::sqrt(variance) * 2.0f);
    
    return std::clamp(confidence, 0.0f, 1.0f);
}

float PredictionSystem::computePredictionNovelty(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 0.0f;
    }
    
    // Compute novelty as the normalized difference from average prediction
    // This is simplified - real implementation would use statistical novelty detection
    float maxDiff = 0.0f;
    float maxPossible = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        maxDiff = std::max(maxDiff, diff);
        maxPossible = std::max(maxPossible, std::max(predicted[i], actual[i]));
    }
    
    if (maxPossible < 0.0001f) return 0.0f;
    
    return maxDiff / maxPossible;
}

float PredictionSystem::computePredictionRelevance(const std::vector<float>& predicted, const std::vector<float>& actual) const {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 0.0f;
    }
    
    // Compute relevance based on correlation with brain state
    // Simplified implementation
    float relevance = 0.0f;
    float magnitude = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        magnitude += std::abs(predicted[i]);
    }
    
    if (magnitude < 0.0001f) return 0.0f;
    
    // Relevance is inversely proportional to prediction error
    float error = computePredictionError(predicted, actual);
    relevance = 1.0f - error;
    
    return relevance;
}

void PredictionSystem::updatePredictionWeights(const std::vector<float>& predicted, const std::vector<float>& actual, float error) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return;
    }
    
    // Update weights using reward-modulated plasticity
    float reward = 1.0f - error; // Higher reward for lower error
    
    // Update prediction weights using delta rule
    for (size_t i = 0; i < predicted.size() && i < pImpl->predictionWeights.size(); ++i) {
        float delta = (actual[i] - predicted[i]) * reward * pImpl->learningRate;
        pImpl->predictionWeights[i] += delta;
        
        // Clip weights to prevent explosion
        pImpl->predictionWeights[i] = std::clamp(pImpl->predictionWeights[i], -1.0f, 1.0f);
    }
    
    // Update prediction biases
    float biasDelta = reward * pImpl->learningRate * 0.1f;
    for (size_t i = 0; i < pImpl->predictionBiases.size(); ++i) {
        pImpl->predictionBiases[i] += biasDelta;
        pImpl->predictionBiases[i] = std::clamp(pImpl->predictionBiases[i], -1.0f, 1.0f);
    }
}

void PredictionSystem::optimizePredictions() {
    if (pImpl->predictionWeights.empty()) return;
    
    // Regularize prediction weights to prevent overfitting
    float norm = 0.0f;
    for (float weight : pImpl->predictionWeights) {
        norm += weight * weight;
    }
    norm = std::sqrt(norm);
    
    if (norm > 0.0f) {
        // Normalize weights
        for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
            pImpl->predictionWeights[i] /= norm;
        }
    }
    
    // Adjust biases based on prediction statistics
    if (pImpl->totalPredictions > 0) {
        float successRate = static_cast<float>(pImpl->successfulPredictions) / pImpl->totalPredictions;
        pImpl->predictionStrength = std::min(1.0f, 0.5f + successRate * 0.5f);
        pImpl->predictionStability = std::min(1.0f, pImpl->predictionStability + 0.01f * successRate);
    }
}

void PredictionSystem::regularizePredictions() {
    // Apply L2 regularization to prevent overfitting
    float regularizationFactor = 0.01f;
    
    for (size_t i = 0; i < pImpl->predictionWeights.size(); ++i) {
        pImpl->predictionWeights[i] *= (1.0f - regularizationFactor);
    }
    
    // Reduce biases
    for (size_t i = 0; i < pImpl->predictionBiases.size(); ++i) {
        pImpl->predictionBiases[i] *= (1.0f - regularizationFactor);
    }
}

void PredictionSystem::compressPredictions() {
    // Compress prediction model by reducing effective dimensionality
    if (pImpl->predictionWeights.size() < 2) return;
    
    // Apply principal component analysis (simplified)
    size_t newSize = std::max(size_t(10), pImpl->predictionWeights.size() / 2);
    
    std::vector<float> compressedWeights(newSize, 0.0f);
    for (size_t i = 0; i < newSize && i < pImpl->predictionWeights.size(); ++i) {
        compressedWeights[i] = pImpl->predictionWeights[i] * (1.0f + static_cast<float>(i) / static_cast<float>(newSize));
    }
    
    pImpl->predictionWeights = compressedWeights;
    pImpl->predictionBiases.resize(newSize, 0.0f);
}

void PredictionSystem::expandPredictions() {
    // Expand prediction model by increasing dimensionality
    if (pImpl->predictionWeights.size() < 10) return;
    
    // Add noise to weights to create diversity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution(0.0f, 0.1f);
    
    // Add new weights
    size_t originalSize = pImpl->predictionWeights.size();
    for (size_t i = 0; i < originalSize / 2; ++i) {
        float noise = distribution(gen);
        pImpl->predictionWeights.push_back(noise * 0.5f);
        pImpl->predictionBiases.push_back(noise * 0.1f);
    }
}

void PredictionSystem::setPredictionModel(const std::vector<float>& weights, const std::vector<float>& biases) {
    pImpl->predictionWeights = weights;
    pImpl->predictionBiases = biases;
    
    // Reset prediction statistics
    pImpl->predictionStrength = 0.5f;
    pImpl->predictionStability = 0.5f;
}

const std::vector<float>& PredictionSystem::getPredictionModel() const {
    static const std::vector<float> emptyWeights;
    return pImpl->predictionWeights.empty() ? emptyWeights : pImpl->predictionWeights;
}

void PredictionSystem::clearPredictionModel() {
    pImpl->predictionWeights.clear();
    pImpl->predictionBiases.clear();
    
    // Reset prediction statistics
    pImpl->predictionError = 0.0f;
    pImpl->confidence = 0.5f;
    pImpl->novelty = 0.0f;
    pImpl->relevance = 0.0f;
    pImpl->predictionStrength = 0.5f;
    pImpl->predictionStability = 0.5f;
}

const std::vector<float>& PredictionSystem::getPredictionHistory() const {
    return pImpl->errorHistory;
}

const std::vector<float>& PredictionSystem::getPredictionStatistics() const {
    // Return statistics as a vector
    static std::vector<float> stats;
    
    stats.clear();
    if (pImpl->totalPredictions > 0) {
        float accuracy = static_cast<float>(pImpl->successfulPredictions) / pImpl->totalPredictions;
        stats = {
            pImpl->predictionError,
            pImpl->confidence,
            pImpl->novelty,
            pImpl->relevance,
            pImpl->predictionStrength,
            pImpl->predictionStability,
            accuracy,
            static_cast<float>(pImpl->totalPredictions),
            static_cast<float>(pImpl->successfulPredictions),
            static_cast<float>(pImpl->totalTrainingSteps)
        };
    }
    
    return stats;
}

// Helper methods

std::vector<float> PredictionSystem::generateNeuralPrediction(const std::vector<float>& neuralActivity) {
    // Generate prediction from neural activity using neural substrate
    std::vector<float> prediction(neuralActivity.size(), 0.0f);
    
    // Use prediction weights to transform neural activity into prediction
    for (size_t i = 0; i < neuralActivity.size() && i < pImpl->predictionWeights.size(); ++i) {
        prediction[i] = neuralActivity[i] * pImpl->predictionWeights[i];
        
        // Add bias
        if (i < pImpl->predictionBiases.size()) {
            prediction[i] += pImpl->predictionBiases[i];
        }
        
        // Apply activation function
        prediction[i] = std::tanh(prediction[i]);
    }
    
    return prediction;
}

void PredictionSystem::trainPredictionWeights(const std::vector<float>& observation, const std::vector<float>& prediction) {
    if (observation.empty() || prediction.empty() || observation.size() != prediction.size()) {
        return;
    }
    
    // Ensure we have weights for this dimensionality
    if (pImpl->predictionWeights.size() != observation.size()) {
        pImpl->predictionWeights = std::vector<float>(observation.size(), 0.0f);
        pImpl->predictionBiases = std::vector<float>(observation.size(), 0.0f);
    }
    
    // Update weights using simple Hebbian learning
    for (size_t i = 0; i < observation.size() && i < prediction.size(); ++i) {
        float weightUpdate = observation[i] * prediction[i] * pImpl->learningRate;
        pImpl->predictionWeights[i] += weightUpdate;
        
        // Add bias update
        pImpl->predictionBiases[i] += prediction[i] * pImpl->learningRate * 0.1f;
    }
}

void PredictionSystem::updatePredictionFromPattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return;
    
    // Ensure we have weights for this pattern size
    if (pImpl->predictionWeights.size() != pattern.size()) {
        pImpl->predictionWeights = pattern;
        pImpl->predictionBiases = std::vector<float>(pattern.size(), 0.0f);
    }
    
    // Blend with existing weights
    for (size_t i = 0; i < pattern.size() && i < pImpl->predictionWeights.size(); ++i) {
        pImpl->predictionWeights[i] = (pImpl->predictionWeights[i] + pattern[i]) * 0.5f;
    }
}

} // namespace nlm

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
    // TODO PHASE 2: Train prediction model
}

} // namespace nlm

#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "NeuralPrediction.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

// Enhanced PredictionSystem implementation with neural-based predictive coding
// Integrates NeuralPrediction and ActionConsequencePredictor for comprehensive prediction

// Enhanced PredictionSystem::Impl with neural prediction capabilities
struct PredictionSystem::Impl {
    // Core prediction engines
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    std::unique_ptr<ActionConsequencePredictor> actionConsequencePredictor;
    
    // Enhanced prediction error signal
    float predictionError;
    float confidence;
    float noveltyPrediction;
    
    // Prediction history and learning
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::deque<std::vector<float>> predictionBuffer;
    std::deque<float> predictionErrorsBuffer;
    
    // Memory-based prediction components
    Brain* brain;
    float episodicPredictionWeight;
    float workingMemoryPredictionWeight;
    float noveltyDrivenPredictionWeight;
    
    // Temporal prediction state
    std::vector<float> lastSensoryState;
    std::vector<float> predictedState;
    std::vector<float> predictionComponents;
    
    // Prediction-driven action selection
    std::vector<std::pair<ActionType, std::vector<float>>> predictedActions;
    std::vector<float> actionPredictionScores;
    
    // Prediction learning state
    float predictionLearningRate;
    float predictionAdaptationRate;
    size_t predictionSequenceMemory;
    
    Impl() : brain(nullptr),
             predictionError(0.0f),
             confidence(0.5f),
             noveltyPrediction(0.0f),
             episodicPredictionWeight(0.4f),
             workingMemoryPredictionWeight(0.3f),
             noveltyDrivenPredictionWeight(0.3f),
             predictionLearningRate(0.1f),
             predictionAdaptationRate(0.05f),
             predictionSequenceMemory(20) {}
};

PredictionSystem::PredictionSystem() : pImpl(new Impl) {}

PredictionSystem::~PredictionSystem() = default;

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!pImpl->brain) {
        return currentState.clone();
    }
    
    // Convert current state to vector for processing
    const auto& currentData = currentState.getData();
    pImpl->lastSensoryState = currentData;
    
    // Generate multi-level prediction using neural prediction system
    std::vector<float> predictedPattern = generateMultiLevelPrediction(currentData);
    
    // Apply memory-based refinement
    std::vector<float> memoryRefinedPrediction = applyMemoryRefinement(predictedPattern, currentData);
    
    // Store in prediction buffer for learning
    pImpl->predictionBuffer.push_back(memoryRefinedPrediction);
    if (pImpl->predictionBuffer.size() > 100) {
        pImpl->predictionBuffer.pop_front();
    }
    
    // Convert back to SensoryInput
    auto result = std::make_unique<SensoryInput>();
    result->setData(memoryRefinedPrediction);
    
    // Update prediction error based on discrepancy
    pImpl->predictionError = computePredictionError(currentData, memoryRefinedPrediction);
    
    // Update prediction confidence based on consistency
    pImpl->confidence = updatePredictionConfidence(pImpl->predictionError);
    
    return result;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    const auto& predictedData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Compute prediction error using enhanced error signal
    float error = computeEnhancedPredictionError(predictedData, actualData);
    
    // Store error history
    pImpl->predictionError = error;
    pImpl->errorHistory.push_back(error);
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Update prediction confidence based on error
    pImpl->confidence = updatePredictionConfidence(error);
    
    // Learn from prediction error
    learnFromPredictionError(predictedData, actualData, error);
    
    // Update neural prediction system if available
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->updateWithObservation(actualData, pImpl->brain->getCurrentStep());
    }
    
    // Update action consequence predictor
    if (pImpl->actionConsequencePredictor) {
        // Get the action that was taken (simplified - in reality this would come from environment)
        ActionType lastAction = ActionType::Wait;  // TODO: Get from actual action
        pImpl->actionConsequencePredictor->updatePrediction(lastAction, predictedData, actualData, 1.0f - error);
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
    pImpl->confidenceHistory.clear();
    pImpl->predictionBuffer.clear();
    pImpl->predictionErrorsBuffer.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
    if (pImpl->actionConsequencePredictor) {
        pImpl->actionConsequencePredictor->clear();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    const auto& data = observation.getData();
    
    // Train neural prediction system
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->recordSensoryState(data, pImpl->brain->getCurrentStep());
    }
    
    // Update action consequence predictor with new experience
    if (pImpl->actionConsequencePredictor) {
        // This would need the actual action taken - for now just record state
        // In practice, this would come from the environment's action
        pImpl->actionConsequencePredictor->recordExperience(
            ActionType::Wait, // TODO: Get from environment
            data,
            data, // TODO: Get actual resulting state
            0.0f  // TODO: Get actual reward
        );
    }
}

// Helper methods for enhanced prediction
std::vector<float> PredictionSystem::generateMultiLevelPrediction(const std::vector<float>& currentState) {
    std::vector<float> prediction = currentState;
    
    // Get prediction from neural prediction system
    if (pImpl->neuralPrediction) {
        prediction = pImpl->neuralPrediction->generatePrediction(pImpl->brain->getCurrentStep());
        if (prediction.empty()) {
            // Fallback to simple prediction
            prediction = currentState;
        }
    }
    
    // Get prediction from action consequence predictor
    if (pImpl->actionConsequencePredictor) {
        for (const auto& actionPair : pImpl->predictedActions) {
            std::vector<float> actionResult = pImpl->actionConsequencePredictor->predictConsequence(
                actionPair.first, currentState);
            if (!actionResult.empty()) {
                // Blend action-based prediction with current prediction
                for (size_t i = 0; i < std::min(prediction.size(), actionResult.size()); ++i) {
                    prediction[i] = (prediction[i] + actionResult[i]) * 0.5f;
                }
            }
        }
    }
    
    return prediction;
}

std::vector<float> PredictionSystem::applyMemoryRefinement(const std::vector<float>& predicted, 
                                                           const std::vector<float>& current) {
    std::vector<float> refined = predicted;
    
    // Get episodic memory for context-based refinement
    if (pImpl->brain->getEpisodicMemory()) {
        auto episodes = pImpl->brain->getEpisodicMemory()->getRecentEpisodes(5);
        if (!episodes.empty()) {
            // Use episodic memory to refine prediction
            float totalWeight = 0.0f;
            std::vector<float> memorySum(refined.size(), 0.0f);
            
            for (const auto* episode : episodes) {
                if (!episode->sensoryState.empty() && episode->reward > 0) {
                    float similarity = computeSimilarity(current, episode->sensoryState);
                    if (similarity > 0.5f) {
                        for (size_t i = 0; i < refined.size() && i < episode->sensoryState.size(); ++i) {
                            memorySum[i] += episode->sensoryState[i] * similarity;
                        }
                        totalWeight += similarity;
                    }
                }
            }
            
            if (totalWeight > 0.0f) {
                for (size_t i = 0; i < refined.size(); ++i) {
                    refined[i] = refined[i] * (1.0f - pImpl->episodicPredictionWeight) + 
                                memorySum[i] / totalWeight * pImpl->episodicPredictionWeight;
                }
            }
        }
    }
    
    // Get working memory for short-term context refinement
    if (pImpl->brain->getWorkingMemory()) {
        auto memoryPattern = pImpl->brain->getWorkingMemory()->retrieve();
        if (!memoryPattern.empty() && memoryPattern.size() == refined.size()) {
            for (size_t i = 0; i < refined.size(); ++i) {
                refined[i] = refined[i] * (1.0f - pImpl->workingMemoryPredictionWeight) + 
                            memoryPattern[i] * pImpl->workingMemoryPredictionWeight;
            }
        }
    }
    
    // Apply novelty-driven adjustment
    if (pImpl->brain->getNovelty()) {
        float novelty = pImpl->brain->getNovelty()->getLevel();
        if (novelty > 0.5f) {
            // Increase prediction variability for novel situations
            for (size_t i = 0; i < refined.size(); ++i) {
                refined[i] += (pImpl->rng->nextFloat() - 0.5f) * 0.2f * novelty;
            }
        }
    }
    
    // Normalize refined prediction
    float norm = computeVectorNorm(refined);
    if (norm > 0.0f) {
        for (size_t i = 0; i < refined.size(); ++i) {
            refined[i] /= norm;
        }
    }
    
    return refined;
}

float PredictionSystem::computeEnhancedPredictionError(const std::vector<float>& predicted, 
                                                     const std::vector<float>& actual) {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    // Compute overall error using cosine similarity
    float similarity = computeSimilarity(predicted, actual);
    float error = 1.0f - similarity;
    
    // Add temporal error component (if we have temporal context)
    if (!pImpl->lastSensoryState.empty()) {
        float temporalError = computeTemporalError(pImpl->lastSensoryState, predicted);
        error = error * 0.7f + temporalError * 0.3f;
    }
    
    // Add novelty-based error component
    if (pImpl->brain->getNovelty()) {
        float novelty = pImpl->brain->getNovelty()->getLevel();
        if (novelty > 0.0f) {
            // Novel situations should have higher prediction error initially
            error = error * (1.0f - novelty * 0.3f);
        }
    }
    
    // Clamp error to [0, 1]
    return std::max(0.0f, std::min(1.0f, error));
}

float PredictionSystem::updatePredictionConfidence(float error) {
    float baseConfidence = 0.5f;
    
    // Reduce confidence based on prediction error
    float errorBased = 1.0f - error;
    
    // Get confidence boost from memory systems
    float memoryBoost = 1.0f;
    
    // Episodic memory confidence
    if (pImpl->brain->getEpisodicMemory()) {
        size_t episodeCount = pImpl->brain->getEpisodicMemory()->getEpisodeCount();
        memoryBoost *= std::min(1.0f, 1.0f + episodeCount * 0.01f);
    }
    
    // Working memory activity confidence
    if (pImpl->brain->getWorkingMemory()) {
        float memoryActivity = pImpl->brain->getWorkingMemory()->getMemoryActivity();
        memoryBoost *= (1.0f + memoryActivity * 0.5f);
    }
    
    // Dopamine modulation of confidence
    if (pImpl->brain->getDopamine()) {
        float dopamine = pImpl->brain->getDopamine()->getLevel();
        memoryBoost *= (1.0f + dopamine * 0.3f);
    }
    
    // Adapt prediction based on recent error history
    if (!pImpl->errorHistory.empty()) {
        float avgRecentError = computeMean(pImpl->errorHistory);
        float adaptationFactor = 1.0f - std::abs(avgRecentError - error) * 2.0f;
        adaptationFactor = std::max(0.5f, adaptationFactor);
        memoryBoost *= adaptationFactor;
    }
    
    // Combine confidence components
    float confidence = baseConfidence * errorBased * memoryBoost;
    
    // Apply learning rate adaptation
    confidence = confidence + (baseConfidence - confidence) * pImpl->predictionLearningRate;
    
    // Clamp confidence to [0.1, 0.95]
    return std::max(0.1f, std::min(0.95f, confidence));
}

void PredictionSystem::learnFromPredictionError(const std::vector<float>& predicted,
                                               const std::vector<float>& actual,
                                               float error) {
    // Store error in buffer for learning
    pImpl->predictionErrorsBuffer.push_back(error);
    if (pImpl->predictionErrorsBuffer.size() > 100) {
        pImpl->predictionErrorsBuffer.pop_front();
    }
    
    // Adapt prediction learning based on error
    if (error > 0.5f) {
        // High error -> increase learning rate
        pImpl->predictionLearningRate = std::min(0.2f, pImpl->predictionLearningRate * 1.1f);
    } else if (error < 0.1f) {
        // Low error -> decrease learning rate (stabilize)
        pImpl->predictionLearningRate = std::max(0.01f, pImpl->predictionLearningRate * 0.95f);
    }
    
    // Modulate neural prediction system based on error
    if (pImpl->neuralPrediction && error > 0.3f) {
        // High prediction error should trigger learning in neural prediction system
        // This happens through neuromodulatory signals (dopamine)
        if (pImpl->brain->getDopamine()) {
            float dopamine = pImpl->brain->getDopamine()->getLevel();
            if (dopamine > 0.5f) {
                // Strong learning signal when prediction error is high and dopamine is high
                pImpl->neuralPrediction->modulatePredictionSynapses(error, 1.0f - error);
            }
        }
    }
    
    // Update episodic memory with prediction error as learning signal
    if (pImpl->brain->getEpisodicMemory()) {
        // Create an episodic memory item representing the prediction error learning event
        EpisodicMemoryItem learningEpisode;
        learningEpisode.timestamp = pImpl->brain->getCurrentStep();
        learningEpisode.reward = 1.0f - error;  // Invert error to get a reward-like signal
        learningEpisode.novelty = error;  // Prediction error as novelty
        learningEpisode.sensoryState = predicted;  // The predicted state that was wrong
        learningEpisode.resultingSensoryState = actual;  // The actual state
        
        pImpl->brain->getEpisodicMemory()->storeEpisode(learningEpisode);
    }
}

float PredictionSystem::computePredictionError(const std::vector<float>& predicted, 
                                            const std::vector<float>& actual) const {
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    // Compute cosine similarity as error measure
    float similarity = computeSimilarity(predicted, actual);
    return 1.0f - similarity;
}

float PredictionSystem::computeSimilarity(const std::vector<float>& a, 
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

float PredictionSystem::computeTemporalError(const std::vector<float>& prevState,
                                          const std::vector<float>& currentState) const {
    if (prevState.empty() || currentState.empty()) {
        return 0.0f;
    }
    
    // Simple temporal coherence measure
    float totalDiff = 0.0f;
    for (size_t i = 0; i < std::min(prevState.size(), currentState.size()); ++i) {
        totalDiff += std::abs(currentState[i] - prevState[i]);
    }
    
    return std::min(1.0f, totalDiff / prevState.size());
}

float PredictionSystem::computeVectorNorm(const std::vector<float>& vec) const {
    float sum = 0.0f;
    for (float v : vec) {
        sum += v * v;
    }
    return std::sqrt(sum);
}

float PredictionSystem::computeMean(const std::vector<float>& values) const {
    if (values.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (float v : values) {
        sum += v;
    }
    return sum / values.size();
}

} // namespace nlm

#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <memory>

namespace nlm {

PredictionSystem::PredictionSystem()
    : neuralImpl_(new NeuralPredictionImpl)
    , actionImpl_(new ActionConsequencePredictorImpl)
    , errorImpl_(new PredictionErrorSignalImpl)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , confidence_(0.5f)
    , errorHistory_()
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalSequenceCount_(0)
    , lastError_(0.0f)
    , surpriseThreshold_(0.3f)
{
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    neuralImpl_->brain = brain;
    actionImpl_->brain = brain;
    
    NLM_LOG_INFO("Prediction system initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    // Use neural prediction if available
    if (temporalPredictionEnabled_ && brain_) {
        auto predictedData = generatePrediction(0);
        if (!predictedData.empty()) {
            // Create a new SensoryInput with the predicted data
            // For now, return a clone of current state as placeholder
            return currentState.clone();
        }
    }
    
    // Simple prediction as placeholder
    return currentState.clone();
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute prediction error using neural system if available
    if (temporalPredictionEnabled_) {
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            predictionError_ = sumError / predData.size();
            errorHistory_.push_back(predictionError_);
        }
    } else {
        // Simple error calculation
        const auto& predData = predicted.getData();
        const auto& actualData = actual.getData();
        
        if (predData.size() == actualData.size() && !predData.empty()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            predictionError_ = sumError / predData.size();
            errorHistory_.push_back(predictionError_);
        }
    }
}

float PredictionSystem::getPredictionError() const {
    return predictionError_;
}

float PredictionSystem::getConfidence() const {
    if (errorHistory_.empty()) return 0.5f;
    float avgError = 0.0f;
    for (float error : errorHistory_) {
        avgError += error;
    }
    avgError /= errorHistory_.size();
    return std::max(0.0f, 1.0f - avgError);
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return errorHistory_;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    if (neuralImpl_) {
        neuralImpl_->learnedSequences.clear();
        neuralImpl_->sequenceAssociations.clear();
        neuralImpl_->patternRepresentations.clear();
        neuralImpl_->stateHistory.clear();
    }
    if (actionImpl_) {
        actionImpl_->experiences.clear();
        actionImpl_->actionQuality.clear();
    }
    
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    errorHistory_.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train both neural and simple prediction systems
    if (temporalPredictionEnabled_ && brain_) {
        // Convert SensoryInput to vector for neural prediction
        const auto& obsData = observation.getData();
        recordSensoryState(obsData, 0);
    }
}

void PredictionSystem::recordSensoryState(const std::vector<float>& sensoryState, SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    temporalSequenceCount_++;
    
    // Keep only recent states
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.erase(recentSensoryStates_.begin());
        stateTimestamps_.erase(stateTimestamps_.begin());
    }
    
    // Learn sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
}

std::vector<float> PredictionSystem::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    const auto& currentState = recentSensoryStates_.back();
    
    // Find most similar learned sequence
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < neuralImpl_->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentState, neuralImpl_->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = neuralImpl_->patternNeurons[i];
        }
    }
    
    // If we have a good match, predict what comes next
    if (bestSimilarity > 0.7f && bestMatch != INVALID_NEURON_ID) {
        for (const auto& assoc : neuralImpl_->sequenceAssociations) {
            if (assoc.first == bestMatch) {
                for (size_t i = 0; i < neuralImpl_->patternNeurons.size(); ++i) {
                    if (neuralImpl_->patternNeurons[i] == assoc.second) {
                        confidence_ = bestSimilarity;
                        return neuralImpl_->patternRepresentations[i];
                    }
                }
            }
        }
    }
    
    // No good prediction - return current state as baseline
    confidence_ = 0.0f;
    return currentState;
}

float PredictionSystem::updateWithObservation(const std::vector<float>& actualState,
                                             SimulationStep currentStep) {
    auto predicted = generatePrediction(currentStep);
    
    if (predicted.empty()) {
        predictionError_ = 0.0f;
    } else {
        predictionError_ = computeSimilarity(predicted, actualState);
        predictionError_ = 1.0f - predictionError_;
    }
    
    errorHistory_.push_back(predictionError_);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    recordSensoryState(actualState, currentStep);
    
    float reward = 1.0f - predictionError_;
    modulatePredictionSynapses(predictionError_, reward);
    
    return predictionError_;
}

std::vector<float> PredictionSystem::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;
    }
    
    float bestMatch = 0.0f;
    std::vector<float> bestConsequence;
    
    for (size_t i = 0; i < recentActions_.size(); ++i) {
        if (recentActions_[i].first == action) {
            float sim = computeSimilarity(currentState, recentActions_[i].second);
            if (sim > bestMatch) {
                bestMatch = sim;
                if (i < actionImpl_->experiences.size()) {
                    bestConsequence = actionImpl_->experiences[i].second;
                }
            }
        }
    }
    
    if (bestMatch > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;
}

std::vector<std::vector<float>> PredictionSystem::predictMultipleSteps(SimulationStep currentStep,
                                                                        size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    auto currentPred = recentSensoryStates_.empty() ? 
                       std::vector<float>() : recentSensoryStates_.back();
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (currentPred.empty()) break;
        
        predictions.push_back(currentPred);
        
        auto nextPred = generatePrediction(currentStep + step);
        if (nextPred.empty()) break;
        currentPred = nextPred;
    }
    
    return predictions;
}

void PredictionSystem::recordAction(ActionType action, SimulationStep step) {
    if (recentSensoryStates_.empty()) return;
    
    recentActions_.push_back({action, recentSensoryStates_.back()});
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
    }
}

void PredictionSystem::recordExperience(ActionType action,
                                       const std::vector<float>& beforeState,
                                       const std::vector<float>& afterState,
                                       float reward) {
    actionImpl_->experiences.push_back({{beforeState, action}, afterState});
    actionImpl_->actionQuality.push_back({action, reward});
    
    float totalReward = 0.0f;
    size_t count = 0;
    for (const auto& aq : actionImpl_->actionQuality) {
        if (aq.first == action) {
            totalReward += aq.second;
            ++count;
        }
    }
    
    for (auto& aq : actionImpl_->actionQuality) {
        if (aq.first == action) {
            aq.second = totalReward / count;
            break;
        }
    }
    
    if (actionImpl_->experiences.size() > 1000) {
        actionImpl_->experiences.erase(actionImpl_->experiences.begin());
    }
}

std::vector<float> PredictionSystem::predictConsequence(ActionType action,
                                                         const std::vector<float>& currentState) {
    float bestSimilarity = 0.0f;
    std::vector<float> bestConsequence;
    
    for (const auto& exp : actionImpl_->experiences) {
        if (exp.first.second == action) {
            const auto& before = exp.first.first;
            const auto& after = exp.second;
            
            if (before.size() != currentState.size()) continue;
            
            float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
            for (size_t i = 0; i < before.size(); ++i) {
                dot += before[i] * currentState[i];
                norm1 += before[i] * before[i];
                norm2 += currentState[i] * currentState[i];
            }
            
            float sim = (norm1 > 0.0001f && norm2 > 0.0001f) ? 
                        dot / (std::sqrt(norm1) * std::sqrt(norm2)) : 0.0f;
            
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestConsequence = after;
            }
        }
    }
    
    if (bestSimilarity > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;
}

float PredictionSystem::computeError(const std::vector<float>& predicted,
                                   const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return 0.0f;
    }
    
    float totalDiff = 0.0f;
    float intensityDiff = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalDiff += diff;
        intensityDiff += diff * diff;
    }
    
    float meanError = totalDiff / predicted.size();
    float meanSqError = intensityDiff / predicted.size();
    
    float spatialVar = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    float spatialError = std::sqrt(spatialVar / predicted.size());
    
    errorImpl_->lastIntensityError = std::sqrt(meanSqError);
    errorImpl_->lastSpatialError = spatialError;
    errorComponents_.intensityError = errorImpl_->lastIntensityError;
    errorComponents_.spatialError = errorImpl_->lastSpatialError;
    
    lastError_ = std::sqrt(meanSqError + spatialError * spatialError);
    errorComponents_.totalError = lastError_;
    
    return lastError_;
}

float PredictionSystem::getModulationSignal() const {
    return std::min(1.0f, lastError_ * 2.0f);
}

void PredictionSystem::recordError(float error, SimulationStep step) {
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

float PredictionSystem::getAverageActionQuality(ActionType action) const {
    if (actionImpl_->actionQuality.empty()) return 0.0f;
    
    float total = 0.0f;
    size_t count = 0;
    for (const auto& aq : actionImpl_->actionQuality) {
        if (aq.first == action) {
            total += aq.second;
            ++count;
        }
    }
    
    return count > 0 ? total / count : 0.0f;
}

void PredictionSystem::learnTemporalSequence(const std::vector<float>& currentState,
                                           const std::vector<float>& nextState,
                                           SimulationStep currentStep) {
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron != nextNeuron) {
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
    }
}

NeuronId PredictionSystem::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                   float similarityThreshold) {
    for (size_t i = 0; i < neuralImpl_->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(pattern, neuralImpl_->patternRepresentations[i]);
        if (sim >= similarityThreshold) {
            return neuralImpl_->patternNeurons[i];
        }
    }
    
    if (brain_) {
        NeuronId newId = NeuronId(neuralImpl_->patternNeurons.size() + 10000);
        neuralImpl_->patternNeurons.push_back(newId);
        neuralImpl_->patternRepresentations.push_back(pattern);
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

void PredictionSystem::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    if (from == INVALID_NEURON_ID || to == INVALID_NEURON_ID) return;
    
    for (auto& assoc : neuralImpl_->sequenceAssociations) {
        if (assoc.first == from && assoc.second == to) {
            return;
        }
    }
    
    neuralImpl_->sequenceAssociations.emplace_back(from, to);
}

float PredictionSystem::computeSimilarity(const std::vector<float>& a,
                                       const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
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

void PredictionSystem::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    predictionNeurons_ = neuralImpl_->patternNeurons;
    return predictionNeurons_;
}

std::vector<NeuronId> PredictionSystem::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : neuralImpl_->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    if (neuralImpl_) {
        neuralImpl_->learnedSequences.clear();
        neuralImpl_->sequenceAssociations.clear();
        neuralImpl_->patternRepresentations.clear();
        neuralImpl_->stateHistory.clear();
    }
    if (actionImpl_) {
        actionImpl_->experiences.clear();
        actionImpl_->actionQuality.clear();
    }
    
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    errorHistory_.clear();
    predictionError_ = 0.0f;
    confidence_ = 0.5f;
    lastError_ = 0.0f;
}

} // namespace nlm

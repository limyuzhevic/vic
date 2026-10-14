#include "PredictionSystem.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Multi-scale prediction buffers
    std::map<size_t, std::vector<std::unique_ptr<SensoryInput>>> multiScalePredictions;
    std::vector<float> predictionErrors;
    std::vector<float> predictionConfidences;
    
    // Integration state
    float accumulatedPredictionError;
    float totalPredictionWeight;
    
    // Cross-system integration flags
    bool attentionUpdated;
    bool workingMemoryUpdated;
    bool actionPlanningUpdated;
    
    Impl() : brain(nullptr), accumulatedPredictionError(0.0f), totalPredictionWeight(0.0f),
             attentionUpdated(false), workingMemoryUpdated(false), actionPlanningUpdated(false) {}
};

PredictionSystem::PredictionSystem()
    : pImpl(new Impl)
    , predictionHorizon_(1)
    , sequenceMemorySize_(10)
    , temporalPredictionEnabled_(true)
    , actionConsequencePredictionEnabled_(true)
    , multiScalePredictionEnabled_(true)
    , temporalPredictionGain_(1.0f)
    , actionPredictionGain_(0.5f)
    , brain_(nullptr)
    , neuralPrediction_(nullptr)
{
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain) {
    brain_ = brain;
    
    // Create and initialize the neural prediction component
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    neuralPrediction_->initialize(brain_);
    
    // Configure neural prediction
    neuralPrediction_->setSequenceMemorySize(sequenceMemorySize_);
    neuralPrediction_->setPredictionHorizon(predictionHorizon_);
    neuralPrediction_->enableTemporalPrediction(temporalPredictionEnabled_);
    neuralPrediction_->enableActionConsequencePrediction(actionConsequencePredictionEnabled_);
    
    NLM_LOG_INFO("Enhanced PredictionSystem initialized");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!temporalPredictionEnabled_ || !brain_ || !neuralPrediction_) {
        return currentState.clone();
    }
    
    // Generate prediction using neural prediction system
    auto predictionData = neuralPrediction_->generatePrediction(brain_->getCurrentStep());
    
    if (predictionData.empty()) {
        return currentState.clone();
    }
    
    // Create a new SensoryInput with predicted data
    auto predictedInput = std::make_unique<SensoryInput>();
    predictedInput->setData(predictionData);
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!brain_ || !neuralPrediction_) {
        return;
    }
    
    // Update neural prediction with actual observation
    auto predictedData = predicted.getData();
    auto actualData = actual.getData();
    
    // Compute prediction error
    float error = 0.0f;
    if (!predictedData.empty() && !actualData.empty() && predictedData.size() == actualData.size()) {
        float sumError = 0.0f;
        for (size_t i = 0; i < predictedData.size(); ++i) {
            float diff = predictedData[i] - actualData[i];
            sumError += diff * diff;
        }
        error = sumError / predictedData.size();
    }
    
    // Update with actual observation
    float neuralError = neuralPrediction_->updateWithObservation(actualData, brain_->getCurrentStep());
    
    // Combine errors
    float combinedError = (error + neuralError) / 2.0f;
    
    // Store error metrics
    pImpl->predictionErrors.push_back(combinedError);
    pImpl->accumulatedPredictionError += combinedError;
    pImpl->totalPredictionWeight += 1.0f;
    
    // Keep history limited
    if (pImpl->predictionErrors.size() > 1000) {
        pImpl->predictionErrors.erase(pImpl->predictionErrors.begin());
    }
    
    // Update confidence based on consistency of predictions
    float confidence = 1.0f - std::min(1.0f, combinedError * 2.0f);  // Invert error to confidence
    pImpl->predictionConfidences.push_back(confidence);
    if (pImpl->predictionConfidences.size() > 1000) {
        pImpl->predictionConfidences.erase(pImpl->predictionConfidences.begin());
    }
    
    // Apply plasticity based on prediction error
    applyPredictionErrorPlasticity(combinedError, actualData);
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->predictionErrors.empty()) {
        return 0.0f;
    }
    return pImpl->predictionErrors.back();
}

float PredictionSystem::getConfidence() const {
    if (pImpl->predictionConfidences.empty()) {
        return 0.5f;
    }
    return pImpl->predictionConfidences.back();
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->predictionErrors;
}

void PredictionSystem::clearHistory() {
    pImpl->predictionErrors.clear();
    pImpl->predictionConfidences.clear();
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!brain_ || !neuralPrediction_) {
        return;
    }
    
    // Train neural prediction system
    neuralPrediction_->recordSensoryState(observation.getData(), brain_->getCurrentStep());
    
    // Generate and record predicted action consequences
    if (actionConsequencePredictionEnabled_) {
        // Get all possible actions from brain planner
        if (brain_->getPlanner()) {
            // Record current state and action
            neuralPrediction_->recordAction(ActionType::Wait, brain_->getCurrentStep());
        }
    }
}

std::vector<std::unique_ptr<SensoryInput>> PredictionSystem::predictMultipleSteps(
    const SensoryInput& currentState, size_t numSteps) {
    std::vector<std::unique_ptr<SensoryInput>> predictions;
    
    if (!multiScalePredictionEnabled_ || !brain_ || !neuralPrediction_) {
        return predictions;
    }
    
    // Generate multi-scale predictions
    auto currentData = currentState.getData();
    neuralPrediction_->recordSensoryState(currentData, brain_->getCurrentStep());
    
    // Use neural prediction's multi-step capability
    auto multiStepPredictions = neuralPrediction_->predictMultipleSteps(
        brain_->getCurrentStep(), numSteps);
    
    for (const auto& predData : multiStepPredictions) {
        auto predInput = std::make_unique<SensoryInput>();
        predInput->setData(predData);
        predictions.push_back(std::move(predInput));
    }
    
    return predictions;
}

PredictionSystem::ErrorComponents PredictionSystem::getErrorComponents() const {
    ErrorComponents components{0.0f, 0.0f, 0.0f, 0.0f};
    
    if (pImpl->predictionErrors.empty()) {
        components.totalError = 0.0f;
        return components;
    }
    
    float totalError = pImpl->predictionErrors.back();
    
    // Split error into components based on neural prediction state
    if (neuralPrediction_) {
        components.sensoryError = totalError * 0.6f;    // 60% sensory prediction error
        components.temporalError = totalError * 0.3f;   // 30% temporal prediction error
        components.actionError = totalError * 0.1f;     // 10% action prediction error
    }
    
    components.totalError = totalError;
    return components;
}

void PredictionSystem::updateAttentionBasedOnPrediction(const SensoryInput& predicted) {
    if (!brain_ || !brain_->getAttention() || pImpl->attentionUpdated) {
        return;
    }
    
    // Use prediction to modulate attention
    auto predictedData = predicted.getData();
    if (!predictedData.empty()) {
        // Find salient features in prediction
        float maxPrediction = *std::max_element(predictedData.begin(), predictedData.end());
        float minPrediction = *std::min_element(predictedData.begin(), predictedData.end());
        
        // Adjust attention strength based on prediction variability
        float predictionVariability = maxPrediction - minPrediction;
        float attentionStrength = 0.5f + predictionVariability * temporalPredictionGain_;
        
        // Update attention system
        brain_->getAttention()->setExcitationStrength(attentionStrength);
        
        // If prediction error is high, focus attention on prediction areas
        if (getPredictionError() > 0.5f) {
            // Enhance attentional focus on prediction-relevant neurons
            // (In a full implementation, this would identify and boost
            // neurons involved in predictive coding)
        }
    }
    
    pImpl->attentionUpdated = true;
}

void PredictionSystem::updateWorkingMemoryFromPredictions(const SensoryInput& predicted) {
    if (!brain_ || !brain_->getWorkingMemory() || pImpl->workingMemoryUpdated) {
        return;
    }
    
    auto predictedData = predicted.getData();
    if (predictedData.empty()) {
        return;
    }
    
    // Store predicted sensory pattern in working memory
    float strength = getConfidence() * temporalPredictionGain_;
    brain_->getWorkingMemory()->storeSensoryPattern(predictedData, strength);
    
    pImpl->workingMemoryUpdated = true;
}

std::vector<ActionType> PredictionSystem::getPredictedActionRecommendations() const {
    std::vector<ActionType> recommendations;
    
    if (!brain_ || !brain_->getPlanner() || !neuralPrediction_) {
        return recommendations;
    }
    
    // For now, return simple action based on current prediction state
    // In a full implementation, this would integrate with action planning
    if (getPredictionError() < 0.3f) {
        recommendations.push_back(ActionType::Wait);
        recommendations.push_back(ActionType::Move);
    } else {
        recommendations.push_back(ActionType::Wait);
        recommendations.push_back(ActionType::TurnLeft);
        recommendations.push_back(ActionType::TurnRight);
    }
    
    return recommendations;
}

void PredictionSystem::applyPredictionErrorPlasticity(float error, const std::vector<float>& currentState) {
    if (!brain_ || !error) {
        return;
    }
    
    // Apply reward-modulated plasticity to prediction-related synapses
    if (brain_->getSTDP()) {
        // STDP is automatically applied during brain step
        // Here we just note that prediction error plasticity should be applied
    }
    
    // Hebbian strengthening of predictive connections based on error
    if (brain_->getHebbian()) {
        // The Hebbian system will strengthen connections when neurons fire together
        // Prediction error modulates this process
    }
}

void PredictionSystem::updateWithNeuromodulation(float dopamine, float curiosity) {
    if (!brain_ || !neuralPrediction_) {
        return;
    }
    
    // Dopamine modulates prediction learning (positive: strengthen predictions, negative: weaken)
    // Curiosity modulates exploration-based prediction
    
    // Update neural prediction with neuromodulatory signals
    // This would be used in Brain::step() integration
}

} // namespace nlm

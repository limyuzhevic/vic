#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    float predictionError;
    float confidence;
    float uncertainty;
    float intensityError;
    float spatialError;
    float temporalError;
    std::vector<float> errorHistory;
    std::vector<float> surpriseHistory;
    std::vector<float> rpeHistory;
    std::vector<StateTransition> transitionModel;
    
    Impl() : predictionError(0.0f), confidence(0.5f), uncertainty(0.5f),
             intensityError(0.0f), spatialError(0.0f), temporalError(0.0f) {}
};

PredictionSystem::PredictionSystem() : neuralPrediction_(nullptr), brain_(nullptr)
    , predictionError_(0.0f), confidence_(0.5f), uncertainty_(0.5f)
    , intensityError_(0.0f), spatialError_(0.0f), temporalError_(0.0f)
    , recentRPE_(0.0f)
    , maxHistorySize_(1000), maxTransitionMemory_(1000)
    , surpriseThreshold_(0.3f), enableMultiStepPrediction_(true)
    , enablePlasticityIntegration_(true)
    , neuralPlanner_(nullptr), agentBrain_(nullptr) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(NeuralPrediction* neuralPrediction, Brain* brain) {
    neuralPrediction_ = neuralPrediction;
    brain_ = brain;
    NLM_LOG_INFO("PredictionSystem initialized with neural substrate and brain integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!neuralPrediction_) {
        return currentState.clone();
    }
    
    // Convert SensoryInput to vector for neural prediction
    const auto& currentData = currentState.getData();
    
    // Use neural prediction system for state transition
    // For now, generate a simple prediction based on learned transitions
    std::unique_ptr<SensoryInput> predictedState = currentState.clone();
    auto& predData = predictedState->getData();
    
    // Apply a simple dynamics model (placeholder for real neural dynamics)
    // In a real implementation, this would use the neural prediction substrate
    // to generate a more sophisticated prediction
    
    // For now, use the neural prediction system if available
    if (neuralPrediction_) {
        // Generate prediction using neural substrate
        auto neuralPred = neuralPrediction_->generatePrediction(0);
        if (!neuralPred.empty() && neuralPred.size() == predData.size()) {
            predData = neuralPred;
        } else {
            // Simple dynamics: small random changes
            for (size_t i = 0; i < predData.size() && i < currentData.size(); ++i) {
                float change = (neuralPrediction_->getPredictionConfidence() * 0.1f) * 
                              ((rand() % 200 - 100) / 100.0f);  // Small random change
                predData[i] = std::clamp(predData[i] + change, 0.0f, 1.0f);
            }
        }
    }
    
    return predictedState;
}

std::vector<std::unique_ptr<SensoryInput>> PredictionSystem::predictActionSequence(
    const std::vector<ActionType>& actions,
    const SensoryInput& currentState,
    size_t horizon) {
    
    std::vector<std::unique_ptr<SensoryInput>> predictions;
    
    if (!enableMultiStepPrediction_ || actions.empty()) {
        predictions.push_back(currentState.clone());
        return predictions;
    }
    
    // Get current state as prediction
    auto currentPred = currentState.clone();
    const auto& currentData = currentPred->getData();
    predictions.push_back(std::move(currentPred));
    
    // Predict each step in the action sequence
    for (size_t i = 0; i < horizon && i < actions.size(); ++i) {
        auto& nextState = predictions.back();
        const auto& stateData = nextState->getData();
        
        // Predict next state based on action
        auto stepPred = predictStateTransition(*nextState, actions[i]);
        predictions.push_back(std::move(stepPred));
    }
    
    return predictions;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictStateTransition(
    const SensoryInput& currentState,
    ActionType action) {
    
    std::unique_ptr<SensoryInput> predictedState = currentState.clone();
    auto& predData = predictedState->getData();
    const auto& currentData = currentState.getData();
    
    // Look up learned transition model for this state-action pair
    bool foundTransition = false;
    float bestConfidence = 0.0f;
    
    for (const auto& transition : pImpl->transitionModel) {
        if (transition.action == action) {
            // Check if we have a similar before-state
            const auto& beforeData = transition.beforeState.getData();
            if (beforeData.size() == currentData.size()) {
                float similarity = computeSimilarity(currentData, beforeData);
                if (similarity > 0.8f && transition.confidence > bestConfidence) {
                    // Use this transition to predict next state
                    const auto& afterData = transition.afterState.getData();
                    predData = afterData;
                    foundTransition = true;
                    bestConfidence = transition.confidence;
                    break;
                }
            }
        }
    }
    
    // If no learned transition found, apply default dynamics
    if (!foundTransition) {
        // Apply simple action-specific dynamics based on neural prediction
        if (neuralPrediction_) {
            auto actionPred = neuralPrediction_->predictActionConsequence(action, currentData);
            if (!actionPred.empty() && actionPred.size() == predData.size()) {
                predData = actionPred;
            }
        }
        
        // Add some randomness based on uncertainty
        float uncertainty = 1.0f - confidence_;
        for (size_t i = 0; i < predData.size(); ++i) {
            float noise = (rand() % 200 - 100) / 100.0f * uncertainty * 0.1f;
            predData[i] = std::clamp(predData[i] + noise, 0.0f, 1.0f);
        }
    }
    
    return predictedState;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    // Compute comprehensive prediction errors
    PredictionErrors errors;
    computePredictionErrors(predicted, actual, errors);
    
    // Update internal state
    predictionError_ = errors.totalError;
    intensityError_ = errors.intensityError;
    spatialError_ = errors.spatialError;
    temporalError_ = errors.temporalError;
    
    // Add to history
    pImpl->errorHistory.push_back(predictionError_);
    
    // Check for surprise
    float surprise = computeSurprise(predicted, actual);
    if (surprise > 0.0f) {
        pImpl->surpriseHistory.push_back(surprise);
    }
    
    // Maintain history size
    if (pImpl->errorHistory.size() > maxHistorySize_) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    if (pImpl->surpriseHistory.size() > maxHistorySize_) {
        pImpl->surpriseHistory.erase(pImpl->surpriseHistory.begin());
    }
    
    // Update confidence based on prediction quality
    updateConfidence();
}

void PredictionSystem::computePredictionErrors(const SensoryInput& predicted,
                                            const SensoryInput& actual,
                                            PredictionErrors& errors) const {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    if (predData.size() != actualData.size() || predData.empty()) {
        errors.intensityError = 1.0f;
        errors.spatialError = 1.0f;
        errors.temporalError = 1.0f;
        errors.totalError = 1.0f;
        return;
    }
    
    // Intensity error: magnitude of deviation
    float intensitySum = 0.0f;
    float spatialSum = 0.0f;
    float temporalSum = 0.0f;
    
    for (size_t i = 0; i < predData.size(); ++i) {
        float diff = predData[i] - actualData[i];
        intensitySum += diff * diff;
        
        // Spatial error: position-based differences (assuming first 10 dimensions are spatial)
        if (i < 10) {
            spatialSum += diff * diff;
        }
        
        // Temporal error: pattern changes over time (simplified)
        if (i == 0) {
            temporalSum += std::abs(diff);
        }
    }
    
    size_t numSamples = predData.size();
    errors.intensityError = std::sqrt(intensitySum / numSamples);
    errors.spatialError = std::sqrt(spatialSum / std::min<size_t>(numSamples, 10));
    errors.temporalError = std::sqrt(temporalSum / numSamples);
    
    // Combine errors for total prediction error
    errors.totalError = std::sqrt(
        errors.intensityError * errors.intensityError +
        errors.spatialError * errors.spatialError +
        errors.temporalError * errors.temporalError
    );
}

PredictionSystem::PredictionErrors PredictionSystem::getPredictionErrors() const {
    PredictionErrors errors;
    errors.intensityError = intensityError_;
    errors.spatialError = spatialError_;
    errors.temporalError = temporalError_;
    errors.totalError = predictionError_;
    return errors;
}

void PredictionSystem::updateConfidence() {
    // Confidence is based on prediction error and surprise
    if (predictionError_ < 0.1f) {
        // Low error = high confidence
        confidence_ = 1.0f - predictionError_ * 2.0f;
    } else if (predictionError_ < 0.3f) {
        // Medium error = medium confidence
        confidence_ = 1.0f - predictionError_ * 0.8f;
    } else {
        // High error = low confidence
        confidence_ = std::max(0.1f, 0.4f - predictionError_ * 0.5f);
    }
    
    // Adjust based on recent surprises
    if (!pImpl->surpriseHistory.empty()) {
        float avgSurprise = std::accumulate(pImpl->surpriseHistory.begin(),
                                           pImpl->surpriseHistory.end(), 0.0f) /
                           pImpl->surpriseHistory.size();
        // Surprise reduces confidence
        confidence_ *= (1.0f - avgSurprise * 0.5f);
    }
    
    // Clamp confidence
    confidence_ = std::clamp(confidence_, 0.1f, 1.0f);
    uncertainty_ = 1.0f - confidence_;
}

float PredictionSystem::getConfidence() const {
    return confidence_;
}

float PredictionSystem::getUncertainty() const {
    return uncertainty_;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->errorHistory;
}

void PredictionSystem::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->surpriseHistory.clear();
    pImpl->rpeHistory.clear();
    confidence_ = 0.5f;
    uncertainty_ = 0.5f;
    recentRPE_ = 0.0f;
}

void PredictionSystem::train(const SensoryInput& observation) {
    // Train the prediction system using the observation
    // In a real implementation, this would update neural weights, etc.
    
    if (!neuralPrediction_) {
        return;
    }
    
    const auto& data = observation.getData();
    
    // Use neural prediction system to learn from observation
    if (neuralPrediction_->getPredictionConfidence() > 0.5f) {
        // Neural system can learn from stable patterns
        neuralPrediction_->recordSensoryState(data, 0);
    }
    
    // Update transition model based on observation
    // This is a simplified version - real implementation would be more sophisticated
}

void PredictionSystem::learnActionEffect(const SensoryInput& beforeState,
                                       ActionType action,
                                       const SensoryInput& afterState,
                                       float reward) {
    StateTransition transition;
    transition.beforeState = beforeState.clone();
    transition.action = action;
    transition.afterState = afterState.clone();
    transition.reward = reward;
    transition.confidence = 0.5f; // Default confidence
    transition.lastObserved = 0;   // Simplified
    
    // Add to transition model
    pImpl->transitionModel.push_back(transition);
    
    // Maintain memory size
    if (pImpl->transitionModel.size() > maxTransitionMemory_) {
        pImpl->transitionModel.erase(pImpl->transitionModel.begin());
    }
}

size_t PredictionSystem::getTransitionCount() const {
    return pImpl->transitionModel.size();
}

bool PredictionSystem::isSurprising(const SensoryInput& predicted,
                                 const SensoryInput& actual) const {
    return computeSurprise(predicted, actual) > surpriseThreshold_;
}

float PredictionSystem::computeSurprise(const SensoryInput& predicted,
                                     const SensoryInput& actual) const {
    PredictionErrors errors;
    computePredictionErrors(predicted, actual, errors);
    
    // Surprise is based on prediction error
    return std::min(1.0f, errors.totalError * 2.0f);
}

float PredictionSystem::getPredictionErrorSignal() const {
    // Returns error signal for reward prediction error computation
    return predictionError_ * 2.0f - 1.0f; // Scale to [-1, 1]
}

float PredictionSystem::getPrecisionWeight() const {
    // Precision-based learning rate (inverse of uncertainty)
    return confidence_ * 0.1f + 0.01f; // Base learning rate with precision scaling
}

float PredictionSystem::getSurpriseSignal() const {
    if (pImpl->surpriseHistory.empty()) {
        return 0.0f;
    }
    float avgSurprise = std::accumulate(pImpl->surpriseHistory.begin(),
                                       pImpl->surpriseHistory.end(), 0.0f) /
                       pImpl->surpriseHistory.size();
    return avgSurprise;
}

void PredictionSystem::applyPredictionErrorToPlasticity(float error, const SensoryInput& predicted,
                                                      const SensoryInput& actual) {
    if (!brain_ || !enablePlasticityIntegration_) {
        return;
    }
    
    PredictionErrors errors;
    computePredictionErrors(predicted, actual, errors);
    
    // Apply reward-modulated plasticity based on prediction error
    // This is similar to how AgentBrain applies reward modulation
    float plasticitySignal = error * 2.0f - 1.0f; // Scale to [-1, 1]
    
    // Modulate STDP with prediction error
    auto* stdp = brain_->getSTDP();
    if (stdp && stdp->getLTPWeight() > 0.0f) {
        float originalLTP = stdp->getLTPWeight();
        float originalLTD = stdp->getLTDWeight();
        
        // Scale plasticity weights based on prediction error
        float plasticityFactor = 0.5f + 0.5f * plasticitySignal;
        plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
        
        // Apply scaled plasticity
        stdp->setLTPWeight(originalLTP * plasticityFactor);
        stdp->setLTDWeight(originalLTD * plasticityFactor);
        
        NLM_LOG_INFO("Prediction error applied to STDP: plasticityFactor = " +
                    std::to_string(plasticityFactor));
    }
    
    // Apply Hebbian plasticity with prediction error
    auto* hebbian = brain_->getHebbian();
    if (hebbian) {
        float hebbianFactor = std::max(0.0f, 1.0f - error);
        // Hebbian strengthening is reduced by prediction error
        NLM_LOG_INFO("Prediction error applied to Hebbian: hebbianFactor = " +
                    std::to_string(hebbianFactor));
    }
    
    // Trigger structural plasticity if error is large
    if (errors.totalError > 0.5f) {
        triggerStructuralPlasticity(predicted, actual);
    }
}

void PredictionSystem::modulateLearningRate(float precision, float error) {
    if (!brain_) {
        return;
    }
    
    // Modulate neural learning rates based on prediction precision and error
    float learningRate = precision * (1.0f - error);
    learningRate = std::clamp(learningRate, 0.01f, 0.5f);
    
    // Update STDP parameters
    auto* stdp = brain_->getSTDP();
    if (stdp) {
        float originalLTP = stdp->getLTPWeight();
        float originalLTD = stdp->getLTDWeight();
        
        // Scale LTP/LTD based on learning rate
        stdp->setLTPWeight(originalLTP * learningRate);
        stdp->setLTDWeight(originalLTD * learningRate);
        
        NLM_LOG_INFO("Learning rate modulated: " + std::to_string(learningRate));
    }
}

void PredictionSystem::enhanceSynapticEligibility(float error) {
    if (!brain_) {
        return;
    }
    
    // Enhance synaptic eligibility traces based on prediction error
    // This prepares the network for stronger weight updates
    
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // Scale eligibility based on prediction error
            if (error > 0.3f) {
                // High error = high eligibility
                syn->addToEligibilityTrace(0.5f * error);
            } else if (error < 0.1f) {
                // Low error = decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

void PredictionSystem::triggerStructuralPlasticity(const SensoryInput& predicted,
                                                 const SensoryInput& actual) {
    if (!brain_) {
        return;
    }
    
    // Trigger structural plasticity changes based on prediction error
    auto* structural = brain_->getStructuralPlasticity();
    if (!structural) {
        return;
    }
    
    PredictionErrors errors;
    computePredictionErrors(predicted, actual, errors);
    
    // Adjust structural plasticity parameters based on error magnitude
    float synaptogenesisRate = 0.0001f * (1.0f - errors.totalError);
    float pruningRate = 0.00001f * errors.totalError;
    
    structural->setSynaptogenesisRate(synaptogenesisRate);
    structural->setPruningRate(pruningRate);
    
    NLM_LOG_INFO("Structural plasticity triggered: synaptogenesis = " +
                std::to_string(synaptogenesisRate) +
                ", pruning = " + std::to_string(pruningRate));
}

void PredictionSystem::computeRewardPredictionError(float actualReward, float predictedReward) {
    // Compute reward prediction error (RPE)
    recentRPE_ = actualReward - predictedReward;
    
    // Store in history
    pImpl->rpeHistory.push_back(recentRPE_);
    
    // Maintain history size
    if (pImpl->rpeHistory.size() > maxHistorySize_) {
        pImpl->rpeHistory.erase(pImpl->rpeHistory.begin());
    }
    
    NLM_LOG_INFO("RPE computed: actual = " + std::to_string(actualReward) +
                ", predicted = " + std::to_string(predictedReward) +
                ", RPE = " + std::to_string(recentRPE_));
}

void PredictionSystem::updateDevelopment(float plasticityModifier) {
    // Update developmental state based on plasticity
    if (!brain_) {
        return;
    }
    
    // Adjust prediction system parameters based on development
    // In early development, predictions are more plastic
    if (plasticityModifier > 0.8f) {
        // High plasticity phase - learn faster
        maxTransitionMemory_ = 500;
        confidence_ = std::min(confidence_ + 0.1f, 1.0f);
    } else if (plasticityModifier < 0.5f) {
        // Low plasticity phase - more stable predictions
        maxTransitionMemory_ = 2000;
        confidence_ = std::max(confidence_ - 0.05f, 0.5f);
    }
    
    NLM_LOG_INFO("PredictionSystem development update: plasticityModifier = " +
                std::to_string(plasticityModifier));
}

} // namespace nlm
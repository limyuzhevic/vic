#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionError::Impl {
    Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    float magnitude;
    float confidence;
    float learningRate;
    float rewardPredictionError;
    std::vector<float> errorHistory;
    std::vector<TimestepDuration> errorTimestamps;
    float temporalDifference;
    float policyGradient;
    float curiosityContribution;
    float neuromodulationLevel;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             magnitude(0.0f), confidence(0.5f), learningRate(0.1f), 
             rewardPredictionError(0.0f), temporalDifference(0.0f), 
             policyGradient(0.0f), curiosityContribution(0.0f), neuromodulationLevel(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual, 
                                  Action* action, Brain* brain) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    pImpl->rewardPredictionError = computeRewardPredictionError(predicted, actual);
    pImpl->temporalDifference = computeTemporalDifference(predicted, actual);
    
    // Update action selection based on error
    if (brain && action) {
        updateActionValues(brain, action);
    }
    
    // Modulate synaptic learning
    if (brain) {
        modulateSynapticLearning(brain);
    }
    
    // Integrate with motor system
    integrateWithMotorSystem(action, brain);
    
    // Advanced learning mechanisms
    implementTemporalDifferenceLearning(brain);
    applyErrorPropagatedLearning(brain);
    implementPolicyGradientIntegration(brain);
    
    pImpl->errorHistory.push_back(pImpl->error);
    pImpl->errorTimestamps.push_back(0);
    
    // Keep history bounded
    if (pImpl->errorHistory.size() > 1000) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
        pImpl->errorTimestamps.erase(pImpl->errorTimestamps.begin());
    }
}

void PredictionError::updateActionValues(Brain* brain, Action* action) {
    if (!brain || !action) return;
    
    // Update action value estimates based on prediction error
    float currentValue = action->getValue();
    float delta = pImpl->error * pImpl->learningRate;
    
    // Standard learning rule: new value = old value + learning_rate * error
    action->setValue(currentValue + delta);
    
    // Apply eligibility trace for delayed learning
    float eligibility = action->getEligibilityTrace();
    eligibility += 1.0f;
    action->setEligibilityTrace(eligibility);
}

void PredictionError::modulateSynapticLearning(float predictionError, Brain* brain) {
    if (!brain) return;
    
    pImpl->error *= predictionError;
    
    // Scale STDP learning rates based on prediction error
    STDP* stdp = brain->getSTDP();
    if (stdp) {
        float modulation = 1.0f + pImpl->error * 0.5f;
        modulation = std::max(0.0f, std::min(2.0f, modulation));
        
        stdp->setLTPWeight(stdp->getLTPWeight() * modulation);
        stdp->setLTDWeight(stdp->getLTDWeight() * modulation);
    }
    
    // Modulate Hebbian learning
    Hebbian* hebbian = brain->getHebbian();
    if (hebbian) {
        float learningRate = hebbian->getLearningRate() * (1.0f + pImpl->error);
        hebbian->setLearningRate(learningRate);
    }
    
    pImpl->neuromodulationLevel = pImpl->error * pImpl->error;
}

void PredictionError::implementRewardPredictionErrorLearning() {
    // Implement reward prediction error for RL
    pImpl->rewardPredictionError = (pImpl->actualValue - pImpl->predictedValue) / 
                                 (std::abs(pImpl->actualValue) + 0.001f);
    
    // Clip to reasonable range
    pImpl->rewardPredictionError = std::max(-1.0f, std::min(1.0f, pImpl->rewardPredictionError));
}

void PredictionError::integrateWithMotorSystem(Action* action, Brain* brain) {
    if (!action || !brain) return;
    
    // Apply prediction error to motor system
    float motorModulation = pImpl->error * action->getValue() * 0.1f;
    action->applyMotorModulation(motorModulation);
    
    // Update action selection probabilities
    action->updateSelectionProbability(pImpl->error);
}

void PredictionError::implementTemporalDifferenceLearning(Brain* brain) {
    if (!brain) return;
    
    // Simplified temporal difference learning
    pImpl->temporalDifference = pImpl->error * 0.9f;  // Discount factor
    
    // Update value function for next state
    float updatedValue = pImpl->actualValue + pImpl->temporalDifference * pImpl->learningRate;
    pImpl->actualValue = updatedValue;
}

void PredictionError::applyErrorPropagatedLearning(Brain* brain) {
    if (!brain) return;
    
    // Forward error propagation through neural networks
    float propagatedError = pImpl->error * 0.5f;  // Error decay factor
    
    // Apply to working memory traces
    NeuralWorkingMemory* workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        workingMemory->strengthenMemory(propagatedError);
    }
    
    // Apply to episodic memory
    NeuralEpisodicMemory* episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        // Find similar episodes to update
        auto currentPattern = std::vector<float>(10, pImpl->error); // Simplified
        auto similar = episodicMemory->retrieveSimilar(currentPattern, 5);
        for (const auto* episode : similar) {
            episodicMemory->updateRelevance(episode->timestamp, propagatedError * 0.1f);
        }
    }
}

void PredictionError::implementPolicyGradientIntegration(Brain* brain) {
    if (!brain) return;
    
    // Compute policy gradient contribution
    pImpl->policyGradient = pImpl->error * pImpl->error * pImpl->learningRate;
    
    // Apply to action selection policy
    NeuralPlanner* planner = brain->getPlanner();
    if (planner) {
        float policyModulation = pImpl->policyGradient * 0.1f;
        planner->modulateActionSelection(policyModulation);
    }
}

float PredictionError::computeRewardPredictionError(float predicted, float actual) const {
    // RPE = reward - prediction
    return actual - predicted;
}

float PredictionError::computeTemporalDifference(float predicted, float actual) const {
    // Simplified TD error: reward + gamma * next_value - current_value
    return actual - predicted;
}

void PredictionError::clearHistory() {
    pImpl->errorHistory.clear();
    pImpl->errorTimestamps.clear();
    pImpl->error = 0.0f;
    pImpl->magnitude = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

float PredictionError::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float PredictionError::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float PredictionError::getPolicyGradient() const {
    return pImpl->policyGradient;
}

float PredictionError::getNeuromodulationLevel() const {
    return pImpl->neuromodulationLevel;
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->errorHistory;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float PredictionError::getLearningRate() const {
    return pImpl->learningRate;
}

void PredictionError::setConfidence(float confidence) {
    pImpl->confidence = std::clamp(confidence, 0.0f, 1.0f);
}

} // namespace nlm

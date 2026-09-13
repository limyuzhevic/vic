#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    float tdError; // Temporal difference error
    float learningRate;
    float discountFactor;
    
    // Advanced prediction error computation
    float rewardPredictionError; // RPE (reward prediction error)
    float temporalError;         // Temporal component
    float stateError;            // State prediction error
    float policyError;           // Policy evaluation error
    
    // History and memory
    std::vector<float> history;
    std::vector<float> rewardHistory;
    std::vector<float> temporalHistory;
    float lastStateHash;
    
    // Temporal dynamics
    float temporalDecay;
    float temporalIntegration;
    float predictionGain;
    
    // Learning system integration
    float eligibilityTrace;
    float plasticityModulation;
    float neuromodulatorInfluence;
    
    // Random number generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             tdError(0.0f), learningRate(0.1f), discountFactor(0.95f),
             rewardPredictionError(0.0f), temporalError(0.0f), stateError(0.0f), policyError(0.0f),
             lastStateHash(0.0f), temporalDecay(0.9f), temporalIntegration(0.0f),
             predictionGain(1.0f), eligibilityTrace(0.0f), plasticityModulation(1.0f),
             neuromodulatorInfluence(1.0f)
    {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->history.clear();
    pImpl->rewardHistory.clear();
    pImpl->temporalHistory.clear();
    
    NLM_LOG_INFO("Advanced PredictionError system initialized with temporal difference learning");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Basic prediction error
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    
    // Advanced prediction error components
    pImpl->rewardPredictionError = actual - pImpl->predictedValue;
    pImpl->temporalError = pImpl->rewardPredictionError * pImpl->predictionGain;
    pImpl->stateError = computeStateError();
    pImpl->policyError = computePolicyError();
    
    // Combine components with weighted sum
    float weightedError = pImpl->rewardPredictionError * 0.5f + 
                         pImpl->temporalError * 0.2f + 
                         pImpl->stateError * 0.2f + 
                         pImpl->policyError * 0.1f;
    
    // Update history
    pImpl->history.push_back(weightedError);
    pImpl->rewardHistory.push_back(pImpl->rewardPredictionError);
    pImpl->temporalHistory.push_back(pImpl->temporalError);
    
    // Maintain memory capacity (keep last 1000 entries)
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
        pImpl->temporalHistory.erase(pImpl->temporalHistory.begin());
    }
}

float PredictionError::computeStateError() const {
    // Compute state prediction error based on brain state
    if (!pImpl->brain) return 0.0f;
    
    // Simple state error based on neuron firing rate variance
    float totalFiringRate = 0.0f;
    int firingCount = 0;
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const Neuron* n : pop->getNeurons()) {
                float firing = n->getState().firingRate;
                totalFiringRate += firing;
                firingCount++;
            }
        }
    }
    
    if (firingCount == 0) return 0.0f;
    
    float avgFiring = totalFiringRate / firingCount;
    float variance = 0.0f;
    
    // Compute variance around mean
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const Neuron* n : pop->getNeurons()) {
                float firing = n->getState().firingRate;
                variance += (firing - avgFiring) * (firing - avgFiring);
            }
        }
    }
    
    variance /= firingCount;
    
    // State error is normalized variance (higher variance = higher prediction error)
    return std::sqrt(variance) * 0.5f;
}

float PredictionError::computePolicyError() const {
    // Compute policy evaluation error based on action selection
    if (!pImpl->brain) return 0.0f;
    
    // Simple policy error: difference between selected action value and expected
    // This is a placeholder - in a real implementation, this would access
    // the action selection mechanism
    float actionValueEstimate = 0.0f;
    float actualOutcome = pImpl->actualValue;
    
    // Basic policy error calculation
    return actualOutcome - actionValueEstimate;
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
    
    // Update reward prediction error with temporal dynamics
    if (pImpl->actualValue != 0.0f) {
        pImpl->rewardPredictionError = pImpl->actualValue - pImpl->predictedValue;
    }
    
    // Apply temporal decay to prediction
    pImpl->predictedValue *= pImpl->temporalDecay;
    
    NLM_LOG_DEBUG("Prediction updated: predicted=%f, actual=%f, RPE=%f", 
                  pImpl->predictedValue, pImpl->actualValue, pImpl->rewardPredictionError);
}

void PredictionError::updateTemporalDynamics(float dt) {
    // Update temporal component of prediction error
    if (!pImpl->history.empty()) {
        // Compute temporal difference error
        float currentError = pImpl->history.back();
        float previousError = pImpl->history.size() > 1 ? pImpl->history[pImpl->history.size() - 2] : 0.0f;
        
        pImpl->tdError = (currentError - previousError * pImpl->discountFactor) / (1.0f + pImpl->discountFactor);
        
        // Temporal integration
        pImpl->temporalIntegration += pImpl->tdError * dt;
        pImpl->temporalIntegration *= pImpl->temporalDecay;
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

const std::vector<float>& PredictionError::getRewardHistory() const {
    return pImpl->rewardHistory;
}

const std::vector<float>& PredictionError::getTemporalHistory() const {
    return pImpl->temporalHistory;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
    pImpl->rewardHistory.clear();
    pImpl->temporalHistory.clear();
    pImpl->temporalIntegration = 0.0f;
}

float PredictionError::getMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getTemporalDifferenceError() const {
    return pImpl->tdError;
}

float PredictionError::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float PredictionError::getStateError() const {
    return pImpl->stateError;
}

float PredictionError::getPolicyError() const {
    return pImpl->policyError;
}

float PredictionError::getTemporalIntegration() const {
    return pImpl->temporalIntegration;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.01f, 1.0f);
}

void PredictionError::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.5f, 0.999f);
}

void PredictionError::setPredictionGain(float gain) {
    pImpl->predictionGain = std::clamp(gain, 0.1f, 2.0f);
}

void PredictionError::setTemporalDecay(float decay) {
    pImpl->temporalDecay = std::clamp(decay, 0.5f, 0.999f);
}

void PredictionError::updateEligibilityTrace(float traceValue) {
    pImpl->eligibilityTrace = traceValue * pImpl->learningRate;
}

void PredictionError::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
}

float PredictionError::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void PredictionError::setPlasticityModulation(float modulation) {
    pImpl->plasticityModulation = std::clamp(modulation, 0.1f, 2.0f);
}

void PredictionError::setNeuromodulatorInfluence(float influence) {
    pImpl->neuromodulatorInfluence = std::clamp(influence, 0.0f, 1.0f);
}

float PredictionError::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

float PredictionError::getNeuromodulatorInfluence() const {
    return pImpl->neuromodulatorInfluence;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->tdError = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->temporalError = 0.0f;
    pImpl->stateError = 0.0f;
    pImpl->policyError = 0.0f;
    pImpl->temporalIntegration = 0.0f;
    
    clearHistory();
    
    NLM_LOG_INFO("PredictionError system reset");
}

} // namespace nlm

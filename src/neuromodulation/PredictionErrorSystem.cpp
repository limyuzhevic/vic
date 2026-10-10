#include "PredictionErrorSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

struct PredictionErrorSystem::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    MultiStepPredictionState multiStepState;
    PredictionErrorState errorState;
    
    // History tracking
    std::vector<float> predictionErrorHistory;
    std::vector<std::vector<float>> multiStepErrorHistory;
    std::vector<float> predictionHistory;
    std::vector<float> targetHistory;
    std::vector<float> rewardHistory;
    
    // TD learning components
    std::vector<float> temporalDifferenceErrors;
    std::vector<std::vector<float>> eligibilityTraceHistory;
    
    // Integration state
    float dopamineIntegration;
    float curiosityIntegration;
    float noveltyIntegration;
    
    // Configuration
    int maxHorizons;
    float defaultDiscountFactor;
    float defaultLearningRate;
    float eligibilityTraceDecay;
    
    // Safety bounds
    float minPredictionError;
    float maxPredictionError;
    float minUncertainty;
    float maxUncertainty;
    
    Impl() : brain(nullptr), engine(nullptr),
             maxHorizons(10), defaultDiscountFactor(0.9f), defaultLearningRate(0.01f),
             eligibilityTraceDecay(0.9f),
             dopamineIntegration(0.0f), curiosityIntegration(0.0f), noveltyIntegration(0.0f),
             minPredictionError(-10.0f), maxPredictionError(10.0f),
             minUncertainty(0.0f), maxUncertainty(10.0f) {
        
        // Initialize eligibility trace history for each horizon
        eligibilityTraceHistory.resize(maxHorizons);
        for (auto& traceHistory : eligibilityTraceHistory) {
            traceHistory.reserve(1000);
        }
        
        // Initialize multi-step error history
        multiStepErrorHistory.resize(maxHorizons);
        for (auto& errorHistory : multiStepErrorHistory) {
            errorHistory.reserve(100);
        }
        
        // Initialize other histories
        predictionErrorHistory.reserve(1000);
        predictionHistory.reserve(1000);
        targetHistory.reserve(1000);
        rewardHistory.reserve(1000);
        temporalDifferenceErrors.reserve(1000);
    }
};

PredictionErrorSystem::PredictionErrorSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("PredictionErrorSystem created");
}

PredictionErrorSystem::~PredictionErrorSystem() {
    NLM_LOG_INFO("PredictionErrorSystem destroyed");
}

PredictionErrorSystem::PredictionErrorSystem(PredictionErrorSystem&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

PredictionErrorSystem& PredictionErrorSystem::operator=(PredictionErrorSystem&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool PredictionErrorSystem::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("PredictionErrorSystem initialization failed: invalid dependencies");
            return false;
        }
        
        NLM_LOG_INFO("PredictionErrorSystem initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("PredictionErrorSystem initialization failed: " + std::string(e.what()));
        return false;
    }
}

void PredictionErrorSystem::update(TimestepDuration dt, SimulationStep step) {
    // Update eligibility traces
    updateEligibilityTraces(step);
    
    // Decay eligibility traces
    decayEligibilityTraces(dt);
    
    // Update multi-step predictions
    if (enableMultiStepLearning_) {
        updateMultiStepPredictions();
    }
    
    // Update state
    updateState(dt);
}

void PredictionErrorSystem::computeError(float predictedValue, float actualValue, float reward) {
    // Standard prediction error: δ = actual - predicted
    float error = actualValue - predictedValue;
    
    // Update global prediction error state
    pImpl->errorState.globalPredictionError = error;
    pImpl->errorState.errorMagnitude = std::abs(error);
    pImpl->errorState.errorDirection = (error > 0.0f) ? 1.0f : -1.0f;
    
    // Update cumulative error
    pImpl->errorState.cumulativeError = pImpl->errorState.cumulativeError * 0.9f + error * 0.1f;
    
    // Update temporal gradient (simple finite difference)
    static float previousError = 0.0f;
    pImpl->errorState.temporalGradient = error - previousError;
    previousError = error;
    
    // Update prediction variance
    static float errorSum = 0.0f;
    static float errorSumSq = 0.0f;
    errorSum += error;
    errorSumSq += error * error;
    float meanError = errorSum / (pImpl->predictionErrorHistory.size() + 1);
    float variance = (errorSumSq - meanError * meanError * (pImpl->predictionErrorHistory.size() + 1)) /
                     std::max(1.0f, pImpl->predictionErrorHistory.size());
    pImpl->errorState.predictionVariance = variance;
    
    // Update uncertainty estimate based on error magnitude
    pImpl->errorState.uncertaintyEstimate = std::min(pImpl->maxUncertainty,
                                                    pImpl->errorState.uncertaintyEstimate + 
                                                    pImpl->errorState.errorMagnitude * 0.1f);
    
    // Update confidence value (inverse of uncertainty)
    pImpl->errorState.confidenceValue = 1.0f / (1.0f + pImpl->errorState.uncertaintyEstimate);
    
    // Store in history
    pImpl->predictionErrorHistory.push_back(error);
    pImpl->predictionHistory.push_back(predictedValue);
    pImpl->targetHistory.push_back(actualValue);
    pImpl->rewardHistory.push_back(reward);
    
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
        pImpl->predictionHistory.erase(pImpl->predictionHistory.begin());
        pImpl->targetHistory.erase(pImpl->targetHistory.begin());
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Apply temporal difference learning
    applyTemporalDifferenceLearning(error, static_cast<SimulationStep>(pImpl->predictionErrorHistory.size() - 1));
    
    NLM_LOG_DEBUG("PredictionErrorSystem: Error computed - " + std::to_string(error));
}

void PredictionErrorSystem::computeMultiStepError(const std::vector<float>& predictions,
                                                 const std::vector<float>& targets,
                                                 const std::vector<float>& rewards) {
    if (predictions.empty() || targets.empty()) return;
    
    int numHorizons = std::min({predictions.size(), targets.size(), 
                               static_cast<size_t>(pImpl->maxHorizons)});
    
    // Compute multi-step errors for each horizon
    for (int horizon = 0; horizon < numHorizons; ++horizon) {
        if (!pImpl->multiStepState.isActive[horizon]) {
            continue;
        }
        
        float prediction = predictions[horizon];
        float target = targets[horizon];
        float reward = (horizon < rewards.size()) ? rewards[horizon] : 0.0f;
        
        // Compute multi-step TD error
        float tdError = computeMultiStepTDError(prediction, reward, 
                                               std::vector<float>(predictions.begin() + horizon + 1,
                                                                predictions.end()));
        
        // Store multi-step error
        pImpl->multiStepState.predictionErrors[horizon] = target - prediction;
        pImpl->multiStepState.rewardPredictions[horizon] = reward;
        
        // Add to history
        pImpl->multiStepErrorHistory[horizon].push_back(tdError);
        if (pImpl->multiStepErrorHistory[horizon].size() > 100) {
            pImpl->multiStepErrorHistory[horizon].erase(pImpl->multiStepErrorHistory[horizon].begin());
        }
    }
}

void PredictionErrorSystem::updatePredictions(float newPrediction, float newTarget, float newReward) {
    // Shift predictions and add new one
    for (int horizon = pImpl->maxHorizons - 1; horizon > 0; --horizon) {
        pImpl->multiStepState.predictionValues[horizon] = 
            pImpl->multiStepState.predictionValues[horizon - 1];
        pImpl->multiStepState.targetValues[horizon] = 
            pImpl->multiStepState.targetValues[horizon - 1];
        pImpl->multiStepState.rewardPredictions[horizon] = 
            pImpl->multiStepState.rewardPredictions[horizon - 1];
        pImpl->multiStepState.eligibilityTraces[horizon] = 
            pImpl->multiStepState.eligibilityTraces[horizon - 1] * pImpl->eligibilityTraceDecay;
        pImpl->multiStepState.isActive[horizon] = pImpl->multiStepState.isActive[horizon - 1];
    }
    
    // Set first horizon to new values
    pImpl->multiStepState.predictionValues[0] = newPrediction;
    pImpl->multiStepState.targetValues[0] = newTarget;
    pImpl->multiStepState.rewardPredictions[0] = newReward;
    pImpl->multiStepState.eligibilityTraces[0] = 1.0f;  // Reset eligibility trace
    pImpl->multiStepState.isActive[0] = true;
}

void PredictionErrorSystem::updateEligibilityTraces(SimulationStep step) {
    // Update eligibility trace for each active horizon
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            // Decay and update eligibility trace
            pImpl->multiStepState.eligibilityTraces[horizon] *= pImpl->eligibilityTraceDecay;
            
            // Add new eligibility based on current prediction error
            pImpl->multiStepState.eligibilityTraces[horizon] += 
                pImpl->errorState.globalPredictionError * pImpl->defaultLearningRate;
            
            // Store in history
            pImpl->eligibilityTraceHistory[horizon].push_back(
                pImpl->multiStepState.eligibilityTraces[horizon]);
            
            if (pImpl->eligibilityTraceHistory[horizon].size() > 1000) {
                pImpl->eligibilityTraceHistory[horizon].erase(
                    pImpl->eligibilityTraceHistory[horizon].begin());
            }
        }
    }
}

void PredictionErrorSystem::decayEligibilityTraces(TimestepDuration dt) {
    // Decay eligibility traces over time
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            pImpl->multiStepState.eligibilityTraces[horizon] *= 
                std::pow(pImpl->eligibilityTraceDecay, dt);
        }
    }
}

void PredictionErrorSystem::applyTemporalDifferenceLearning(float tdError, SimulationStep step) {
    // Store TD error
    pImpl->temporalDifferenceErrors.push_back(tdError);
    if (pImpl->temporalDifferenceErrors.size() > 1000) {
        pImpl->temporalDifferenceErrors.erase(pImpl->temporalDifferenceErrors.begin());
    }
    
    // Apply eligibility trace-based learning
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            float learningRate = pImpl->multiStepState.learningRates[horizon];
            float discount = pImpl->multiStepState.discountFactors[horizon];
            
            // Update prediction based on TD error
            float predictionUpdate = learningRate * tdError * 
                                   pImpl->multiStepState.eligibilityTraces[horizon];
            
            pImpl->multiStepState.predictionValues[horizon] += predictionUpdate;
            
            // Apply bounds
            pImpl->multiStepState.predictionValues[horizon] = 
                std::clamp(pImpl->multiStepState.predictionValues[horizon],
                          pImpl->minPredictionError, pImpl->maxPredictionError);
        }
    }
}

void PredictionErrorSystem::applyRewardPredictionLearning(float error, float reward) {
    // Apply reward prediction learning based on error
    float learningRate = pImpl->defaultLearningRate * 2.0f;  // Higher learning rate for reward prediction
    
    // Update prediction towards target
    float predictionUpdate = learningRate * (reward - pImpl->multiStepState.predictionValues[0]);
    
    pImpl->multiStepState.predictionValues[0] += predictionUpdate;
    
    // Update reward prediction for all horizons
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            pImpl->multiStepState.rewardPredictions[horizon] = reward;
        }
    }
}

void PredictionErrorSystem::applyIntrinsicCuriosityLearning(float novelty, float predictionError) {
    // Apply intrinsic curiosity learning if enabled
    if (!enableIntrinsicCuriosityLearning_) {
        return;
    }
    
    // Enhance prediction learning with novelty
    float curiosityFactor = novelty * 0.5f;
    float learningRate = pImpl->defaultLearningRate * (1.0f + curiosityFactor);
    
    // Update predictions based on combined error
    float combinedError = predictionError + curiosityFactor * novelty;
    
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            float predictionUpdate = learningRate * combinedError * 
                                   pImpl->multiStepState.eligibilityTraces[horizon];
            
            pImpl->multiStepState.predictionValues[horizon] += predictionUpdate;
        }
    }
}

void PredictionErrorSystem::updateState(TimestepDuration dt) {
    // Update uncertainty estimate over time
    pImpl->errorState.uncertaintyEstimate *= (1.0f - 0.01f * static_cast<float>(dt));
    pImpl->errorState.uncertaintyEstimate = std::max(pImpl->minUncertainty, 
                                                    pImpl->errorState.uncertaintyEstimate);
    
    // Update confidence based on uncertainty
    pImpl->errorState.confidenceValue = 1.0f / (1.0f + pImpl->errorState.uncertaintyEstimate);
    
    // Apply bounds
    enforceBounds();
}

void PredictionErrorSystem::reset() {
    NLM_LOG_INFO("PredictionErrorSystem reset");
    
    // Reset state
    pImpl->multiStepState = MultiStepPredictionState();
    pImpl->errorState = PredictionErrorState();
    
    // Clear histories
    clearHistory();
}

void PredictionErrorSystem::setMaxHorizons(int horizons) {
    pImpl->maxHorizons = std::max(1, std::min(horizons, 100));  // Cap at 100
    
    // Resize arrays based on new max horizons
    pImpl->multiStepState.predictionValues.resize(pImpl->maxHorizons);
    pImpl->multiStepState.targetValues.resize(pImpl->maxHorizons);
    pImpl->multiStepState.rewardPredictions.resize(pImpl->maxHorizons);
    pImpl->multiStepState.predictionErrors.resize(pImpl->maxHorizons);
    pImpl->multiStepState.eligibilityTraces.resize(pImpl->maxHorizons);
    pImpl->multiStepState.isActive.resize(pImpl->maxHorizons);
    pImpl->multiStepState.learningRates.resize(pImpl->maxHorizons);
    pImpl->multiStepState.discountFactors.resize(pImpl->maxHorizons);
    
    // Reset isActive for new horizons
    for (int i = 0; i < pImpl->maxHorizons; ++i) {
        pImpl->multiStepState.isActive[i] = (i == 0);
    }
}

void PredictionErrorSystem::integrateWithDopamine(float dopamineLevel) {
    pImpl->dopamineIntegration = dopamineLevel;
    
    // Adjust learning rates based on dopamine
    float dopamineFactor = 1.0f + dopamineLevel * 0.5f;
    
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        pImpl->multiStepState.learningRates[horizon] *= dopamineFactor;
    }
}

void PredictionErrorSystem::integrateWithCuriosity(float curiosityLevel) {
    pImpl->curiosityIntegration = curiosityLevel;
    
    // Increase uncertainty estimate with curiosity
    pImpl->errorState.uncertaintyEstimate += curiosityLevel * 0.1f;
}

void PredictionErrorSystem::integrateWithNovelty(float noveltyLevel) {
    pImpl->noveltyIntegration = noveltyLevel;
    
    // Enhance prediction updates with novelty
    pImpl->errorState.uncertaintyEstimate = 
        std::min(pImpl->maxUncertainty, pImpl->errorState.uncertaintyEstimate + noveltyLevel * 0.05f);
}

std::vector<float> PredictionErrorSystem::computeFuturePredictions(float currentPrediction, int horizons) const {
    std::vector<float> futurePredictions;
    futurePredictions.reserve(horizons);
    
    float prediction = currentPrediction;
    for (int i = 0; i < horizons && i < pImpl->maxHorizons; ++i) {
        prediction *= pImpl->multiStepState.discountFactors[std::min(i, pImpl->maxHorizons - 1)];
        futurePredictions.push_back(prediction);
    }
    
    return futurePredictions;
}

std::vector<float> PredictionErrorSystem::computeFutureRewards(float currentReward, int horizons) const {
    std::vector<float> futureRewards;
    futureRewards.reserve(horizons);
    
    float reward = currentReward;
    for (int i = 0; i < horizons; ++i) {
        futureRewards.push_back(reward);
        reward *= pImpl->defaultDiscountFactor;
    }
    
    return futureRewards;
}

float PredictionErrorSystem::computeMultiStepTDError(float currentValue, float currentReward,
                                                    const std::vector<float>& futureRewards) const {
    float tdError = currentReward;
    
    // Add discounted future rewards
    for (size_t i = 0; i < futureRewards.size(); ++i) {
        tdError += std::pow(pImpl->defaultDiscountFactor, static_cast<float>(i + 1)) * futureRewards[i];
    }
    
    // Subtract current value estimate
    tdError -= currentValue;
    
    return tdError;
}

const std::vector<float>& PredictionErrorSystem::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

const std::vector<std::vector<float>>& PredictionErrorSystem::getMultiStepErrorHistory() const {
    return pImpl->multiStepErrorHistory;
}

const std::vector<float>& PredictionErrorSystem::getPredictionHistory() const {
    return pImpl->predictionHistory;
}

const std::vector<float>& PredictionErrorSystem::getTargetHistory() const {
    return pImpl->targetHistory;
}

void PredictionErrorSystem::clearHistory() {
    pImpl->predictionErrorHistory.clear();
    pImpl->multiStepErrorHistory.clear();
    pImpl->predictionHistory.clear();
    pImpl->targetHistory.clear();
    pImpl->rewardHistory.clear();
    pImpl->temporalDifferenceErrors.clear();
    
    for (auto& traceHistory : pImpl->eligibilityTraceHistory) {
        traceHistory.clear();
    }
}

bool PredictionErrorSystem::validateSafetyConstraints() const {
    // Check prediction error bounds
    if (pImpl->errorState.globalPredictionError < pImpl->minPredictionError ||
        pImpl->errorState.globalPredictionError > pImpl->maxPredictionError) {
        NLM_LOG_ERROR("PredictionErrorSystem: Global prediction error out of bounds");
        return false;
    }
    
    // Check uncertainty bounds
    if (pImpl->errorState.uncertaintyEstimate < pImpl->minUncertainty ||
        pImpl->errorState.uncertaintyEstimate > pImpl->maxUncertainty) {
        NLM_LOG_ERROR("PredictionErrorSystem: Uncertainty estimate out of bounds");
        return false;
    }
    
    // Check confidence value bounds
    if (pImpl->errorState.confidenceValue < 0.0f || pImpl->errorState.confidenceValue > 1.0f) {
        NLM_LOG_ERROR("PredictionErrorSystem: Confidence value out of bounds");
        return false;
    }
    
    // Check for numerical instabilities
    if (std::isnan(pImpl->errorState.globalPredictionError) ||
        std::isnan(pImpl->errorState.uncertaintyEstimate) ||
        std::isnan(pImpl->errorState.confidenceValue)) {
        NLM_LOG_ERROR("PredictionErrorSystem: Numerical instability detected");
        return false;
    }
    
    return true;
}

void PredictionErrorSystem::enforceBounds() {
    pImpl->errorState.globalPredictionError = 
        std::clamp(pImpl->errorState.globalPredictionError,
                  pImpl->minPredictionError, pImpl->maxPredictionError);
    
    pImpl->errorState.uncertaintyEstimate = 
        std::clamp(pImpl->errorState.uncertaintyEstimate,
                  pImpl->minUncertainty, pImpl->maxUncertainty);
    
    pImpl->errorState.confidenceValue = 
        std::clamp(pImpl->errorState.confidenceValue, 0.0f, 1.0f);
}

void PredictionErrorSystem::exportState(std::ostream& stream) const {
    stream.write(reinterpret_cast<const char*>(&pImpl->errorState), sizeof(PredictionErrorState));
    stream.write(reinterpret_cast<const char*>(&pImpl->multiStepState), sizeof(MultiStepPredictionState));
    
    // Export histories (simplified)
    size_t errorHistorySize = pImpl->predictionErrorHistory.size();
    stream.write(reinterpret_cast<const char*>(&errorHistorySize), sizeof(size_t));
    if (errorHistorySize > 0) {
        stream.write(reinterpret_cast<const char*>(pImpl->predictionErrorHistory.data()),
                    errorHistorySize * sizeof(float));
    }
}

void PredictionErrorSystem::importState(std::istream& stream) {
    stream.read(reinterpret_cast<char*>(&pImpl->errorState), sizeof(PredictionErrorState));
    stream.read(reinterpret_cast<char*>(&pImpl->multiStepState), sizeof(MultiStepPredictionState));
    
    // Import histories
    size_t errorHistorySize;
    stream.read(reinterpret_cast<char*>(&errorHistorySize), sizeof(size_t));
    pImpl->predictionErrorHistory.resize(errorHistorySize);
    if (errorHistorySize > 0) {
        stream.read(reinterpret_cast<char*>(pImpl->predictionErrorHistory.data()),
                   errorHistorySize * sizeof(float));
    }
}

void PredictionErrorSystem::updateMultiStepPredictions() {
    // Update multi-step predictions based on current error
    for (int horizon = 0; horizon < pImpl->maxHorizons; ++horizon) {
        if (pImpl->multiStepState.isActive[horizon]) {
            // Apply TD learning to update predictions
            float tdError = pImpl->errorState.globalPredictionError;
            float learningRate = pImpl->multiStepState.learningRates[horizon];
            
            // Update prediction
            float predictionUpdate = learningRate * tdError * 
                                   pImpl->multiStepState.eligibilityTraces[horizon];
            
            pImpl->multiStepState.predictionValues[horizon] += predictionUpdate;
            
            // Apply bounds
            pImpl->multiStepState.predictionValues[horizon] = 
                std::clamp(pImpl->multiStepState.predictionValues[horizon],
                          pImpl->minPredictionError, pImpl->maxPredictionError);
        }
    }
}

float PredictionErrorSystem::getGlobalPredictionError() const {
    return pImpl->errorState.globalPredictionError;
}

float PredictionErrorSystem::getErrorMagnitude() const {
    return pImpl->errorState.errorMagnitude;
}

float PredictionErrorSystem::getTemporalGradient() const {
    return pImpl->errorState.temporalGradient;
}

float PredictionErrorSystem::getUncertaintyEstimate() const {
    return pImpl->errorState.uncertaintyEstimate;
}

float PredictionErrorSystem::getPredictionErrorForHorizon(int horizon) const {
    if (horizon >= 0 && horizon < pImpl->maxHorizons) {
        return pImpl->multiStepState.predictionErrors[horizon];
    }
    return 0.0f;
}

void PredictionErrorSystem::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm
#include "DopamineSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct DopamineSystem::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    DopamineSystemState state;
    TDLearningParams tdParams;
    std::vector<DopamineLearningRule> learningRules;
    RewardPredictionError rpe;
    
    // History tracking
    std::vector<float> rewardHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> tonicLevelHistory;
    std::vector<float> eligibilityTraceHistory;
    
    // Multi-step learning
    std::vector<std::vector<float>> multiStepPredictions;
    std::vector<std::vector<float>> futureRewards;
    std::vector<float> temporalDifferenceErrors;
    
    // Integration state
    float curiosityIntegration;
    float noveltyIntegration;
    float predictionErrorIntegration;
    
    // Safety state
    float minSafeLevel;
    float maxSafeLevel;
    
    Impl() : brain(nullptr), engine(nullptr),
             curiosityIntegration(0.0f), noveltyIntegration(0.0f), 
             predictionErrorIntegration(0.0f),
             minSafeLevel(0.0f), maxSafeLevel(1.0f) {
        
        // Initialize default learning rules
        learningRules.resize(5);
        learningRules[0] = {DopamineLearningRule::RuleType::RESCORLA_WALTERS, 1.0f};
        learningRules[1] = {DopamineLearningRule::RuleType::TEMPORAL_DIFFERENCE, 1.0f};
        learningRules[2] = {DopamineLearningRule::RuleType::SHAPED_REWARD, 0.8f};
        learningRules[3] = {DopamineLearningRule::RuleType::INTRINSIC_CURIOUSITY, 0.6f};
        learningRules[4] = {DopamineLearningRule::RuleType::MULTI_STEP_LOOKAHEAD, 0.7f};
        
        // Initialize eligibility trace decay
        for (int i = 0; i < 100; ++i) {
            eligibilityTraceHistory.push_back(0.0f);
        }
    }
};

DopamineSystem::DopamineSystem() : pImpl(new Impl) {
    NLM_LOG_INFO("DopamineSystem created");
}

DopamineSystem::~DopamineSystem() {
    NLM_LOG_INFO("DopamineSystem destroyed");
}

DopamineSystem::DopamineSystem(DopamineSystem&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

DopamineSystem& DopamineSystem::operator=(DopamineSystem&& other) noexcept {
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

bool DopamineSystem::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        // Validate initialization
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("DopamineSystem initialization failed: invalid dependencies");
            return false;
        }
        
        NLM_LOG_INFO("DopamineSystem initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("DopamineSystem initialization failed: " + std::string(e.what()));
        return false;
    }
}

void DopamineSystem::update(TimestepDuration dt, SimulationStep step) {
    // Update eligibility traces
    updateEligibilityTraces(step);
    
    // Update multi-step predictions
    if (enableMultiStepLearning_) {
        updateMultiStepPredictions({});
    }
    
    // Update internal state
    updateState(dt);
    
    // Process any pending rewards/errors
    // (These would be called from external systems)
}

void DopamineSystem::processReward(float reward, float predictedReward) {
    // Compute reward prediction error
    RewardPredictionError rpe;
    rpe.actualReward = reward;
    rpe.rewardPrediction = predictedReward;
    rpe.tdError = reward - predictedReward;
    rpe.intrinsicError = computeIntrinsicCuriosity();
    
    // Update state based on RPE
    updateFromRPE(rpe);
    
    // Store in history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 1000) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Signal phasic response
    signalPhasicResponse(std::abs(rpe.tdError) * 0.5f);
    
    NLM_LOG_DEBUG("DopamineSystem: Reward processed - RPE: " + std::to_string(rpe.tdError));
}

void DopamineSystem::signalPhasicResponse(float magnitude) {
    // Scale magnitude based on current state
    float scaledMagnitude = magnitude * pImpl->state.rewardSensitivity;
    
    // Update phasic component
    pImpl->state.phasicComponent = std::min(pImpl->state.phasicComponent + scaledMagnitude, 
                                          pImpl->config_.safetyLevelThreshold - pImpl->state.tonicComponent);
    
    // Ensure total concentration stays within bounds
    float totalWithPhasic = pImpl->state.tonicComponent + pImpl->state.phasicComponent;
    if (totalWithPhasic > pImpl->config_.safetyLevelThreshold) {
        pImpl->state.phasicComponent = pImpl->config_.safetyLevelThreshold - pImpl->state.tonicComponent;
    }
}

void DopamineSystem::signalTonicResponse(float baseline) {
    // Update tonic component with smooth transitions
    float transitionRate = 0.1f;  // Smooth transition rate
    float delta = baseline - pImpl->state.tonicComponent;
    pImpl->state.tonicComponent += delta * transitionRate;
    
    // Enforce bounds
    pImpl->state.tonicComponent = std::clamp(pImpl->state.tonicComponent, 
                                           pImpl->config_.safetyLevelThreshold * 0.1f,
                                           pImpl->config_.safetyLevelThreshold);
}

void DopamineSystem::computeTemporalDifferenceError(float predictedValue, float actualValue, 
                                                    float reward, float nextPredictedValue) {
    // Standard TD error: δ = r + γV(s') - V(s)
    float tdError = reward + pImpl->tdParams.gamma * nextPredictedValue - predictedValue;
    
    // Update prediction error state
    pImpl->rpe.tdError = tdError;
    pImpl->rpe.predictionError = tdError;
    
    // Update eligibility traces
    pImpl->rpe.eligibilityTrace = std::max(0.0f, pImpl->rpe.eligibilityTrace + tdError * pImpl->tdParams.lambda);
    
    // Store in history
    pImpl->predictionErrorHistory.push_back(tdError);
    if (pImpl->predictionErrorHistory.size() > 1000) {
        pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
    }
    
    // Apply learning rules
    applyLearningRules(tdError);
}

void DopamineSystem::updateEligibilityTraces(SimulationStep step) {
    // Decay eligibility traces over time
    for (auto& trace : pImpl->eligibilityTraceHistory) {
        trace *= (1.0f - pImpl->tdParams.lambda);
    }
    
    // Add new eligibility trace for this step
    pImpl->eligibilityTraceHistory[step % pImpl->eligibilityTraceHistory.size()] = 
        pImpl->rpe.tdError * pImpl->tdParams.alpha;
}

void DopamineSystem::updateMultiStepPredictions(const std::vector<float>& futureRewards) {
    if (futureRewards.empty()) return;
    
    // Store future rewards for multi-step learning
    pImpl->futureRewards.push_back(futureRewards);
    if (pImpl->futureRewards.size() > 10) {
        pImpl->futureRewards.erase(pImpl->futureRewards.begin());
    }
    
    // Compute multi-step TD errors
    float accumulatedReward = 0.0f;
    for (size_t i = 0; i < futureRewards.size(); ++i) {
        accumulatedReward += futureRewards[i] * std::pow(pImpl->tdParams.gamma, static_cast<float>(i));
        
        MultiStepPrediction pred;
        pred.currentPrediction = accumulatedReward;
        pred.targetValue = accumulatedReward + futureRewards[i] * pImpl->tdParams.gamma;
        pred.futurePredictions = std::vector<float>(3, accumulatedReward * 0.9f);
        pred.futureRewards = std::vector<float>(3, futureRewards[i]);
        pred.gamma = pImpl->tdParams.gamma;
        pred.lambda = pImpl->tdParams.lambda;
        
        pImpl->multiStepPredictions.push_back(pred);
    }
}

void DopamineSystem::modulatePlasticity(float& synapticWeight, float predictionError) const {
    if (!pImpl->engine) return;
    
    // Apply dopamine-modulated plasticity
    float plasticityFactor = 1.0f + pImpl->state.plasticityModulation * predictionError * 0.1f;
    
    // Apply scaling based on receptor saturation
    float saturationFactor = 1.0f - pImpl->state.receptorSaturation;
    plasticityFactor *= saturationFactor;
    
    // Apply bounds
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 10.0f);
    
    synapticWeight *= plasticityFactor;
}

void DopamineSystem::modulateLearningRate(float& learningRate, float novelty) const {
    // Increase learning rate with novelty
    float noveltyFactor = 1.0f + novelty * 0.5f;
    
    // Apply dopamine sensitivity
    float sensitivityFactor = 1.0f + pImpl->state.predictionErrorSensitivity * 0.2f;
    
    learningRate *= noveltyFactor * sensitivityFactor;
    
    // Apply bounds
    learningRate = std::clamp(learningRate, 0.001f, 0.5f);
}

void DopamineSystem::modulateAttention(float& attentionWeight, float predictionError) const {
    // Attentional modulation based on prediction error
    float attentionFactor = 1.0f + std::abs(predictionError) * 0.3f;
    
    // Apply receptor saturation
    float saturationFactor = 1.0f - pImpl->state.receptorSaturation;
    attentionFactor *= saturationFactor;
    
    attentionWeight *= attentionFactor;
    attentionWeight = std::clamp(attentionWeight, 0.1f, 2.0f);
}

void DopamineSystem::updateState(TimestepDuration dt) {
    // Update phasic component (decay)
    pImpl->state.phasicComponent = std::max(0.0f, pImpl->state.phasicComponent - 
                                          pImpl->state.degradationRate * static_cast<float>(dt));
    
    // Update intracellular storage (placeholder for vesicular storage)
    // This would be more complex in a real implementation
    
    // Update reuptake
    float reuptake = pImpl->state.releaseProbability * pImpl->state.totalConcentration;
    pImpl->state.totalConcentration = std::max(0.0f, 
                                              pImpl->state.totalConcentration - reuptake * static_cast<float>(dt));
    
    // Apply homeostatic regulation
    enforceHomeostasis();
    
    // Store tonic level history
    pImpl->tonicLevelHistory.push_back(pImpl->state.tonicComponent);
    if (pImpl->tonicLevelHistory.size() > 1000) {
        pImpl->tonicLevelHistory.erase(pImpl->tonicLevelHistory.begin());
    }
}

void DopamineSystem::reset() {
    NLM_LOG_INFO("DopamineSystem reset");
    
    // Reset state
    pImpl->state.phasicComponent = 0.0f;
    pImpl->state.totalConcentration = pImpl->state.homeostasisValue;
    pImpl->state.receptorSaturation = 0.0f;
    pImpl->state.intracellularStorage = 0.0f;
    
    // Reset learning state
    pImpl->rpe = RewardPredictionError();
    pImpl->eligibilityTraceHistory.clear();
    for (int i = 0; i < 100; ++i) {
        pImpl->eligibilityTraceHistory.push_back(0.0f);
    }
    
    // Clear history
    clearHistory();
}

void DopamineSystem::setTDLearningParams(const TDLearningParams& params) {
    pImpl->tdParams = params;
}

void DopamineSystem::setLearningRules(const std::vector<DopamineLearningRule>& rules) {
    pImpl->learningRules = rules;
}

const std::vector<float>& DopamineSystem::getRewardHistory() const {
    return pImpl->rewardHistory;
}

const std::vector<float>& DopamineSystem::getPredictionErrorHistory() const {
    return pImpl->predictionErrorHistory;
}

const std::vector<float>& DopamineSystem::getTonicLevelHistory() const {
    return pImpl->tonicLevelHistory;
}

void DopamineSystem::clearHistory() {
    pImpl->rewardHistory.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->tonicLevelHistory.clear();
}

void DopamineSystem::integrateWithCuriosity(float curiosityLevel) {
    pImpl->curiosityIntegration = curiosityLevel;
    // Modulate dopamine response based on curiosity
    if (enableIntrinsicCuriosity_) {
        pImpl->state.intracellularStorage += curiosityLevel * 0.01f;
    }
}

void DopamineSystem::integrateWithNovelty(float noveltyLevel) {
    pImpl->noveltyIntegration = noveltyLevel;
    // Enhance dopaminergic response to novel stimuli
    pImpl->state.releaseProbability += noveltyLevel * 0.005f;
}

void DopamineSystem::integrateWithPredictionError(float predictionError) {
    pImpl->predictionErrorIntegration = predictionError;
    // Adjust sensitivity based on prediction error
    pImpl->state.predictionErrorSensitivity = 1.0f + std::abs(predictionError) * 0.2f;
}

bool DopamineSystem::validateSafetyConstraints() const {
    if (pImpl->state.totalConcentration > pImpl->config_.safetyLevelThreshold) {
        NLM_LOG_ERROR("DopamineSystem: Total concentration exceeds safety threshold");
        return false;
    }
    
    if (pImpl->state.phasicComponent > pImpl->state.tonicComponent) {
        NLM_LOG_ERROR("DopamineSystem: Phasic component exceeds tonic component");
        return false;
    }
    
    // Check for numerical instabilities
    if (std::isnan(pImpl->state.totalConcentration) || 
        std::isinf(pImpl->state.totalConcentration)) {
        NLM_LOG_ERROR("DopamineSystem: Numerical instability detected");
        return false;
    }
    
    return true;
}

void DopamineSystem::enforceHomeostasis() {
    // Maintain total concentration around homeostatic target
    float deviation = pImpl->state.totalConcentration - pImpl->state.homeostasisValue;
    float correction = -deviation * 0.1f;  // Simple proportional controller
    
    pImpl->state.totalConcentration += correction;
    
    // Ensure bounds
    pImpl->state.totalConcentration = std::clamp(pImpl->state.totalConcentration,
                                                 pImpl->config_.safetyLevelThreshold * 0.1f,
                                                 pImpl->config_.safetyLevelThreshold);
}

float DopamineSystem::getPhasicComponent() const {
    return pImpl->state.phasicComponent;
}

float DopamineSystem::getTonicComponent() const {
    return pImpl->state.tonicComponent;
}

float DopamineSystem::getTotalConcentration() const {
    return pImpl->state.tonicComponent + pImpl->state.phasicComponent;
}

float DopamineSystem::getPlasticityModulation() const {
    return pImpl->state.plasticityModulation;
}

float DopamineSystem::getRewardPredictionError() const {
    return pImpl->rpe.tdError;
}

float DopamineSystem::getIntrinsicCuriosity() const {
    return pImpl->rpe.intrinsicError;
}

float DopamineSystem::computeIntrinsicCuriosity() const {
    // Placeholder for actual intrinsic curiosity computation
    // In a real system, this would be based on prediction errors, novelty, etc.
    return 0.5f;
}

void DopamineSystem::applyLearningRules(float tdError) {
    // Apply each learning rule
    for (const auto& rule : pImpl->learningRules) {
        switch (rule.type) {
            case DopamineLearningRule::RuleType::RESCORLA_WALTERS:
                // Rescorla-Wagner rule: ΔV = α * β * (λ - V)
                break;
            case DopamineLearningRule::RuleType::TEMPORAL_DIFFERENCE:
                // TD learning already handled
                break;
            case DopamineLearningRule::RuleType::SHAPED_REWARD:
                // Shaped reward modulation
                break;
            case DopamineLearningRule::RuleType::INTRINSIC_CURIOUSITY:
                // Intrinsic curiosity modulation
                break;
            case DopamineLearningRule::RuleType::MULTI_STEP_LOOKAHEAD:
                // Multi-step lookahead
                break;
        }
    }
}

void DopamineSystem::updateFromRPE(const RewardPredictionError& rpe) {
    // Update state based on reward prediction error
    float sign = rpe.tdError > 0.0f ? 1.0f : -1.0f;
    
    // Adjust receptor saturation based on error magnitude
    pImpl->state.receptorSaturation = std::min(1.0f, 
                                             pImpl->state.receptorSaturation + 
                                             std::abs(rpe.tdError) * 0.1f);
    
    // Adjust release probability
    pImpl->state.releaseProbability = std::min(1.0f, 
                                              pImpl->state.releaseProbability + 
                                              sign * rpe.tdError * 0.05f);
    
    // Update plasticity modulation
    pImpl->state.plasticityModulation = 1.0f + rpe.tdError * 0.3f;
}

void DopamineSystem::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm
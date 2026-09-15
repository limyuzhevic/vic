#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine level with biologically plausible dynamics
    float level;
    
    // Baseline tonic level
    float baseline;
    
    // Peak level for burst responses
    float peak;
    
    // Time constants for dynamics
    float decayRate;
    float releaseRate;
    
    // Short-term plasticity (eligibility trace)
    float eligibilityTrace;
    float traceDecayRate;
    
    // Filtering for prediction error
    float filteredPredictionError;
    float filterFactor;
    
    // Gain factors for different components
    float predictionErrorGain;
    float contextGain;
    float rewardGain;
    
    // Biological constraints
    float minLevel;
    float maxLevel;
    float timeConstant;
    
    // State variables
    float voltageDependency;
    float neuromodulatoryState;
    
    // Eligibility trace history
    std::vector<float> eligibilityTraceHistory;
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.5f)
        , decayRate(0.1f)
        , releaseRate(2.0f)
        , eligibilityTrace(0.0f)
        , traceDecayRate(0.05f)
        , filteredPredictionError(0.0f)
        , filterFactor(0.3f)
        , predictionErrorGain(1.5f)
        , contextGain(0.3f)
        , rewardGain(2.0f)
        , minLevel(0.0f)
        , maxLevel(2.0f)
        , timeConstant(0.2f)
        , voltageDependency(1.0f)
        , neuromodulatoryState(0.0f) {
        eligibilityTraceHistory.reserve(100);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, pImpl->minLevel, pImpl->maxLevel);
}

float Dopamine::getPlasticityFactor() const {
    // More realistic plasticity factor based on dopamine level
    // Higher dopamine increases plasticity but with diminishing returns
    // Also includes voltage dependency for biological realism
    float voltageEffect = 1.0f + 0.5f * pImpl->voltageDependency;
    float saturation = 1.0f - std::exp(-pImpl->level * 2.0f); // Sigmoidal saturation
    
    return pImpl->baseline + (1.0f - pImpl->baseline) * saturation * voltageEffect;
}

void Dopamine::update(float predictionError, float reward, float contextSignal, TimestepDuration dt) {
    // Phasic component: prediction error signal (burst responses)
    float phasicPredictionError = predictionError * pImpl->predictionErrorGain;
    
    // Baseline modulation: context and reward prediction
    float baselineMod = contextSignal * pImpl->contextGain;
    
    // Reward component: unexpected reward (in unexpected reward responses)
    float rewardComponent = reward * pImpl->rewardGain;
    
    // Time-dependent filtering of prediction error
    pImpl->filteredPredictionError = pImpl->filteredPredictionError * (1.0f - pImpl->filterFactor) + 
                                     predictionError * pImpl->filterFactor;
    
    // Calculate new dopamine level using biologically plausible dynamics
    float newLevel = pImpl->filteredPredictionError * phasicPredictionError + 
                     baselineMod + 
                     rewardComponent;
    
    // Apply differential equation for dynamics (first-order low-pass filter)
    // level += (newLevel - level) * dt / timeConstant
    pImpl->level += (newLevel - pImpl->level) * static_cast<float>(dt) / pImpl->timeConstant;
    
    // Apply biological constraints
    pImpl->level = std::clamp(pImpl->level, pImpl->minLevel, pImpl->maxLevel);
    
    // Decay towards baseline when no strong signals
    if (std::abs(predictionError) < 0.01f && std::abs(reward) < 0.01f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    }
    
    // Update eligibility trace for reward-modulated learning
    updateEligibilityTrace(predictionError, reward, dt);
    
    // Apply neuromodulatory effects to plasticity rules
    applyNeuromodulationEffects(dt);
}

void Dopamine::updateEligibilityTrace(float predictionError, float reward, TimestepDuration dt) {
    // Phasic dopamine modulates eligibility traces (short-term synaptic plasticity)
    // Higher prediction error and reward increase eligibility trace
    float phasicSignal = predictionError * pImpl->predictionErrorGain + reward * pImpl->rewardGain;
    
    // Add to eligibility trace
    pImpl->eligibilityTrace += phasicSignal * static_cast<float>(dt);
    
    // Clamp eligibility trace to prevent runaway growth
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
    
    // Decay eligibility trace over time
    pImpl->eligibilityTrace *= std::pow(1.0f - pImpl->traceDecayRate, static_cast<float>(dt));
    
    // Store history for analysis
    pImpl->eligibilityTraceHistory.push_back(pImpl->eligibilityTrace);
    if (pImpl->eligibilityTraceHistory.size() > 100) {
        pImpl->eligibilityTraceHistory.erase(pImpl->eligibilityTraceHistory.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // More realistic reward prediction error signaling
    // Includes both immediate and filtered responses
    float filteredError = error * pImpl->filterFactor + 
                         pImpl->filteredPredictionError * (1.0f - pImpl->filterFactor);
    
    // Burst response proportional to prediction error
    float burst = std::max(0.0f, filteredError * pImpl->releaseRate);
    
    // Apply burst with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update eligibility trace for learning
    updateEligibilityTrace(filteredError, 0.0f, 0.001f); // Assuming small dt
}

void Dopamine::signalReward(float reward) {
    // Reward signaling includes both expected and unexpected components
    // Context-dependent response magnitude
    
    // Calculate expected reward component based on baseline
    float expectedReward = getLevel() * pImpl->rewardGain * 0.5f;
    
    // Actual prediction error for unexpected reward
    float predictionError = reward - expectedReward;
    
    // Use unified update function
    update(predictionError, reward, 1.0f, 0.001f); // Assuming small dt
}

void Dopamine::applyNeuromodulationEffects(TimestepDuration dt) {
    // Apply neuromodulatory effects to plasticity rules
    // This would integrate with the brain's plasticity system
    // For now, we'll store the eligibility trace for use by other systems
    
    // The eligibility trace represents the synaptic modifications that should
    // occur when reward is received. This would be used by:
    // 1. STDP plasticity rules
    // 2. Reward-modulated learning
    // 3. Structural plasticity
    
    // In a real implementation, this would interact with the brain's
    // plasticity system to apply weighted updates to synapses
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= std::pow(1.0f - decayRate, 0.001f); // Assuming dt=0.001
}

void Dopamine::setVoltageDependency(float dependency) {
    pImpl->voltageDependency = std::clamp(dependency, 0.1f, 2.0f);
}

void Dopamine::setContextGain(float gain) {
    pImpl->contextGain = std::clamp(gain, 0.0f, 1.0f);
}

void Dopamine::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::clamp(gain, 0.0f, 5.0f);
}

void Dopamine::setRewardGain(float gain) {
    pImpl->rewardGain = std::clamp(gain, 0.0f, 5.0f);
}

float Dopamine::getFilteredPredictionError() const {
    return pImpl->filteredPredictionError;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->filteredPredictionError = 0.0f;
    pImpl->voltageDependency = 1.0f;
    pImpl->neuromodulatoryState = 0.0f;
}

} // namespace nlm

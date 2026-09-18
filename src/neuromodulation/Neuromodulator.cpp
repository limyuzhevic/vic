#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float predictionError;
    
    // Dopamine dynamics for real reinforcement learning
    float phineasGageEffect;  // Personality changes from injury
    float burstPhases[3];    // Phasic bursts at rewards, prediction errors, violations
    float tonicLevel;        // Baseline dopamine for alertness
    
    // Learning-related components
    float rewardPrediction;   // Expected future rewards
    float predictionError;   // TD error (delta)
    float learningRate;      // How quickly predictions update
    float eligibilityTrace;  // For reward-modulated synaptic learning
    
    // Behavioral modulation
    float explorationBias;   // More exploration when learning
    float exploitationWeight; // Favor known good strategies
    float contextSignal;     // What behavior should do
    
    // Biological realism
    float vesiclePool;       // Available dopamine vesicles
    float reuptakeRate;      // DA reuptake speed
    float diffusionConstant; // DA diffusion in extracellular space
    
    // Fatigue and adaptation
    float fatigueLevel;      // Depletion from high firing
    float adaptationTime;    // Time constant for adaptation
    float stressLevel;       // Stress affects dopamine release
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.0f), adaptationRate(0.05f), predictionError(0.0f),
             phineasGageEffect(1.0f), tonicLevel(0.15f), rewardPrediction(0.0f),
             learningRate(0.01f), eligibilityTrace(0.0f), explorationBias(0.5f),
             exploitationWeight(0.5f), contextSignal(0.0f), vesiclePool(1.0f),
             reuptakeRate(0.2f), diffusionConstant(0.5f), fatigueLevel(0.0f),
             adaptationTime(100.0f), stressLevel(0.0f) {
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Integrate multiple influences on dopamine level
    float totalLevel = pImpl->level;
    
    // Tonic baseline with phasic modulation
    totalLevel = totalLevel * 0.7f + pImpl->tonicLevel * 0.3f;
    
    // Apply fatigue (depletion from high activity)
    totalLevel *= (1.0f - pImpl->fatigueLevel * 0.2f);
    
    // Stress effect (stress increases baseline)
    totalLevel *= (1.0f + pImpl->stressLevel * 0.3f);
    
    // Phineas Gage effect (personality changes)
    totalLevel *= pImpl->phineasGageEffect;
    
    // Ensure reasonable bounds
    totalLevel = std::clamp(totalLevel, 0.0f, 1.0f);
    
    return totalLevel;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity based on level
    // More biologically realistic implementation
    
    if (getLevel() < 0.1f) {
        // Low DA: weak plasticity, stabilized learning
        return 0.2f + 0.3f * getLevel();
    } else if (getLevel() < 0.5f) {
        // Moderate DA: balanced learning and stability
        return 0.5f + 0.2f * getLevel();
    } else if (getLevel() < 0.8f) {
        // High DA: strong plasticity, exploration
        return 0.8f + 0.2f * getLevel();
    } else {
        // Very high DA: excessive plasticity, instability
        return 1.2f - 0.5f * getLevel();
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple time scales
    // dt is in milliseconds
    
    // Decay towards baseline (tonic regulation)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Fatigue recovery
    pImpl->fatigueLevel = std::max(0.0f, pImpl->fatigueLevel - dt * 0.001f);
    
    // Adaptation to stress
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - dt * 0.0005f);
    
    // Update eligibility trace for reward-modulated learning
    if (pImpl->eligibilityTrace > 0.0f) {
        pImpl->eligibilityTrace *= std::pow(0.9f, dt * 0.001f);
    }
}

void Dopamine::signalReward(float reward) {
    // Reward signaling with burst dynamics
    // Phasic dopamine burst at reward delivery
    
    // Burst magnitude scales with reward salience
    float burstMagnitude = reward * pImpl->releaseRate;
    
    // Apply burst with vesicle availability constraint
    float availableVesicles = std::min(burstMagnitude, pImpl->vesiclePool);
    pImpl->level = std::min(pImpl->peak, pImpl->level + availableVesicles);
    
    // Update vesicle pool
    pImpl->vesiclePool -= availableVesicles;
    
    // Trigger learning
    if (availableVesicles > 0.01f) {
        pImpl->predictionError = 1.0f;  // Clear prediction error for reward
    }
    
    // Fatigue from high firing
    pImpl->fatigueLevel += availableVesicles * 0.1f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TD error)
    // Phasic bursts for positive prediction errors (better than expected)
    // Dip for negative prediction errors (worse than expected)
    
    // Calculate TD error: δ = r + γV(s') - V(s)
    // For simplicity: error represents the discrepancy
    
    float tdError = error;
    
    // Phase-based response
    if (std::abs(tdError) > 0.01f) {
        // Phasic burst or dip based on error sign
        if (tdError > 0) {
            // Reward prediction error (better than expected)
            // Burst magnitude scales with error size
            float burst = tdError * pImpl->releaseRate;
            pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
            
            // Strong prediction error signal for learning
            pImpl->predictionError = tdError;
            
            // Update reward prediction
            pImpl->rewardPrediction = std::min(1.0f, pImpl->rewardPrediction + tdError * pImpl->learningRate);
            
        } else {
            // Negative prediction error (worse than expected)
            // Dip in dopamine level
            float dip = std::abs(tdError) * pImpl->releaseRate * 0.8f;
            pImpl->level = std::max(0.0f, pImpl->level - dip);
            
            // Weaker prediction error signal
            pImpl->predictionError = tdError * 0.5f;
        }
    }
    
    // Update context signal based on error
    pImpl->contextSignal = std::clamp(pImpl->contextSignal + tdError * 0.01f, -1.0f, 1.0f);
    
    // Fatigue from high processing
    pImpl->fatigueLevel += std::abs(tdError) * 0.05f;
}

void Dopamine::signalNovelty(float novelty) {
    // Novelty signaling for curiosity
    // Increases exploration of unknown states
    
    // Modulate novelty detection based on current state
    float noveltyResponse = novelty * pImpl->releaseRate * 0.5f;
    
    // Apply to exploration bias
    pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + noveltyResponse * 0.1f);
    
    // Also applies to tonic level for broader effects
    pImpl->tonicLevel = std::min(0.5f, pImpl->tonicLevel + noveltyResponse * 0.05f);
}

void Dopamine::signalStress(float stress) {
    // Stress signaling
    // Stress affects dopamine system and behavior
    
    // Immediate stress response
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stress * 0.1f);
    
    // Chronic stress reduces baseline
    pImpl->baseline *= (1.0f - stress * 0.2f);
    
    // Induce learning via stress hormones
    if (stress > 0.5f) {
        // High stress promotes learning from mistakes
        pImpl->predictionError = std::max(pImpl->predictionError, stress * 0.5f);
    }
}

void Dopamine::signalActionSelection(float actionValue, float alternativeValue) {
    // Action selection signaling
    // Sets context for choosing between actions
    
    // Calculate preference signal
    float preference = actionValue - alternativeValue;
    
    // Update exploitation bias
    pImpl->exploitationWeight = std::min(1.0f, pImpl->exploitationWeight + preference * 0.01f);
    pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - preference * 0.01f);
    
    // Signal the chosen action
    pImpl->contextSignal = preference;
}

float Dopamine::getExplorationBias() const {
    return pImpl->explorationBias;
}

float Dopamine::getExploitationWeight() const {
    return pImpl->exploitationWeight;
}

float Dopamine::getContextSignal() const {
    return pImpl->contextSignal;
}

float Dopamine::getPredictionErrorSignal() const {
    return pImpl->predictionError;
}

void Dopamine::reset() {
    // Reset dopamine state (e.g., for sleep/rest)
    pImpl->level = pImpl->baseline;
    pImpl->vesiclePool = 1.0f;
    pImpl->fatigueLevel = 0.0f;
    pImpl->predictionError = 0.0f;
}

} // namespace nlm

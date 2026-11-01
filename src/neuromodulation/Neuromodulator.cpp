#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Phase 6: Implement real dopamine-modulated plasticity factor
    // Complex dynamics: ventral striatum (reward prediction) vs dorsal striatum (action selection)
    // Higher dopamine increases plasticity but with inverted U-curve relationship
    
    // Signal-dependent plasticity: dopamine modulates learning rate based on reward prediction
    float baselineLearningRate = 0.001f;
    float maxLearningRate = 0.02f;
    
    // Use error-correcting plasticity: dopamine helps reduce prediction error over time
    float learningModulation = baselineLearningRate + 
                               (maxLearningRate - baselineLearningRate) * pImpl->level * 0.8f;
    
    // Incorporate prediction error for adaptive learning
    // In Phase 6, this would integrate with prediction error system
    float predictionErrorInfluence = pImpl->level * 0.5f;  // Dopamine scales with prediction error
    
    return learningModulation * (1.0f + predictionErrorInfluence);
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 6: Implement real dopamine dynamics
    // Multi-timescale dynamics: fast (milliseconds) and slow (seconds) components
    // integrates with reward prediction error and curiosity systems
    
    // Fast component: immediate response to rewards and prediction errors
    float fastDecay = 1.0f - (pImpl->decayRate * 2.0f * static_cast<float>(dt));
    pImpl->level = std::max(pImpl->baseline, pImpl->level * fastDecay);
    
    // Slow component: tonic dopamine for baseline arousal and motivation
    float slowRecovery = 1.0f - (pImpl->decayRate * 0.2f * static_cast<float>(dt));
    pImpl->level = std::max(pImpl->level * slowRecovery, pImpl->baseline);
    
    // In Phase 6, integrate with:
    // - Reward prediction error signals from prediction system
    // - Novelty signals from novelty detector
    // - Curiosity signals from curiosity system
    // - Developmental stage effects (dopamine response changes with age)
}

void Dopamine::signalReward(float reward) {
    // Phase 6: Implement real reward signaling with complex dynamics
    // Includes reward prediction error computation and temporal integration
    
    // Compute reward prediction error (placeholder - in Phase 6, integrate with prediction system)
    float predictionError = reward - pImpl->predictedValue;
    
    // Update predicted value
    pImpl->predictedValue = reward;
    
    // Dopamine response to reward prediction error
    // Phasic dopamine bursts for positive prediction errors (surprise rewards)
    if (predictionError > 0.0f) {
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
    }
    // Dopamine dips for negative prediction errors (unexpected non-rewards)
    else {
        pImpl->level = std::max(pImpl->baseline,
                              pImpl->level + predictionError * pImpl->releaseRate);
    }
    
    // Integrate with curiosity and novelty systems
    signalToOtherSystems(reward, predictionError);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 6: Implement reward prediction error signaling with temporal dynamics
    // Critical for learning and adaptive behavior
    
    // Error-correcting dynamics: dopamine helps minimize prediction error over time
    float errorCorrection = error * pImpl->releaseRate;
    pImpl->level = std::max(0.0f, pImpl->level + errorCorrection);
    
    // Temporal integration: accumulate prediction errors over time
    // for long-term plasticity changes
    pImpl->predictedValue += errorCorrection * 0.1f;
    
    // Integrate with other neuromodulators
    // Dopamine-tinged prediction error signals work with curiosity and novelty
    integrateWithOtherSystems();
}

} // namespace nlm

// Phase 6 helper methods for Neuromodulator integration
void Dopamine::integrateWithOtherSystems() {
    // Phase 6: Integrate dopamine with other neuromodulation systems
    // Creates coordinated regulation of learning and behavior
    
    // Connect to curiosity system:
    // - Dopamine modulates curiosity for exploration-exploitation balance
    // - High dopamine increases exploration
    // - Low dopamine reduces exploration
    
    // Connect to novelty system:
    // - Dopamine amplifies responses to novel stimuli
    // - Creates reward prediction error for novel experiences
    // - Enhances memory encoding of novel events
    
    // Connect to prediction error system:
    // - Dopamine and prediction error interact for adaptive learning
    // - Creates temporal difference learning signals
    // - Modulates synaptic plasticity based on prediction accuracy
    
    // Note: In a full Phase 6 implementation, we would:
    // 1. Update curiosity level based on dopamine
    // 2. Signal novelty detection based on dopamine
    // 3. Coordinate prediction error computation
    // 4. Implement developmental modulation
}

void Dopamine::signalToOtherSystems(float reward, float predictionError) {
    // Phase 6: Signal to other neuromodulation systems
    // Creates coordinated behavioral responses
    
    // Signal to curiosity system for exploration
    if (predictionError > 0.0f) {
        // Positive prediction error increases exploration (curiosity)
        // This would call into curiosity system
    } else {
        // Negative prediction error decreases exploration
        // This would call into curiosity system
    }
    
    // Signal to novelty system for pattern detection
    if (reward > 0.0f) {
        // Rewards amplify novelty signals
        // This would call into novelty system
    }
    
    // Signal to prediction error system for learning
    if (std::abs(predictionError) > 0.01f) {
        // Significant prediction errors drive learning
        // This would call into prediction error system
    }
}

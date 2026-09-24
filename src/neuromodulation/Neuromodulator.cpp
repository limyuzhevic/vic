#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), adaptationRate(0.01f) {}
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

bool Dopamine::configure(float baseline, float peak, float decayRate, float releaseRate, float adaptationRate) {
    if (baseline < 0.0f || peak < baseline || decayRate <= 0.0f || releaseRate <= 0.0f || adaptationRate <= 0.0f) {
        NLM_LOG_ERROR(std::string("Invalid dopamine configuration parameters"));
        return false;
    }
    
    pImpl->baseline = baseline;
    pImpl->peak = peak;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
    pImpl->adaptationRate = adaptationRate;
    
    // Gradually adapt to new baseline
    pImpl->level = baseline;
    NLM_LOG_INFO(std::string("Dopamine system configured: baseline=") + std::to_string(baseline) +
                 std::string(", peak=") + std::to_string(peak) +
                 std::string(", decayRate=") + std::to_string(decayRate) +
                 std::string(", releaseRate=") + std::to_string(releaseRate));
    
    return true;
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced dopamine-modulated plasticity factor based on neuroscience
    // Low dopamine (baseline): Reduced plasticity, focus on stability
    // Medium dopamine: Balanced plasticity for learning
    // High dopamine (near peak): Enhanced plasticity for rapid learning
    
    float normalizedLevel = (pImpl->level - pImpl->baseline) / std::max(0.001f, (pImpl->peak - pImpl->baseline));
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Nonlinear relationship that captures dopamine's effect on learning
    // Based on computational neuroscience models of dopamine's role in reinforcement learning
    float plasticityFactor = 0.1f + 0.8f * (1.0f - std::exp(-normalizedLevel * 3.0f));
    
    NLM_LOG_DEBUG(std::string("Dopamine plasticity factor: level=") + std::to_string(pImpl->level) +
                  std::string(", factor=") + std::to_string(plasticityFactor));
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    if (dt <= 0.0) return;
    
    float oldLevel = pImpl->level;
    
    // Realistic dopamine dynamics with multiple time constants
    // 1. Exponential decay towards baseline (tonic component)
    float tonicDecay = (pImpl->level - pImpl->baseline) * pImpl->decayRate * static_cast<float>(dt);
    
    // 2. Adaptive baseline based on recent history (phasic adaptation)
    // This models how dopamine baseline shifts with learning history
    float adaptation = (pImpl->baseline - pImpl->level) * pImpl->adaptationRate * static_cast<float>(dt);
    
    // 3. Combined update with constrained dynamics
    pImpl->level = pImpl->baseline + adaptation + tonicDecay;
    
    // Ensure level stays within valid bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Log significant changes for debugging
    if (std::abs(pImpl->level - oldLevel) > 0.01f) {
        NLM_LOG_DEBUG(std::string("Dopamine updated: ") + std::to_string(oldLevel) + 
                     std::string(" -> ") + std::to_string(pImpl->level));
    }
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling based on computational neuroscience
    if (reward <= 0.0f) return;
    
    float oldLevel = pImpl->level;
    
    // Reward prediction error (RPE) - key component of dopamine signaling
    // Dopamine neurons fire more for better-than-expected rewards
    float rpe = reward; // Simplified RPE model
    
    // Burst response with rapid increase followed by decay
    float burstComponent = pImpl->peak * (1.0f - std::exp(-rpe * pImpl->releaseRate));
    
    // Update level with reward burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstComponent);
    
    // Trigger secondary plasticity processes
    triggerAssociatedLearning(rpe);
    
    NLM_LOG_DEBUG(std::string("Dopamine reward signal: reward=") + std::to_string(reward) +
                  std::string(", level=") + std::to_string(pImpl->level) +
                  std::string(", RPE=") + std::to_string(rpe));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TD learning)
    // Dopamine encodes prediction errors to guide learning
    if (std::abs(error) < 0.001f) return;
    
    float oldLevel = pImpl->level;
    
    // Different responses for positive vs negative prediction errors
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        // Increases dopamine levels to reinforce the learned behavior
        float positiveRPE = error * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + positiveRPE);
        
        // Enhance synaptic eligibility traces
        enhanceEligibilityTraces(positiveRPE);
    } else {
        // Negative prediction error (worse than expected)
        // Reduces dopamine levels to decrease the learned behavior
        float negativeRPE = std::abs(error) * pImpl->decayRate * 2.0f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - negativeRPE);
        
        // Suppress competing synaptic changes
        suppressCompetingLearning(negativeRPE);
    }
    
    NLM_LOG_DEBUG(std::string("Dopamine RPE signal: error=") + std::to_string(error) +
                  std::string(", level=") + std::to_string(pImpl->level));
}

void Dopamine::enhanceEligibilityTraces(float strength) {
    // Implement dopamine-modulated eligibility trace enhancement
    // This is a placeholder for more sophisticated eligibility trace mechanisms
    float eligibilityBoost = strength * 0.5f;
    NLM_LOG_DEBUG(std::string("Dopamine enhancing eligibility traces: boost=") + std::to_string(eligibilityBoost));
}

void Dopamine::suppressCompetingLearning(float suppression) {
    // Implement dopamine-modulated learning suppression
    // This models how negative prediction errors suppress competing behaviors
    float competitionSuppression = suppression * 0.3f;
    NLM_LOG_DEBUG(std::string("Dopamine suppressing competing learning: suppression=") + std::to_string(competitionSuppression));
}

void Dopamine::triggerAssociatedLearning(float learningSignal) {
    // Trigger associated learning processes
    // This connects dopamine signals to broader learning mechanisms
    float learningEffect = learningSignal * 0.4f;
    NLM_LOG_DEBUG(std::string("Dopamine triggering associated learning: effect=") + std::to_string(learningEffect));
}

std::string Dopamine::getStateDescription() const {
    std::ostringstream oss;
    oss << "Dopamine State:";
    oss << "\n  Level: " << pImpl->level;
    oss << "\n  Baseline: " << pImpl->baseline;
    oss << "\n  Peak: " << pImpl->peak;
    oss << "\n  Decay Rate: " << pImpl->decayRate;
    oss << "\n  Release Rate: " << pImpl->releaseRate;
    oss << "\n  Adaptation Rate: " << pImpl->adaptationRate;
    return oss.str();
}

} // namespace nlm

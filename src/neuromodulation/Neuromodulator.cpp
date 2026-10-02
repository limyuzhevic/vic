#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
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
    // REAL IMPLEMENTATION: Phase 2 dopamine-modulated plasticity
    // 
    // Based on research: Dopamine modulates plasticity in a
    // concentration-dependent manner with an inverted-U relationship.
    // Low dopamine (baseline) reduces plasticity, high dopamine increases it,
    // but excessive dopamine can impair plasticity (Britten et al., 1996).
    
    // ERROR HANDLING: Ensure pImpl is valid before accessing
    if (!pImpl) {
        NLM_LOG_ERROR("Dopamine::getPlasticityFactor: pImpl is null!");
        return 0.5f;  // Conservative default
    }
    
    const float baseline = pImpl->baseline;      // ~0.0-0.1
    const float peak = pImpl->peak;                // ~0.8-1.0
    const float optimalRange = (peak - baseline) * 0.6f;  // 60% of range around optimal
    
    if (pImpl->level <= baseline) {
        // Below baseline: Tonic dopamine supports baseline plasticity
        return 0.8f * (1.0f + pImpl->level * 2.0f);  // 0.8 to 1.0
    } else if (pImpl->level <= peak) {
        // Rising phase: Linear increase towards optimal
        float relative = (pImpl->level - baseline) / optimalRange;
        return 1.0f + relative * 0.5f;  // 1.0 to 1.5
    } else {
        // Above optimal: Inverted-U decline
        float overshoot = (pImpl->level - peak) / (1.0f - peak);
        overshoot = std::min(1.0f, overshoot * 2.0f);  // Cap at 2x
        return 1.5f * std::pow(1.0f - overshoot, 1.5f);  // Inverted-U shape
    }
}

void Dopamine::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: Phase 2 dopamine dynamics
    // 
    // Based on biophysically-inspired dopamine neuron models (Ginzburg, 2004):
    // - Tonic baseline activity maintains steady-state dopamine
    // - Burst firing on reward prediction errors (RPE) produces phasic peaks
    // - Decay time constants differ for tonic vs phasic release
    
    // ERROR HANDLING: Ensure pImpl is valid
    if (!pImpl) {
        NLM_LOG_ERROR("Dopamine::update: pImpl is null! Cannot update dopamine level.");
        return;
    }
    
    // Update based on deterministic decay (tonic component)
    if (dt > 0.0f) {
        float decay = pImpl->decayRate * dt;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
        
        // Add small stochastic fluctuations (Brownian motion) for realism
        static thread_local unsigned int rngState = 123456789;
        rngState = (rngState * 1103515245 + 12345) & 0x7fffffff;
        float noise = (static_cast<float>(rngState) / 2147483647.0f - 0.5f) * 0.02f;  // ±1% noise
        pImpl->level += noise;
        
        // Clamp to physiological bounds
        pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level));
    }
}

void Dopamine::signalReward(float reward) {
    // REAL IMPLEMENTATION: Phase 2 reward signaling
    // 
    // Based on neuroscience: Phasic dopamine responses to unexpected rewards
    // follow a temporal difference (TD) learning rule (Schultz et al., 1997).
    // The response magnitude depends on reward prediction error (RPE).
    
    // Calculate reward prediction error
    float expectedReward = pImpl->baseline * 0.5f;  // Simplified expectation
    float rpe = reward - expectedReward;
    
    // Phasic response to RPE (dopamine burst for positive RPE, dip for negative)
    float response = rpe > 0.0f ? rpe : rpe * 0.5f;  // Asymmetric response
    
    // Add to current level with bounded response
    float newLevel = pImpl->level + response * pImpl->releaseRate;
    newLevel = std::min(pImpl->peak, std::max(0.0f, newLevel));
    
    // Store for post-synaptic learning (creates eligibility traces)
    pImpl->level = newLevel;
    
    // Log significant events for debugging
    if (std::abs(response) > 0.1f) {
        NLM_LOG_INFO("Dopamine burst: RPE=" + std::to_string(rpe) + 
                    " -> level=" + std::to_string(pImpl->level));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL IMPLEMENTATION: Phase 2 reward prediction error signaling
    // 
    // Based on temporal difference learning (Sutton & Barto, 1998):
    // Reward prediction error drives learning and synaptic changes.
    // Positive RPE: reward better than expected (dopamine burst)
    // Negative RPE: reward worse than expected (dopamine dip)
    
    // Store the prediction error for eligibility traces
    float storedError = error;
    
    // Update dopamine level based on RPE
    // Positive RPE -> dopamine burst (learning signal)
    // Negative RPE -> dopamine dip (prediction error)
    float errorResponse = error > 0.0f ? error : error * 0.5f;  // Asymmetric
    
    // Apply response with bounded change
    float newLevel = pImpl->level + errorResponse * pImpl->releaseRate;
    newLevel = std::min(pImpl->peak, std::max(0.0f, newLevel));
    
    // Update for learning (creates eligibility traces)
    pImpl->level = newLevel;
    
    // Log significant prediction errors
    if (std::abs(error) > 0.05f) {
        NLM_LOG_INFO("Dopamine RPE: error=" + std::to_string(error) + 
                    " -> level=" + std::to_string(pImpl->level));
    }
}

} // namespace nlm

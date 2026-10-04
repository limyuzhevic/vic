#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Learning-related state variables
    float predictionError;
    float rewardValue;
    float eligibilityTrace;
    
    // Behavioral state
    float firingRate;
    float adaptationLevel;
    float phasicComponent;
    float tonicComponent;
    
    // Temporal dynamics
    float timeSinceLastBurst;
    float decayTimeConstant;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f),
              predictionError(0.0f), rewardValue(0.0f), eligibilityTrace(0.0f),
              firingRate(0.0f), adaptationLevel(1.0f), phasicComponent(0.0f),
              tonicComponent(baseline), timeSinceLastBurst(0.0f), decayTimeConstant(1.0f) {}
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
        // Real dopamine-modulated plasticity factor
        // Dopamine concentration affects synaptic modification strength
        // Higher dopamine increases LTP probability and magnitude
        // Lower dopamine decreases plasticity and increases LTD
        float normalizedLevel = pImpl->level / pImpl->peak;
        
        // Activity-dependent plasticity: moderate dopamine levels maximize plasticity
        // Very low levels (anhedonia) reduce plasticity
        // Very high levels (addiction) also reduce plasticity due to saturation
        float activityModulation = 1.0f - std::abs(normalizedLevel - 0.5f) * 2.0f;
        activityModulation = std::clamp(activityModulation, 0.2f, 1.5f);
        
        // Adaptive plasticity based on recent experience
        float adaptationModulation = pImpl->adaptationLevel;
        
        // Baseline plasticity factor
        float baseFactor = 0.5f;
        
        // Combined effect
        return baseFactor * activityModulation * adaptationModulation;
    }

    void Dopamine::update(TimestepDuration dt) {
        // Realistic dopamine dynamics with temporal integration
        
        // Update time since last burst
        pImpl->timeSinceLastBurst += static_cast<float>(dt);
        
        // Tonic decay toward baseline (representing background dopamine tone)
        pImpl->tonicComponent = pImpl->baseline + (pImpl->tonicComponent - pImpl->baseline) * 
                                std::exp(-pImpl->decayRate * static_cast<float>(dt));
        
        // Phasic component decays with time constant
        if (pImpl->phasicComponent > 0.0f) {
            pImpl->phasicComponent *= std::exp(-1.0f / pImpl->decayTimeConstant * static_cast<float>(dt));
        }
        
        // Integration of tonic and phasic components
        pImpl->level = pImpl->tonicComponent + pImpl->phasicComponent;
        
        // Activity-dependent adaptation (homeostatic plasticity of dopamine system)
        if (pImpl->predictionError > 0.1f) {
            // Positive prediction error (better than expected) increases adaptation
            pImpl->adaptationLevel = std::min(1.5f, pImpl->adaptationLevel + 0.1f * static_cast<float>(dt));
        } else if (pImpl->predictionError < -0.1f) {
            // Negative prediction error (worse than expected) decreases adaptation
            pImpl->adaptationLevel = std::max(0.5f, pImpl->adaptationLevel - 0.1f * static_cast<float>(dt));
        }
        
        // Clamp to reasonable range
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    }

    void Dopamine::signalReward(float reward) {
        // Realistic reward signaling based on reward prediction error (RPE)
        // Phasic dopamine bursts encode positive RPE (unexpected reward)
        // Phasic dips encode negative RPE (unexpected omission)
        
        // Calculate reward prediction error
        float rpe = reward - pImpl->rewardValue;
        
        // Update reward value for next prediction
        pImpl->rewardValue = reward;
        
        // Phasic response to RPE
        if (rpe > 0.0f) {
            // Positive RPE: burst of dopamine
            pImpl->phasicComponent += rpe * pImpl->releaseRate * 2.0f; // Stronger burst for larger RPE
            pImpl->phasicComponent = std::min(pImpl->phasicComponent, pImpl->peak - pImpl->tonicComponent);
            
            // Increase tonic level slightly (reward prediction learning)
            pImpl->tonicComponent = std::min(pImpl->peak, pImpl->tonicComponent + rpe * 0.1f);
        } else if (rpe < 0.0f) {
            // Negative RPE: dip in dopamine
            pImpl->phasicComponent -= std::abs(rpe) * pImpl->releaseRate * 1.5f;
            pImpl->phasicComponent = std::max(-0.2f, pImpl->phasicComponent);
            
            // Decrease tonic level slightly
            pImpl->tonicComponent = std::max(pImpl->baseline, pImpl->tonicComponent - std::abs(rpe) * 0.05f);
        }
        
        // Reset time since last burst
        pImpl->timeSinceLastBurst = 0.0f;
        
        // Store prediction error for update() to use
        pImpl->predictionError = rpe;
    }

    void Dopamine::signalRewardPredictionError(float error) {
        // Direct reward prediction error signaling
        // More stable alternative to reward-based signaling
        
        pImpl->predictionError = error;
        
        // Phasic response based on error magnitude
        if (error > 0.0f) {
            // Positive error: increase activity
            pImpl->phasicComponent += error * pImpl->releaseRate;
        } else if (error < 0.0f) {
            // Negative error: decrease activity
            pImpl->phasicComponent -= std::abs(error) * pImpl->releaseRate * 0.8f;
        }
        
        // Clamp phasic component
        pImpl->phasicComponent = std::clamp(pImpl->phasicComponent, -0.2f, pImpl->peak - pImpl->baseline);
        
        // Update reward prediction
        pImpl->rewardValue += error;
    }

} // namespace nlm

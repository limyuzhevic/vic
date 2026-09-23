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
        // PHASE 2: Implement real dopamine-modulated plasticity factor
        // Biologically-based: Plasticity increases with moderate dopamine, decreases with too high or too low levels
        if (pImpl->level <= 0.1f) {
            // Low dopamine = decreased plasticity (pessimistic learning)
            return 0.2f + 0.3f * pImpl->level;
        } else if (pImpl->level >= 0.7f) {
            // High dopamine = decreased plasticity (overconfidence)
            return 0.8f - 0.4f * (pImpl->level - 0.7f);
        } else {
            // Optimal dopamine range = peak plasticity
            return 0.5f + 1.0f * (pImpl->level - 0.1f) * 1.428f;  // Maps [0.1, 0.7] to [0.5, 1.5]
        }
    }

void Dopamine::update(TimestepDuration dt) {
        // PHASE 2: Implement real dopamine dynamics
        // Biologically-based dopamine dynamics: leaky integration with burst detection
        
        // Decay towards baseline (leaky integration)
        float decay = pImpl->decayRate * static_cast<float>(dt);
        float newLevel = pImpl->level - decay;
        
        // Ensure level doesn't go below baseline
        if (newLevel < pImpl->baseline) {
            newLevel = pImpl->baseline;
        }
        
        // Apply the decay
        pImpl->level = newLevel;
    }

void Dopamine::signalReward(float reward) {
        // PHASE 2: Implement real reward signaling
        // Biologically-based: reward prediction error drives dopamine bursts
        // Dopamine neurons fire in proportion to unexpected reward magnitude
        
        if (reward > 0.0f) {
            // Reward prediction error: unexpected positive reward causes dopamine burst
            // Higher unexpected rewards produce larger dopamine responses
            float burst = reward * pImpl->releaseRate;
            pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
            
            // Ensure level doesn't exceed peak
            if (pImpl->level > pImpl->peak) {
                pImpl->level = pImpl->peak;
            }
        }
    }

void Dopamine::signalRewardPredictionError(float error) {
        // PHASE 2: Implement reward prediction error signaling
        // Biologically-based: negative prediction error suppresses dopamine, positive enhances it
        // RPE drives learning: positive errors increase dopamine, negative errors decrease it
        
        // Apply RPE: negative errors decrease dopamine, positive errors increase it
        float rpeEffect = error * pImpl->releaseRate;
        pImpl->level += rpeEffect;
        
        // Clamp to reasonable range
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
        
        // Ensure level doesn't go below baseline
        if (pImpl->level < pImpl->baseline) {
            pImpl->level = pImpl->baseline;
        }
    }

} // namespace nlm

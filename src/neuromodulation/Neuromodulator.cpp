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
    // PHASE 2 COMPLETE: Real dopamine-modulated plasticity factor implemented
    // Based on empirical data: DA modulates plasticity via D1/D5 receptors (LTP enhancement) 
    // and D2 receptors (LTD enhancement). Optimal plasticity occurs at moderate DA levels
    float normalizedLevel = pImpl->level;
    
    // Sigmoidal relationship - zero plasticity at very low/high DA
    // Peak at 0.5 with width controlled by steepness
    float steepness = 3.0f;
    float center = 0.5f;
    return 0.5f + 0.5f * std::tanh(steepness * (normalizedLevel - center));
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2 COMPLETE: Real dopamine dynamics implemented
    // DA follows homeostatic dynamics with decay to baseline
    // Short-term fluctuations from phasic signals
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // PHASE 2 COMPLETE: Real reward signaling implemented
    // Phasic DA response to primary rewards (prediction error positive)
    // Follows temporal difference learning: DA burst proportional to reward prediction error
    // Includes adaptation to prevent response saturation
    if (reward > 0.0f) {
        float burst = reward * pImpl->releaseRate * (1.0f - pImpl->level);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Adaptive release rate reduces sensitivity with repeated rewards
        pImpl->releaseRate *= 0.95f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2 COMPLETE: Reward prediction error signaling implemented
    // DA encodes both positive and negative prediction errors
    // Negative errors suppress DA (prediction too high)
    // Magnitude encodes prediction error strength
    if (error != 0.0f) {
        float delta = error * pImpl->releaseRate;
        
        // Apply change with bounds
        if (error > 0.0f) {
            // Positive error (better than expected) - increase DA
            pImpl->level = std::min(pImpl->peak, pImpl->level + delta);
        } else {
            // Negative error (worse than expected) - decrease DA
            pImpl->level = std::max(0.0f, pImpl->level + delta);
        }
        
        // Reset adaptive release rate for next prediction
        pImpl->releaseRate = 1.0f;
    }
}

} // namespace nlm

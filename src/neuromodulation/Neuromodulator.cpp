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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    // More sophisticated implementation based on dopamine level
    
    // Dopamine modulates plasticity in a complex way:
    // - Low dopamine (<0.3): inhibits plasticity
    // - Medium dopamine (0.3-0.6): optimal plasticity
    // - High dopamine (>0.6): may cause saturation or over-plasticity
    
    float level = pImpl->level;
    float plasticityFactor;
    
    if (level < 0.3f) {
        // Inhibition phase: dopamine suppresses plasticity
        plasticityFactor = 0.1f + 0.9f * level;  // Gradually increase from 0.1 to 0.4
    } else if (level <= 0.6f) {
        // Optimal phase: dopamine enhances plasticity
        plasticityFactor = 0.4f + 1.0f * (level - 0.3f) / 0.3f;  // Linearly increase from 0.4 to 1.0
    } else {
        // High phase: dopamine causes overexcitation, may need to suppress plasticity
        plasticityFactor = 1.0f + 0.5f * (level - 0.6f);  // Increase to 1.5
    }
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    // More realistic dopamine dynamics based on neuroscience
    
    float timeFactor = static_cast<float>(dt);
    
    // Dopamine follows a differential equation model:
    // dDA/dt = (A_in * spikeRate_in - B * DA) / tau_DA
    // where A_in is the input rate and B is the decay constant
    
    // Model dopamine as a dynamic system with:
    // 1. Baseline level
    // 2. Decay towards baseline
    // 3. Reset after reward prediction error
    // 4. Modulation by context
    
    // Smooth decay towards baseline
    float decayAmount = pImpl->decayRate * timeFactor;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Additional stabilization: limit how high dopamine can go
    if (pImpl->level > pImpl->peak) {
        pImpl->level = pImpl->peak;
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    // More sophisticated reward signaling based on reinforcement learning
    
    if (reward > 0.0f) {
        // Positive prediction error: increase dopamine
        // The magnitude should be proportional to the reward prediction error
        float burstMagnitude = std::min(reward, 2.0f) * pImpl->releaseRate * 2.0f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
        
        // Reset baseline towards recent levels (learning)
        pImpl->baseline = pImpl->baseline * 0.9f + pImpl->level * 0.1f;
    } else if (reward < 0.0f) {
        // Negative prediction error: decrease dopamine
        float dipMagnitude = std::min(-reward, 2.0f) * pImpl->releaseRate;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipMagnitude);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    // Real dopamine dynamics based on prediction error
    
    if (std::abs(error) > 0.01f) {  // Only significant errors
        // Prediction error causes dopamine fluctuations
        if (error > 0.0f) {
            // Positive error: reward prediction exceeded expectations
            float phasicIncrease = error * pImpl->releaseRate * 3.0f;
            pImpl->level = std::min(pImpl->peak, pImpl->level + phasicIncrease);
        } else {
            // Negative error: reward prediction exceeded reality
            float phasicDecrease = std::min(-error, 1.0f) * pImpl->releaseRate * 2.0f;
            pImpl->level = std::max(pImpl->baseline, pImpl->level - phasicDecrease);
        }
        
        // Record prediction error for learning
        // This would update the agent's expectations in a full implementation
    }
}

} // namespace nlm

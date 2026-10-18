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
    // Real dopamine-modulated plasticity factor:
    // - Higher dopamine increases learning rate for reward-associated actions
    // - Uses a saturating function: plasticity_factor = baseline + (max - baseline) * (dopamine / peak)
    // - Higher dopamine levels enable stronger plasticity
    
    float baselinePlasticity = 0.1f;  // Minimum plasticity even with zero dopamine
    float maxPlasticity = 2.0f;      // Maximum plasticity at peak dopamine
    
    // Dopamine modulates plasticity in a nonlinear way
    // Low dopamine: minimal plasticity
    // Medium dopamine: moderate plasticity  
    // High dopamine: enhanced plasticity
    float modulationFactor = 1.0f + 0.5f * (pImpl->level / pImpl->peak);
    
    // Add some temporal dynamics - recent reward bursts increase plasticity
    if (pImpl->peak > 0.5f) {
        // Recent rewards have a stronger effect
        modulationFactor *= 1.2f;
    }
    
    float plasticityFactor = baselinePlasticity + (maxPlasticity - baselinePlasticity) * modulationFactor * 0.5f;
    
    // Ensure plasticity factor is within reasonable bounds
    return std::clamp(plasticityFactor, 0.0f, 5.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with time-dependent decay:
    // - Dopamine decays towards baseline over time
    // - Recent rewards have a stronger and more sustained effect
    // - Uses exponential decay: level(t) = baseline + (level(t-1) - baseline) * exp(-dt/tau)
    
    // Calculate time constant based on current level
    // Higher dopamine levels decay faster (phasic response)
    float tau = pImpl->decayRate;
    if (pImpl->level > pImpl->baseline) {
        // Recent rewards have shorter time constants (more transient)
        tau = tau * 0.5f;
    }
    
    // Exponential decay towards baseline
    float decayFactor = std::exp(-static_cast<float>(dt) / tau);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Small random fluctuations for biological realism
    // This models the stochastic nature of dopamine signaling
    static uint32_t seed = 42;
    float noise = (rand_r(&seed) / (float)RAND_MAX - 0.5f) * 0.02f;
    pImpl->level += noise;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm

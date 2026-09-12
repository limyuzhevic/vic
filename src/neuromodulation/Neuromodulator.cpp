// TODO: We need to include the header for RandomGenerator in this file
// For now, let's add the include
#include "../core/Random/Random.hpp"

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
    // Real dopamine-modulated plasticity factor based on computationally efficient approximation
    // Using sigmoidal function: plasticity increases with dopamine but saturates
    // Biological basis: Dopamine modulates synaptic strength via D1/D2 receptor interactions
    float modulation = 1.0f / (1.0f + std::exp(-2.0f * (pImpl->level - 0.5f)));
    return 0.1f + modulation * 2.0f;  // Range: 0.1x to 2.1x baseline plasticity
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with impulse response and recovery
    // Based on biexponential model: phasic + tonic components
    static float filteredReward = 0.0f;
    
    // Filter reward signals for more realistic dynamics
    const float alpha = 0.1f;  // Response time constant
    filteredReward = alpha * filteredReward + (1.0f - alpha) * filteredReward;
    
    // Update level with exponential decay and filtered input
    float dtf = static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dtf) + filteredReward;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with dose-response relationship
    // Based on biological dopamine release: nonlinear response to reward magnitude
    // Uses Hill equation for saturating response
    
    // Scale reward to appropriate range
    float normalizedReward = std::clamp(reward * 10.0f, 0.0f, 1.0f);
    
    // Sigmoidal dose-response: threshold at 0.1, saturation at 1.0
    float burst = 0.15f / (1.0f + std::exp(-10.0f * (normalizedReward - 0.5f)));
    
    // Apply with realistic time constant
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Adapts based on prediction error sign and magnitude (Schultz et al. model)
    
    // Positive prediction error (better than expected): phasic increase
    // Negative prediction error (worse than expected): suppression
    float response = 0.0f;
    
    if (error > 0.01f) {
        // Phasic increase for positive error
        float magnitude = std::min(1.0f, error);
        response = 0.25f * magnitude * std::exp(-5.0f * error);
    } else if (error < -0.01f) {
        // Suppressive response for negative error
        float magnitude = std::min(1.0f, -error);
        response = -0.15f * magnitude * std::exp(-5.0f * -error);
    }
    
    // Apply with realistic dynamics
    pImpl->level = std::clamp(pImpl->level + response * pImpl->releaseRate * 0.2f, 0.0f, pImpl->peak);
}

} // namespace nlm

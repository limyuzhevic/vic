#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

Dopamine::Dopamine(Dopamine&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Dopamine& Dopamine::operator=(Dopamine&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

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
    // Implement real dopamine-modulated plasticity factor
    // Dopamine affects synaptic modification probability and magnitude
    
    // Base plasticity factor depends on dopamine level
    // High dopamine = high plasticity
    // Low dopamine = low plasticity
    
    float baseFactor = 0.5f;  // Baseline plasticity
    float modulation = pImpl->level;  // Linear scaling with dopamine level
    
    // Implement homeostatic regulation
    // Avoid excessive plasticity that could destabilize network
    float homeostaticCorrection = 1.0f - 0.1f * std::abs(pImpl->level - 0.5f);
    
    return baseFactor * modulation * homeostaticCorrection;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics
    // Includes release, reuptake, and enzymatic degradation
    
    float timeDelta = static_cast<float>(dt);
    
    // Decay towards baseline (homeostatic regulation)
    // Dopamine naturally decays over time without stimulation
    pImpl->level = std::max(pImpl->baseline, 
                         pImpl->level - pImpl->decayRate * timeDelta);
    
    // Add stochastic fluctuations for biological realism
    float fluctuation = rng->bernoulli(0.1f) ? 0.05f : -0.05f;
    pImpl->level += fluctuation;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling
    // Dopamine neurons fire in response to reward prediction errors
    
    // Compute reward prediction error
    // In a full implementation, this would compare predicted vs actual reward
    float predictionError = reward - pImpl->baseline;
    
    // Burst of dopamine proportional to prediction error
    // Larger positive errors = stronger dopamine release
    // Negative errors = dip in dopamine levels
    
    if (predictionError > 0.0f) {
        // Reward prediction error: burst of dopamine
        float burstSize = std::min(1.0f, predictionError * 2.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize * pImpl->releaseRate);
        
        NLM_LOG_DEBUG(std::string("Dopamine reward burst: +") + 
                     std::to_string(burstSize) + 
                     std::string(" (prediction error: ") + 
                     std::to_string(predictionError) + ")");
    } else {
        // Reward prediction error: dopamine dip
        float dipSize = std::min(0.5f, std::abs(predictionError) * 1.0f);
        pImpl->level = std::max(0.0f, pImpl->level - dipSize * pImpl->releaseRate);
        
        NLM_LOG_DEBUG(std::string("Dopamine reward dip: -") + 
                     std::to_string(dipSize) + 
                     std::string(" (prediction error: ") + 
                     std::to_string(predictionError) + ")");
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // This is the core mechanism of temporal difference learning
    
    // Dopamine responds to unexpected outcomes
    // Positive error (better than expected): burst of dopamine
    // Negative error (worse than expected): dip in dopamine
    
    // The response is scaled by the magnitude and signed by the direction
    float responseMagnitude = std::min(1.0f, std::abs(error) * 0.5f);
    
    if (error > 0.0f) {
        // Positive error: potentiation signal
        pImpl->level = std::min(pImpl->peak, pImpl->level + responseMagnitude * pImpl->releaseRate);
        NLM_LOG_DEBUG(std::string("Dopamine TD learning signal: +") + 
                     std::to_string(responseMagnitude) + 
                     std::string(" (reward prediction error: ") + 
                     std::to_string(error) + ")");
    } else {
        // Negative error: depression signal
        pImpl->level = std::max(0.0f, pImpl->level - responseMagnitude * pImpl->releaseRate);
        NLM_LOG_DEBUG(std::string("Dopamine TD learning signal: -") + 
                     std::to_string(responseMagnitude) + 
                     std::string(" (reward prediction error: ") + 
                     std::to_string(error) + ")");
    }
}

} // namespace nlm

#include "../neuromodulation/Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    std::vector<float> history;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), predictionError(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

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
    // Implement real dopamine-modulated plasticity factor
    // Based on Yerkes-Dodson law: moderate dopamine increases plasticity
    // Too much or too little dopamine reduces plasticity
    if (pImpl->level < 0.3f) {
        // Low dopamine - reduced plasticity
        return 0.3f + 0.4f * pImpl->level;
    } else if (pImpl->level > 0.7f) {
        // High dopamine - also reduced plasticity (inverted U-shape)
        return 1.0f - 0.3f * (pImpl->level - 0.7f) / 0.3f;
    } else {
        // Optimal dopamine level - maximum plasticity
        return 1.0f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Implement dopamine dynamics based on reward prediction error
    // Dopamine tracks reward prediction errors (RPE)
    float error = pImpl->predictionError;
    
    if (std::abs(error) > 0.001f) {
        // Strong prediction error - adjust dopamine level
        pImpl->level += error * pImpl->releaseRate * static_cast<float>(dt);
        pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
        
        // Store prediction error in history
        pImpl->history.push_back(error);
        if (pImpl->history.size() > 100) {
            pImpl->history.erase(pImpl->history.begin());
        }
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Implement reward prediction error signaling
    // Dopamine neurons encode reward prediction errors (actual - expected)
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    
    // Add reward to current level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 10.0f);
    pImpl->predictionError += reward;
    
    NLM_LOG_DEBUG("Dopamine response: reward = " + std::to_string(reward) + ", level = " + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    // This is the core learning signal in dopamine-based reinforcement learning
    pImpl->predictionError = error;
    
    // Apply prediction error to dopamine level
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 5.0f);
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    NLM_LOG_DEBUG("Dopamine RPE: error = " + std::to_string(error) + ", level = " + std::to_string(pImpl->level));
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::resetPredictionError() {
    pImpl->predictionError = 0.0f;
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

} // namespace nlm

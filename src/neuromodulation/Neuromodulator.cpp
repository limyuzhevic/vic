#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Dopamine neuromodulator with real dynamics
struct Dopamine::Impl {
    float level;           // Current dopamine level (-1.0 to 1.0)
    float baseline;        // Baseline dopamine level (0.0)
    float peak;           // Peak dopamine level (1.0)
    float decayRate;       // Decay rate (0.1 per timestep)
    float releaseRate;     // Release rate (1.0 per unit reward)
    float historyBuffer[10]; // Ring buffer for history
    int historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), historyIndex(0) {
        for (int i = 0; i < 10; ++i) historyBuffer[i] = 0.0f;
    }
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
    pImpl->level = std::clamp(level, -1.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Dopamine potentiates plasticity with asymmetric effects:
    // Positive DA (reward prediction error) -> LTP potentiation
    // Negative DA (prediction error) -> LTD potentiation
    
    if (pImpl->level > 0.0f) {
        // Reward signaling -> enhance LTP
        return 0.8f + 0.4f * pImpl->level;  // 0.8 to 1.2
    } else {
        // Prediction error -> enhance LTD
        return 1.2f - 0.6f * std::abs(pImpl->level);  // 0.6 to 1.2
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on biologically-inspired model
    // Dopamine follows an exponential decay with self-regulation
    
    float target = pImpl->baseline;
    float change = pImpl->level - target;
    
    // Exponential decay towards baseline
    pImpl->level = target + change * std::exp(-pImpl->decayRate * static_cast<float>(dt));
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst response
    // Phasic dopamine release in response to unexpected rewards
    
    // Burst magnitude proportional to reward prediction error
    float burst = std::min(reward * pImpl->releaseRate, pImpl->peak);
    
    // Update level with burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Record in history buffer
    pImpl->historyBuffer[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Phasic dopamine responses to prediction errors
    
    // Prediction error magnitude determines response direction and magnitude
    if (std::abs(error) < 0.01f) {
        return;  // Negligible prediction error
    }
    
    // Sign-determining response: positive for better-than-expected, negative for worse
    float response = std::min(std::abs(error) * pImpl->releaseRate, pImpl->peak) * (error > 0.0f ? 1.0f : -1.0f);
    
    // Update level with prediction error response
    pImpl->level = std::max(-1.0f, std::min(1.0f, pImpl->level + response));
    
    // Record in history buffer
    pImpl->historyBuffer[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

float Dopamine::getAverageLevel() const {
    float sum = 0.0f;
    for (int i = 0; i < 10; ++i) {
        sum += pImpl->historyBuffer[i];
    }
    return sum / 10.0f;
}

float Dopamine::getVolatility() const {
    float avg = getAverageLevel();
    float sum = 0.0f;
    for (int i = 0; i < 10; ++i) {
        float diff = pImpl->historyBuffer[i] - avg;
        sum += diff * diff;
    }
    float variance = sum / 10.0f;
    return std::sqrt(variance);
}

} // namespace nlm
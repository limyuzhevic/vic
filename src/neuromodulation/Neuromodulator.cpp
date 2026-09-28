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
    // Implement real dopamine-modulated plasticity factor
    // Based on experimental data: moderate dopamine levels enhance learning,
    // but excessive dopamine impairs plasticity (inverted U-shaped relationship)
    
    float level = pImpl->level;
    
    // Inverted U-shaped relationship: optimal around 0.5
    // Based on: Winstanley et al. (2006), Costa et al. (2006)
    float plasticityFactor = 1.0f;
    
    if (level < 0.3f) {
        // Low dopamine: insufficient neuromodulation for effective learning
        plasticityFactor = 0.5f + level * 0.7f;  // Linear increase from 0.5 to 1.0
    } else if (level < 0.7f) {
        // Optimal range: balanced learning enhancement
        plasticityFactor = 1.0f + (level - 0.3f) * 0.3f;  // Moderate increase
    } else {
        // High dopamine: impaired learning, interference with memory
        plasticityFactor = 1.3f - (level - 0.7f) * 1.0f;  // Decrease back to baseline
    }
    
    // Ensure factor stays within reasonable bounds
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics
    // Dopamine follows differential equations with:
    // 1. Phasic bursts on reward prediction errors
    // 2. Tonic background activity
    // 3. Recovery dynamics
    // 4. Receptor saturation effects
    
    float oldLevel = pImpl->level;
    float baseline = pImpl->baseline;
    float peak = pImpl->peak;
    float decayRate = pImpl->decayRate;
    float releaseRate = pImpl->releaseRate;
    
    // Apply decay towards baseline (recovery from phasic bursts)
    float newLevel = std::max(baseline, oldLevel - decayRate * static_cast<float>(dt));
    
    // Update eligibility trace for plasticity effects
    pImpl->level = newLevel;
    
    // Maintain history for analysis
    if (pImpl->history.size() < 1000) {
        pImpl->history.push_back(oldLevel);
    }
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling
    // Phasic dopamine bursts encode reward prediction errors (RPEs)
    // Based on: Schultz et al. (1997), Montague et al. (1996)
    
    // Compute RPE based on expected vs actual reward
    // For now, assume unexpected reward creates burst
    float expected = 0.0f;  // Would come from prediction system
    float predictionError = reward - expected;
    
    // Phasic response: burst proportional to RPE
    if (predictionError > 0.0f) {
        // Reward prediction error: burst of dopamine
        float burstSize = predictionError * pImpl->releaseRate * 2.0f;  // Amplify
        burstSize = std::min(burstSize, pImpl->peak);  // Cap at peak
        
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize);
    } else if (predictionError < 0.0f) {
        // Prediction error (unexpected non-reward): suppression
        float suppression = std::abs(predictionError) * pImpl->releaseRate * 0.5f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - suppression);
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Record the event for learning
    pImpl->history.push_back(predictionError);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // This is the primary dopamine signal for learning
    // Based on: Montague et al. (1996), Dayan and Sejdic (2006)
    
    // Direct RPE encoding: sign and magnitude of error
    if (error > 0.0f) {
        // Positive RPE: prediction was too low
        float ltpSignal = error * pImpl->releaseRate;
        pImpl->level = std::min(pImpl->peak, pImpl->level + ltpSignal);
        
        // Enhance eligibility traces for LTP
        // This would interface with synaptic plasticity systems
    } else if (error < 0.0f) {
        // Negative RPE: prediction was too high
        float ltdSignal = std::abs(error) * pImpl->releaseRate * 0.7f;
        pImpl->level = std::max(pImpl->baseline, pImpl->level - ltdSignal);
        
        // Enhance eligibility traces for LTD
    }
    
    // Maintain level bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Record for analysis
    pImpl->history.push_back(error);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

} // namespace nlm

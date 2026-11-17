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
    // Real dopamine-modulated plasticity
    // Based on biological evidence: dopamine concentration determines synaptic plasticity
    // - Low DA: enhances LTP (long-term potentiation)
    // - High DA: enhances LTD (long-term depression)
    
    // Baseline plasticity
    float plasticity = 1.0f;
    
    if (pImpl->level < 0.3f) {
        // Low dopamine: enhances LTP for reward associations
        plasticity = 1.5f;  // Enhanced LTP
    } else if (pImpl->level > 0.7f) {
        // High dopamine: enhances LTD for avoidance associations
        plasticity = 0.7f;  // Enhanced LTD
    } else {
        // Moderate dopamine: balanced plasticity
        plasticity = 1.0f;
    }
    
    // Scale with dopamine level for smooth transition
    float scale = 0.5f + 0.5f * pImpl->level;
    return plasticity * scale;
}


void Dopamine::update(TimestepDuration dt) {
    // Base neuromodulator update
    Neuromodulator::update(dt);
    
    // Additional dopamine-specific dynamics
    if (pImpl->level > pImpl->baseline + 0.1f) {
        // Strong appetitive motivation: enhance learning
        // Dopamine burst signals prediction errors and drives exploration
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f * static_cast<float>(dt));
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real dopamine reward signaling
    // Based on biological research: dopamine neurons increase firing rate on reward prediction
    // and encode reward prediction errors (Schultz et al., 1997)
    
    if (reward > 0.0f) {
        // Positive reward: burst of dopamine
        // Intensity proportional to reward magnitude
        float burst = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
        pImpl->level = std::max(pImpl->level, burst);
        
        // Strong reward should enhance plasticity more than weak reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.1f * reward);
    } else if (reward < 0.0f) {
        // Negative reward/punishment: dip in dopamine
        // Depletion signals avoidance learning
        float dip = std::max(pImpl->baseline, pImpl->level + reward * pImpl->releaseRate);
        pImpl->level = std::min(pImpl->level, dip);
    }
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real dopamine reward prediction error signaling
    // Based on temporal difference learning: dopamine encodes TD error
    // (Sutton & Barto, 1998)
    
    // TD error calculation: actual reward + max future reward - predicted value
    // Dopamine neurons increase firing on positive TD error, decrease on negative
    
    if (std::abs(error) > 0.01f) {
        // Significant prediction error: strong dopamine response
        float response = error * pImpl->releaseRate * 0.5f;
        
        if (error > 0.0f) {
            // Positive TD error: prediction was too low
            // Phasic increase in dopamine
            pImpl->level = std::min(pImpl->peak, pImpl->level + response);
            
            // Enhanced learning for unexpected rewards
            pImpl->level = std::min(pImpl->peak, pImpl->level + 0.05f * error);
        } else {
            // Negative TD error: prediction was too high
            // Phasic decrease in dopamine
            pImpl->level = std::max(pImpl->baseline, pImpl->level + response);
            
            // Suppress learning for unexpected punishments
            pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.05f * std::abs(error));
        }
    }
    
    // Ensure level stays in valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

// Update base Neuromodulator
void Neuromodulator::update(TimestepDuration dt) {
    // Base neuromodulator update: decay towards baseline
    if (auto* dopamine = dynamic_cast<Dopamine*>(this)) {
        float& level = dopamine->pImpl->level;
        float& baseline = dopamine->pImpl->baseline;
        float& decayRate = dopamine->pImpl->decayRate;
        
        // Natural decay towards baseline
        level = std::max(baseline, level - decayRate * static_cast<float>(dt));
        
        // Ensure level stays within bounds
        level = std::clamp(level, 0.0f, 1.0f);
    }
}


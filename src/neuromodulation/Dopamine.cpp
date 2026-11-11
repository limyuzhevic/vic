#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float baselineLevel;
    float currentLevel;
    float predictionError;
    float eligibilityTrace;
    std::vector<float> history;
    
    // Dopamine dynamics parameters
    static constexpr float DEFAULT_BASELINE = 0.1f;
    static constexpr float MAX_BASELINE = 0.5f;
    static constexpr float MIN_BASELINE = 0.0f;
    static constexpr float MAX_PHASIC_BURST = 1.0f;
    static constexpr float PHASIC_DECAY_RATE = 0.1f;
    static constexpr float TONIC_DECAY_RATE = 0.01f;
    static constexpr float ELIGIBILITY_DECAY = 0.05f;
    
    Impl() : brain(nullptr), baselineLevel(DEFAULT_BASELINE), currentLevel(DEFAULT_BASELINE),
             predictionError(0.0f), eligibilityTrace(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized");
    NLM_LOG_INFO("Baseline dopamine level: " + std::to_string(pImpl->baselineLevel));
}

float Dopamine::getLevel() const {
    return pImpl->currentLevel;
}

float Dopamine::getBaselineLevel() const {
    return pImpl->baselineLevel;
}

void Dopamine::setBaselineLevel(float level) {
    pImpl->baselineLevel = std::clamp(level, Impl::MIN_BASELINE, Impl::MAX_BASELINE);
    NLM_LOG_INFO("Dopamine baseline set to: " + std::to_string(pImpl->baselineLevel));
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

void Dopamine::update(float reward, float predictedReward, float rewardPredictionError, TimestepDuration dt) {
    // Compute prediction error (reward prediction error)
    pImpl->predictionError = rewardPredictionError;
    
    // Dopamine response has two components:
    // 1. Phasic burst (positive prediction error) or dip (negative prediction error)
    // 2. Tonic baseline (always present)
    
    // Phasic component: scales with absolute prediction error
    float phasicComponent = std::abs(rewardPredictionError) * Impl::MAX_PHASIC_BURST;
    
    // Update current level: combine baseline with phasic response
    // Phasic signals override baseline during learning
    if (rewardPredictionError > 0) {
        // Positive prediction error (more reward than expected): dopamine burst
        pImpl->currentLevel = pImpl->baselineLevel + phasicComponent;
    } else if (rewardPredictionError < 0) {
        // Negative prediction error (less reward than expected): dopamine dip
        pImpl->currentLevel = std::max(0.0f, pImpl->baselineLevel - phasicComponent * 0.8f);
    } else {
        // No prediction error: return to baseline
        pImpl->currentLevel = pImpl->baselineLevel;
    }
    
    // Natural decay back to baseline over time
    if (dt > 0.0f) {
        float decay = Impl::PHASIC_DECAY_RATE * static_cast<float>(dt);
        pImpl->currentLevel = pImpl->currentLevel * (1.0f - decay);
        pImpl->baselineLevel = pImpl->baselineLevel * (1.0f - Impl::TONIC_DECAY_RATE * static_cast<float>(dt));
    }
    
    // Record in history
    pImpl->history.push_back(pImpl->currentLevel);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update eligibility trace based on prediction error
    if (std::abs(rewardPredictionError) > 0.001f) {
        pImpl->eligibilityTrace = rewardPredictionError * 0.1f;  // Eligibility proportional to PE
    } else {
        pImpl->eligibilityTrace = 0.0f;
    }
    
    // Log dopamine activity
    if (std::abs(rewardPredictionError) > 0.01f) {
        NLM_LOG_INFO("Dopamine PE: " + std::to_string(rewardPredictionError) + 
                     " -> level: " + std::to_string(pImpl->currentLevel));
    }
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Dopamine::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.0001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

float Dopamine::getPlasticityModulation() const {
    // Dopamine modulates plasticity: phasic bursts strengthen synapses, dips weaken them
    if (pImpl->predictionError > 0.0f) {
        // Positive prediction error: potentiation (strengthening)
        return std::min(1.0f, pImpl->currentLevel / pImpl->baselineLevel);
    } else if (pImpl->predictionError < 0.0f) {
        // Negative prediction error: depression (weakening)
        return std::max(-1.0f, pImpl->currentLevel / pImpl->baselineLevel - 1.0f);
    }
    return 0.0f;  // No prediction error: no modulation
}

void Dopamine::reset() {
    pImpl->baselineLevel = Impl::DEFAULT_BASELINE;
    pImpl->currentLevel = Impl::DEFAULT_BASELINE;
    pImpl->predictionError = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->history.clear();
    NLM_LOG_INFO("Dopamine system reset");
}

} // namespace nlm
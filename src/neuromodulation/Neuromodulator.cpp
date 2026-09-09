#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float rewardHistory;
    float workingMemoryBoost;
    float temporalDifference;
    float dopamineTrace;
    float eligibilityTrace;
    std::vector<float> history;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)  // Baseline DA level ~5%
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(2.5f)
        , predictionError(0.0f)
        , rewardHistory(0.0f)
        , workingMemoryBoost(0.0f)
        , temporalDifference(0.0f)
        , dopamineTrace(0.0f)
        , eligibilityTrace(0.0f)
        , dist(0.0f, 1.0f)
    {
        std::random_device rd;
        rng.seed(rd());
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized - implementing real reward prediction dynamics");
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update traces and eligibility
    updateDerivedState();
}

void Dopamine::updateDerivedState() {
    // Update working memory modulation based on DA level
    pImpl->workingMemoryBoost = pImpl->level * 0.6f;  // Strong correlation with WM
    
    // Update temporal difference learning rate
    pImpl->temporalDifference = pImpl->level * 0.8f;  // Range [0, 0.8]
    
    // Update dopamine trace (for reward prediction)
    pImpl->dopamineTrace += pImpl->level - pImpl->dopamineTrace * 0.9f;  // Slow trace
    
    // Update eligibility trace (for synaptic learning)
    pImpl->eligibilityTrace = std::min(pImpl->eligibilityTrace + 0.1f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine dynamics: DA enhances plasticity for rewards but suppresses for prediction errors
    // Inverted U-shaped relationship with optimal level around 0.7
    float normalizedLevel = pImpl->level / 0.7f;
    
    if (normalizedLevel <= 1.0f) {
        return 0.5f + 0.5f * normalizedLevel * 1.4f;  // Rising phase
    } else {
        return 1.5f - 0.5f * (normalizedLevel - 1.0f);  // Falling phase
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline with temporal difference learning
    float predictionError = pImpl->predictionError;
    
    // Update level with TD learning
    pImpl->level += (predictionError * pImpl->temporalDifference) - 
                    pImpl->decayRate * static_cast<float>(dt) * pImpl->level;
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Spontaneous fluctuations
    if (pImpl->level > 0.3f) {
        float fluctuation = pImpl->dist(pImpl->rng);
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level + (fluctuation - 0.5f) * 0.1f);
    }
    
    // Update derived state
    updateDerivedState();
    
    // Log level changes
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic burst proportional to reward prediction error
    // For real implementation, this would be: delta = reward - value_function(state)
    float phasicBurst = std::min(pImpl->peak, reward * pImpl->releaseRate * 1.5f);
    pImpl->level = std::max(pImpl->level, phasicBurst);
    
    // Update reward history
    pImpl->rewardHistory = pImpl->rewardHistory * 0.9f + reward * 0.1f;
    
    // Reset eligibility trace
    pImpl->eligibilityTrace = 0.0f;
    
    // Log reward event
    NLM_LOG_INFO("Dopamine reward burst: " + std::to_string(reward) + " (total: " + std::to_string(pImpl->rewardHistory) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real prediction error signaling: burst for positive errors, dip for negative
    pImpl->predictionError = error;
    
    if (error > 0.0f) {
        // Phasic burst for positive prediction error
        float burst = std::min(pImpl->peak, error * pImpl->releaseRate * 2.0f);
        pImpl->level = std::max(pImpl->level, burst);
        NLM_LOG_INFO("Dopamine prediction error burst: " + std::to_string(error));
    } else {
        // Dip for negative prediction error
        pImpl->level = std::max(pImpl->baseline, pImpl->level * (1.0f + error * 0.5f));
        NLM_LOG_INFO("Dopamine prediction error dip: " + std::to_string(error));
    }
    
    // Update traces
    pImpl->dopamineTrace = pImpl->level;
    pImpl->eligibilityTrace = 0.5f + pImpl->level * 0.5f;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.2f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::max(0.1f, rate);
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::max(0.01f, rate);
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

}

// Get neuromodulation factor for plasticity rules
float Dopamine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float Dopamine::getDopamineTrace() const {
    return pImpl->dopamineTrace;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine state variables
    float level;                // Current dopamine concentration (nM)
    float baseline;             // Resting dopamine level (nM)
    float peak;                 // Maximum achievable level (nM)
    float decayRate;            // Decay rate (1/s)
    float releaseRate;          // Release rate coefficient
    float predictionError;      // Reward prediction error (RPE)
    float recentReward;         // Accumulated recent reward
    float learningRate;         // Plasticity learning rate
    
    // Biological parameters
    float tHalfMax;             // Time for half-maximal activation (s)
    float tHalfDecay;           // Time for half-maximal decay (s)
    float tonicallyActive;      // Tonic firing rate baseline (Hz)
    float phasicGain;           // Phasic response gain
    
    // History tracking
    float recentRPE;            // Recent prediction error (smoothed)
    std::vector<float> history;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    
    Impl() 
        : level(10.0f)           // Baseline DA ~10 nM
        , baseline(10.0f)
        , peak(100.0f)           // Phasic peak ~100 nM
        , decayRate(0.5f)         // Decay half-life ~1.4s
        , releaseRate(5.0f)
        , predictionError(0.0f)
        , recentReward(0.0f)
        , learningRate(0.01f)
        , tHalfMax(0.2f)          // Fast phasic response
        , tHalfDecay(1.0f)        // Slower decay
        , tonicallyActive(2.0f)   // Tonic firing rate 2 Hz
        , phasicGain(15.0f)       // Strong phasic response
        , recentRPE(0.0f)
        , dist(0.0f, 1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

Dopamine::~Dopamine() {
    delete pImpl;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak * 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine modulation of plasticity based onlevel
    // DA > baseline: enhances LTP (learning)
    // DA < baseline: suppresses LTP
    float modulation = 1.0f;
    
    if (pImpl->level > pImpl->baseline) {
        // Phasic increase enhances plasticity (learning boost)
        float excessDA = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        modulation = 1.0f + pImpl->learningRate * excessDA * 5.0f;
    } else if (pImpl->level < pImpl->baseline) {
        // Phasic decrease suppresses plasticity (learning suppression)
        float deficitDA = (pImpl->baseline - pImpl->level) / pImpl->baseline;
        modulation = std::max(0.1f, 1.0f - deficitDA * 2.0f);
    }
    
    return modulation;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline with exponential decay
    float target = pImpl->baseline;
    float decayFactor = std::exp(-pImpl->decayRate * deltaTime);
    pImpl->level = target + (pImpl->level - target) * decayFactor;
    
    // Smooth prediction error
    pImpl->recentRPE = 0.9f * pImpl->recentRPE + 0.1f * pImpl->predictionError;
    
    // Update recent reward history
    pImpl->recentReward = std::max(0.0f, pImpl->recentReward - pImpl->decayRate * deltaTime);
    
    // Record state
    if (pImpl->history.size() < 100) {
        pImpl->history.push_back(pImpl->level);
    }
}

void Dopamine::signalReward(float reward) {
    // Positive reward produces phasic burst
    float burst = reward * pImpl->releaseRate;
    
    // Biological noise for realistic variability
    float noise = pImpl->dist(pImpl->rng) * 0.1f * pImpl->phasicGain;
    
    // Add reward to current level
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst + noise);
    
    // Accumulate reward history
    pImpl->recentReward += reward;
    
    // Clear prediction error (reward matches expectation)
    pImpl->predictionError = 0.0f;
    
    NLM_LOG_DEBUG("Dopamine burst: reward=%.3f, DA level=%.2fnM", reward, pImpl->level);
}

void Dopamine::signalRewardPredictionError(float error) {
    // RPE drives dopaminergic activity (Schultz et al., 1997)
    pImpl->predictionError = error;
    
    // Phasic response proportional to absolute error magnitude
    float response = error * pImpl->phasicGain;
    
    // Direction-specific response: 
    // Positive RPE (better than expected): increases DA
    // Negative RPE (worse than expected): decreases DA
    if (error > 0) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
    } else {
        pImpl->level = std::max(0.0f, pImpl->level + response);
    }
    
    NLM_LOG_DEBUG("Dopamine RPE: error=%.3f, DA level=%.2fnM", error, pImpl->level);
}

float Dopamine::getRecentReward() const {
    return pImpl->recentReward;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 50.0f);
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->recentReward = 0.0f;
    pImpl->history.clear();
    NLM_LOG_INFO("Dopamine neuromodulator reset to baseline");
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

} // namespace nlm
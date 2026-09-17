// Dopamine: Reward and reinforcement learning signal
// Enhanced implementation with proper dopamine dynamics

#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    std::vector<float> history;
    Timestamp lastUpdate;
    float rewardPredictionError;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             lastUpdate(0.0), rewardPredictionError(0.0f) {
        history.reserve(100);
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine system initialized (Level 4 neuromodulation)");
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
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    NLM_LOG_INFO(std::string("Dopamine level set to: ") + std::to_string(pImpl->level));
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

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Higher dopamine increases plasticity, especially for reward prediction errors
    float errorFactor = 1.0f + std::abs(pImpl->rewardPredictionError) * 0.5f;
    float levelFactor = 0.5f + 0.5f * pImpl->level;
    return std::min(2.0f, errorFactor * levelFactor);
}

void Dopamine::update(TimestepDuration dt) {
    if (dt <= 0.0f) return;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store for statistics
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    pImpl->lastUpdate += dt;
}

void Dopamine::signalReward(float reward) {
    // Handle reward signals with burst response
    if (reward > 0.0f) {
        // Positive reward produces dopamine burst
        float burst = std::min(pImpl->peak, reward * pImpl->releaseRate * 2.0f);
        pImpl->level = std::max(pImpl->level, burst);
        
        // Update reward prediction error based on unexpected reward
        pImpl->rewardPredictionError = burst - pImpl->baseline * 0.1f;
        
        NLM_LOG_INFO(std::string("Dopamine reward burst: ") + std::to_string(burst) + 
                    " (RPE: " + std::to_string(pImpl->rewardPredictionError) + ")");
    } else {
        // Negative reward (punishment) causes dip
        float dip = std::max(0.0f, pImpl->level - std::abs(reward) * pImpl->releaseRate * 0.5f);
        pImpl->level = dip;
        
        // Negative reward prediction error
        pImpl->rewardPredictionError = -std::abs(reward);
        
        NLM_LOG_INFO(std::string("Dopamine dip due to punishment: ") + std::to_string(pImpl->level) +
                    " (RPE: " + std::to_string(pImpl->rewardPredictionError) + ")");
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct RPE signaling (like in temporal difference learning)
    pImpl->rewardPredictionError = error;
    
    // Apply RPE to dopamine level
    if (error > 0.0f) {
        // Positive prediction error -> dopamine increase
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate);
        NLM_LOG_INFO(std::string("Dopamine increased due to positive RPE: ") + std::to_string(error));
    } else {
        // Negative prediction error -> dopamine decrease
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate);
        NLM_LOG_INFO(std::string("Dopamine decreased due to negative RPE: ") + std::to_string(error));
    }
}

const std::vector<float>& Dopamine::getHistory() const {
    return pImpl->history;
}

void Dopamine::clearHistory() {
    pImpl->history.clear();
}

std::string Dopamine::getStatistics() const {
    std::ostringstream oss;
    oss << "Dopamine (DA) System Statistics:";
    oss << "  Current Level: " << getLevel();
    oss << "  Baseline: " << getBaseline();
    oss << "  Reward Prediction Error: " << pImpl->rewardPredictionError;
    oss << "  Decay Rate: " << getDecayRate();
    oss << "  Release Rate: " << getReleaseRate();
    oss << "  Max History Level: " << getMaxLevel();
    oss << "  Min History Level: " << getMinLevel();
    oss << "  Average (50 samples): " << getAverageLevel(50);
    oss << "  History Size: " << pImpl->history.size();
    oss << "  Is Active: " << (isActive() ? "Yes" : "No");
    return oss.str();
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->rewardPredictionError = 0.0f;
    clearHistory();
    pImpl->lastUpdate = 0.0;
    NLM_LOG_INFO("Dopamine system reset to baseline");
}

} // namespace nlm
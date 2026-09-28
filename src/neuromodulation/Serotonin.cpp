#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float actionSelectionBias;
    float patienceModulator;
    float socialLearningGain;
    float explorationModulator;
    float rewardPredictionError;
    
    Impl() : level(0.0f), baseline(0.3f), peak(1.0f), decayRate(0.06f), releaseRate(2.0f),
             actionSelectionBias(1.5f), patienceModulator(1.3f), socialLearningGain(1.6f), 
             explorationModulator(1.4f), rewardPredictionError(1.2f) {}
    
    // Checkpoint support
    std::vector<uint8_t> checkpointData() const {
        std::vector<uint8_t> data(sizeof(*this));
        std::memcpy(data.data(), this, sizeof(*this));
        return data;
    }
    
    bool restoreFromCheckpoint(const std::vector<uint8_t>& data) {
        if (data.size() != sizeof(*this)) return false;
        std::memcpy(this, data.data(), sizeof(*this));
        return true;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for mood and behavior
    return 0.7f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalActionSelection(float actionValue) {
    // Serotonin biases action selection
    pImpl->level = std::min(pImpl->peak, pImpl->level + actionValue * pImpl->releaseRate * pImpl->actionSelectionBias);
}

void Serotonin::signalPatience(float waitingTime) {
    // Serotonin modulates patience and waiting behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + waitingTime * pImpl->releaseRate * pImpl->patienceModulator);
}

void Serotonin::signalSocialLearning(float socialReward) {
    // Serotonin integrates with social learning and imitation
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialReward * pImpl->releaseRate * pImpl->socialLearningGain);
}

void Serotonin::signalExplorationExploitation(float explorationBias) {
    // Serotonin modulates exploration vs exploitation balance
    if (explorationBias > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (explorationBias - 0.5f) * pImpl->releaseRate * pImpl->explorationModulator);
    }
}

void Serotonin::signalRewardPredictionError(float error) {
    // Serotonin connects to reward prediction errors
    pImpl->rewardPredictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.8f);
}

void Serotonin::modulateActionSelection(float& actionValue, float baseValue) const {
    // Serotonin biases action selection based on mood/state
    float bias = 1.0f + pImpl->level * pImpl->actionSelectionBias;
    actionValue = baseValue * bias;
}

void Serotonin::modulateExploration(float& explorationRate, float baseRate) const {
    // Serotonin modulates exploration behavior
    float modulation = 0.5f + pImpl->level * 0.5f;
    explorationRate = baseRate * modulation;
}

bool Serotonin::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Serotonin::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Serotonin::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->level * 800);
}

float Serotonin::getAverageLevel() const {
    return pImpl->baseline;
}

float Serotonin::getMinLevel() const {
    return 0.0f;
}

float Serotonin::getMaxLevel() const {
    return pImpl->peak;
}

} // namespace nlm
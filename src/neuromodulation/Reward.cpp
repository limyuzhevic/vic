#include "Reward.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float accumulatedReward;
    float predictionErrorGain;
    float rewardValue;
    float plasticityModulator;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.5f),
             accumulatedReward(0.0f), predictionErrorGain(0.5f), rewardValue(1.0f), plasticityModulator(1.1f) {}
    
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

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

void Reward::update(TimestepDuration dt) {
    // Reward value decays towards baseline
    pImpl->currentValue = std::max(pImpl->baseline, pImpl->currentValue - pImpl->decayRate * static_cast<float>(dt));
}

float Reward::getPlasticityFactor() const {
    // Reward enhances plasticity for reinforcement learning
    return 0.6f + 0.5f * pImpl->currentValue;
}

float Reward::computeReward(const Observation& observation) const {
    // TODO PHASE 2: Implement real reward computation from observation
    // PLACEHOLDER: Returns 0
    return 0.0f;
}

void Reward::signalReward(float reward) {
    // Reward system responds to rewards
    pImpl->currentValue = std::min(pImpl->peak, pImpl->currentValue + reward * pImpl->releaseRate * pImpl->rewardValue);
    pImpl->accumulatedReward += reward;
    pImpl->history.push_back(reward);
}

void Reward::signalRewardPredictionError(float error) {
    // Reward system responds to prediction errors
    pImpl->currentValue = std::max(pImpl->baseline, pImpl->currentValue + error * pImpl->releaseRate * pImpl->predictionErrorGain);
    pImpl->history.push_back(error);
}

void Reward::signalSensoryInput(const SensoryInput& input) {
    // Reward system processes sensory input for reward prediction
    // This would integrate with prediction system
}

void Reward::signalActionOutcome(float outcomeValue) {
    // Reward system processes action outcomes
    pImpl->currentValue = std::min(pImpl->peak, pImpl->currentValue + outcomeValue * pImpl->releaseRate);
    pImpl->history.push_back(outcomeValue);
}

float Reward::getLevel() const {
    return pImpl->currentValue;
}

void Reward::setLevel(float level) {
    pImpl->currentValue = std::clamp(level, 0.0f, 1.0f);
}

bool Reward::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Reward::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Reward::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->currentValue * 850);
}

float Reward::getAverageLevel() const {
    return pImpl->baseline;
}

float Reward::getMinLevel() const {
    return 0.0f;
}

float Reward::getMaxLevel() const {
    return pImpl->peak;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

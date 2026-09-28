#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    std::vector<float> history;
    float magnitude;
    float plasticityModulator;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.5f),
             magnitude(0.0f), plasticityModulator(1.2f) {}
    
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

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->magnitude = std::abs(pImpl->error);
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error decays towards zero
    pImpl->error = pImpl->error * (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->magnitude = std::abs(pImpl->error);
}

float PredictionError::getMagnitude() const {
    return pImpl->magnitude;
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error modulates plasticity for learning
    return 0.8f + 0.3f * pImpl->magnitude;
}

bool PredictionError::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool PredictionError::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t PredictionError::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->magnitude * 800);
}

float PredictionError::getAverageLevel() const {
    return 0.0f; // Placeholder
}

float PredictionError::getMinLevel() const {
    return 0.0f;
}

float PredictionError::getMaxLevel() const {
    return 1.0f; // Placeholder
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

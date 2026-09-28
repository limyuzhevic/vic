#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float baseline;
    float peak;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    float noveltySensitivity;
    float plasticityModulator;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), baseline(0.1f), peak(1.0f), 
             noveltyThreshold(0.3f), noveltySensitivity(1.0f), plasticityModulator(1.1f) {}
    
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

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

float Novelty::getPlasticityFactor() const {
    // Novelty enhances plasticity for learning new information
    return 0.6f + 0.6f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations and compare
    // Simple implementation: just set to a placeholder
    pImpl->level = 1.0f;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(pImpl->peak, avgDiff / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::signalNewExperience(float noveltySignal) {
    // Novelty detection from new experiences
    if (noveltySignal > pImpl->noveltyThreshold) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (noveltySignal - pImpl->noveltyThreshold) * 1.5f);
    }
}

void Novelty::signalPatternChange(float changeMagnitude) {
    // Novelty response to pattern changes
    pImpl->level = std::min(pImpl->peak, pImpl->level + changeMagnitude * 1.2f);
}

void Novelty::signalPredictionMismatch(float predictionError) {
    // Novelty processing of prediction errors
    if (predictionError > 0.1f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 0.5f);
    }
}

void Novelty::signalSleepConsolidation(float consolidationEfficiency) {
    // Novelty modulates during sleep consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - (1.0f - consolidationEfficiency) * pImpl->decayRate);
}

bool Novelty::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Novelty::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Novelty::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->level * 700);
}

float Novelty::getAverageLevel() const {
    return pImpl->baseline;
}

float Novelty::getMinLevel() const {
    return 0.0f;
}

float Novelty::getMaxLevel() const {
    return pImpl->peak;
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

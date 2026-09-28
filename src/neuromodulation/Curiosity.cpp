#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float noveltyWeight;
    float predictionErrorWeight;
    float noveltySensitivity;
    float predictionErrorGain;
    float explorationValue;
    float plasticityModulator;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.5f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , noveltySensitivity(1.2f)
        , predictionErrorGain(0.8f)
        , explorationValue(1.2f)
        , plasticityModulator(1.1f) {}
    
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

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances exploration and plasticity for learning
    return 0.7f + 0.5f * pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    // Simple decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity integrates novelty and prediction error to drive exploration
    float curiosityDrive = novelty * 0.6f + predictionError * pImpl->predictionErrorGain * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + curiosityDrive * pImpl->releaseRate * pImpl->explorationValue);
    
    // Decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::signalExplorationOpportunity(float opportunity) {
    // Curiosity responds to exploration opportunities
    pImpl->level = std::min(pImpl->peak, pImpl->level + opportunity * pImpl->releaseRate * pImpl->explorationValue);
}

void Curiosity::signalNoveltyFeedback(float noveltyFeedback) {
    // Curiosity integrates novelty feedback
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyFeedback * pImpl->releaseRate * pImpl->noveltySensitivity);
}

void Curiosity::signalPredictionErrorFeedback(float predictionErrorFeedback) {
    // Curiosity integrates prediction error feedback
    pImpl->level = std::min(pImpl->peak, pImpl->level + predictionErrorFeedback * pImpl->releaseRate * pImpl->predictionErrorGain);
}

void Curiosity::signalSleepConsolidation(float consolidationEfficiency) {
    // Curiosity modulates during sleep consolidation
    if (consolidationEfficiency > 0.5f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - (1.0f - consolidationEfficiency) * pImpl->decayRate);
    }
}

bool Curiosity::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Curiosity::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Curiosity::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->level * 750);
}

float Curiosity::getAverageLevel() const {
    return pImpl->baseline;
}

float Curiosity::getMinLevel() const {
    return 0.0f;
}

float Curiosity::getMaxLevel() const {
    return pImpl->peak;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm

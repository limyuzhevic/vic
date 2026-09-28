#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionalGain;
    float workingMemoryBoost;
    float plasticityModulator;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             attentionalGain(1.5f), workingMemoryBoost(1.2f), plasticityModulator(1.1f) {}
    
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

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory encoding
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionalSelection(float salience) {
    // Boost ACh on attentional selection
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate * pImpl->attentionalGain);
}

void Acetylcholine::signalWorkingMemoryLoad(float load) {
    // ACh supports working memory maintenance
    pImpl->level = std::min(pImpl->peak, pImpl->level + load * pImpl->releaseRate * pImpl->workingMemoryBoost);
}

void Acetylcholine::signalEpisodicEncoding(float coherence) {
    // ACh promotes episodic memory encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + coherence * pImpl->releaseRate * 1.5f);
}

void Acetylcholine::signalSleepConsolidation(float sleepQuality) {
    // ACh enhances memory consolidation during rest
    if (sleepQuality > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (sleepQuality - 0.5f) * pImpl->releaseRate * 1.3f);
    }
}

void Acetylcholine::modulatePlasticity(float& ltpWeight, float& ltdWeight) const {
    // ACh modulates synaptic plasticity for learning
    float modulation = pImpl->plasticityModulator * pImpl->level;
    ltpWeight *= modulation;
    ltdWeight *= modulation;
}

bool Acetylcholine::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Acetylcholine::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Acetylcholine::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->level * 800);
}

float Acetylcholine::getAverageLevel() const {
    return pImpl->baseline;
}

float Acetylcholine::getMinLevel() const {
    return 0.0f;
}

float Acetylcholine::getMaxLevel() const {
    return pImpl->peak;
}

} // namespace nlm
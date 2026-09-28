#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float threatLevel;
    float plasticityModulator;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.08f), releaseRate(1.5f),
             arousalLevel(1.2f), vigilanceLevel(1.3f), threatLevel(1.1f), plasticityModulator(1.15f) {}
    
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

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal and learning
    return 0.9f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Boost NE on arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate * pImpl->arousalLevel);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // NE enhances vigilance and alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilance * pImpl->releaseRate * pImpl->vigilanceLevel);
}

void Norepinephrine::signalThreatResponse(float threatLevel) {
    // NE responds to threat and danger
    pImpl->level = std::min(pImpl->peak, pImpl->level + threatLevel * pImpl->releaseRate * pImpl->threatLevel);
}

void Norepinephrine::signalSleepArchitecture(float slowWaveRatio) {
    // NE modulates sleep architecture
    if (slowWaveRatio > 0.3f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - (slowWaveRatio - 0.3f) * pImpl->releaseRate * 0.5f);
    }
}

void Norepinephrine::modulateAttention(float& excitationGain, float& inhibitionGain) const {
    // NE enhances arousal and alertness
    float modulation = pImpl->level * 0.5f;
    excitationGain *= (1.0f + modulation);
    inhibitionGain *= (1.0f - modulation * 0.5f);
}

bool Norepinephrine::checkpointData(std::vector<uint8_t>& data) const {
    data = pImpl->checkpointData();
    return !data.empty();
}

bool Norepinephrine::restoreFromCheckpoint(const std::vector<uint8_t>& data) {
    return pImpl->restoreFromCheckpoint(data);
}

uint64_t Norepinephrine::getSpikeCount() const {
    return static_cast<uint64_t>(pImpl->level * 900);
}

float Norepinephrine::getAverageLevel() const {
    return pImpl->baseline;
}

float Norepinephrine::getMinLevel() const {
    return 0.0f;
}

float Norepinephrine::getMaxLevel() const {
    return pImpl->peak;
}

} // namespace nlm
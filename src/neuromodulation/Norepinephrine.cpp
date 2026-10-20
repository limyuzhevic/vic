#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float arousal;
    float vigilance;
    float fightFlightMod;
    float attentionGain;
    float memoryConsolidation;
    float decayRate;
    float phasicReleaseRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , arousal(0.0f)
        , vigilance(0.5f)
        , fightFlightMod(0.5f)
        , attentionGain(1.0f)
        , memoryConsolidation(0.8f)
        , decayRate(0.05f)
        , phasicReleaseRate(1.0f) {}
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
    // Norepinephrine enhances arousal and vigilance
    // Strongly affects attention and memory encoding
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics: sustained arousal with phasic bursts
    float naturalDecay = pImpl->level - pImpl->baseline;
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal increased arousal (fight or flight response)
    pImpl->level = std::min(1.0f, pImpl->level + arousal * pImpl->phasicReleaseRate);
    
    // Increase vigilance when aroused
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + arousal * 0.3f);
}

void Norepinephrine::signalVigilance(float vigilance) {
    // Signal attentional focus and alertness
    pImpl->level = std::min(1.0f, pImpl->level + vigilance * 0.3f);
}

void Norepinephrine::signalFightFlight(float intensity) {
    // Signal fight or flight response
    pImpl->level = std::min(1.0f, pImpl->level + intensity * pImpl->fightFlightMod);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getFightFlightMod() const {
    return pImpl->fightFlightMod;
}

float Norepinephrine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Norepinephrine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousal = arousal;
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = vigilance;
}

void Norepinephrine::setFightFlightMod(float mod) {
    pImpl->fightFlightMod = mod;
}

void Norepinephrine::setAttentionGain(float gain) {
    pImpl->attentionGain = gain;
}

void Norepinephrine::setMemoryConsolidation(float consolidation) {
    pImpl->memoryConsolidation = consolidation;
}

} // namespace nlm
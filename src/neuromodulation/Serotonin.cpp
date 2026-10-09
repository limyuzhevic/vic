#include "Serotonin.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baselineLevel;
    float receptorActivation;
    
    Impl() : level(0.0f), baselineLevel(0.0f), receptorActivation(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {
    pImpl->baselineLevel = 0.0f;
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = Neuromodulator::clampLevel(level);
}

float Serotonin::getBaselineLevel() const {
    return pImpl->baselineLevel;
}

float Serotonin::getReceptorActivation() const {
    return pImpl->receptorActivation;
}

float Serotonin::getExcitabilityModulation() const {
    return 1.0f;
}

float Serotonin::getWorkingMemoryModulation() const {
    return 1.0f;
}

float Serotonin::getEligibilityTraceModulation() const {
    return 1.0f;
}

float Serotonin::getAttentionModulation() const {
    return 1.0f;
}

float Serotonin::getLearningRateModulation() const {
    return 1.0f;
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f;
}

void Serotonin::update(TimestepDuration dt) {
    // Simple receptor activation based on level
    pImpl->receptorActivation = pImpl->level * 0.8f;
}

void Serotonin::signalEvent(float value, const std::string& eventType) {
    if (eventType == "social_reward") {
        pImpl->level = std::min(1.0f, pImpl->level + value * 0.3f);
    } else if (eventType == "mood_change") {
        pImpl->level = std::min(1.0f, pImpl->level + value * 0.2f);
    }
}

std::string Serotonin::getNeurotransmitterType() const {
    return "Serotonin (5-HT)";
}

void Serotonin::applyCrossModulation(float level, const std::string& sourceNeurotransmitter) {
    // Simple cross-modulation implementation
}

float Serotonin::getActivityDependentRelease(float neuralActivity) const {
    return neuralActivity * 0.1f;
}

void Serotonin::signalMoodChange(float moodLevel) {
    signalEvent(moodLevel, "mood_change");
}

void Serotonin::signalBehavioralInhibition(float inhibitionStrength) {
    // Effect on behavioral inhibition
}

void Serotonin::signalSocialBehavior(float socialReward) {
    signalEvent(socialReward, "social_reward");
}

} // namespace nlm

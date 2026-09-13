#include "EnhancedNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float attentionSensitivity_;
    float memoryConsolidationRate_;
    
    Impl() : attentionSensitivity_(1.0f), memoryConsolidationRate_(0.05f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return state_.tonicLevel + state_.phasicLevel;
}

void Acetylcholine::setLevel(float level) {
    state_.tonicLevel = level * 0.6f;  // Tonic is 60% of total
    state_.phasicLevel = level * 0.4f; // Phasic is 40% of total
}

float Acetylcholine::getPhasicLevel() const {
    return state_.phasicLevel;
}

float Acetylcholine::getTonicLevel() const {
    return state_.tonicLevel;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity, especially for attention-related processes
    return 1.0f + 0.5f * (state_.tonicLevel + state_.phasicLevel);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh tonic level decays slowly
    float tonicDecay = 0.01f * static_cast<float>(dt);
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - tonicDecay);
    
    // Phasic ACh decays quickly
    float phasicDecay = state_.phasicAdaptation * static_cast<float>(dt);
    state_.phasicLevel = std::max(0.0f, state_.phasicLevel - phasicDecay);
    
    // Adapt baseline
    float activityAdaptation = state_.baselineAdaptation * static_cast<float>(dt);
    state_.adaptiveBaseline = state_.adaptiveBaseline * (1.0f - activityAdaptation) +
                               (state_.tonicLevel + state_.phasicLevel) * activityAdaptation;
}

void Acetylcholine::phasicResponse(float magnitude) {
    state_.phasicLevel += magnitude * pImpl->attentionSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

const NeuromodulatorState& Acetylcholine::getState() const {
    return state_;
}

void Acetylcholine::increaseAttention(float amount) {
    state_.phasicLevel += amount * pImpl->attentionSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
    NLM_LOG_INFO("Acetylcholine increased attention by " + std::to_string(amount));
}

void Acetylcholine::decreaseAttention(float amount) {
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - amount * pImpl->attentionSensitivity_);
    NLM_LOG_INFO("Acetylcholine decreased attention by " + std::to_string(amount));
}

void Acetylcholine::triggerMemoryConsolidation() {
    state_.phasicLevel += pImpl->memoryConsolidationRate_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
    NLM_LOG_INFO("Acetylcholine triggered memory consolidation");
}

} // namespace nlm

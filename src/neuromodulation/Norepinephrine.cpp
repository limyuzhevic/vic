#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float stressSensitivity;
    float arousalLevel;
    float attentionEnhancement;
    float fightOrFlightActivation;
    float memoryConsolidation;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             stressSensitivity(0.5f), arousalLevel(0.0f), attentionEnhancement(0.0f),
             fightOrFlightActivation(0.0f), memoryConsolidation(0.0f), stressResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine neuromodulator initialized");
}

Norepinephrine::~Norepinephrine() {
    NLM_LOG_INFO("Norepinephrine neuromodulator destroyed");
    delete pImpl;
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Norepinephrine level set to " + std::to_string(pImpl->level));
}

float Norepinephrine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real norepinephrine-modulated plasticity factor
    // PLACEHOLDER: Higher norepinephrine increases arousal and attention, enhancing plasticity for salient events
    // Real: Complex interactions with locus coeruleus-prefrontal circuitry, attention gating, and stress modulation
    float factor = 0.6f + 0.4f * pImpl->level;
    factor += 0.2f * pImpl->attentionEnhancement;
    factor += 0.15f * pImpl->fightOrFlightActivation;
    return std::min(1.0f, factor);
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline with stress and arousal regulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay other modulated effects
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - pImpl->decayRate * 0.5f * static_cast<float>(dt));
    pImpl->attentionEnhancement = std::max(0.0f, pImpl->attentionEnhancement - pImpl->decayRate * 0.3f * static_cast<float>(dt));
    pImpl->fightOrFlightActivation = std::max(0.0f, pImpl->fightOrFlightActivation - pImpl->decayRate * 0.8f * static_cast<float>(dt));
    pImpl->memoryConsolidation = std::max(0.0f, pImpl->memoryConsolidation - pImpl->decayRate * 0.4f * static_cast<float>(dt));
    pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - pImpl->decayRate * 0.6f * static_cast<float>(dt));
    
    // TODO: Model phasic/tonic firing patterns of locus coeruleus neurons
    // TODO: Implement arousal threshold gating for prefrontal cortex
    // TODO: Model stress hormone feedback loops
    // TODO: Integrate with sleep-wake cycle regulation
}

void Norepinephrine::signalStress(float stress) {
    // TODO PHASE 2: Implement real stress signaling
    // PLACEHOLDER: Stress triggers fight/flight response and increases arousal
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stress * pImpl->stressSensitivity);
    pImpl->fightOrFlightActivation = std::min(1.0f, pImpl->fightOrFlightActivation + stress * 0.7f * pImpl->stressSensitivity);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + stress * 0.5f * pImpl->stressSensitivity);
    pImpl->attentionEnhancement = std::min(1.0f, pImpl->attentionEnhancement + stress * 0.3f * pImpl->stressSensitivity);
    
    // Acute stress leads to norepinephrine release
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 1.5f);
    
    NLM_LOG_INFO("Norepinephrine stress signal received: " + std::to_string(stress));
}

void Norepinephrine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Prediction error enhances arousal and attention for learning
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + error * pImpl->stressSensitivity * 0.8f);
    pImpl->attentionEnhancement = std::min(1.0f, pImpl->attentionEnhancement + error * pImpl->stressSensitivity * 0.6f);
    
    // Moderate prediction errors increase norepinephrine for enhanced learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.8f);
    
    NLM_LOG_INFO("Norepinephrine prediction error signal received: " + std::to_string(error));
}

void Norepinephrine::signalNovelty() {
    // TODO PHASE 2: Implement novelty signaling
    // PLACEHOLDER: Novelty increases arousal and attention for exploration
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + 0.4f * pImpl->stressSensitivity);
    pImpl->attentionEnhancement = std::min(1.0f, pImpl->attentionEnhancement + 0.6f * pImpl->stressSensitivity);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
    
    NLM_LOG_INFO("Norepinephrine novelty signal activated");
}

void Norepinephrine::signalMemoryConsolidation() {
    // TODO PHASE 2: Implement memory consolidation effects
    // PLACEHOLDER: Enhances memory consolidation for salient events
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + 0.5f);
    
    // Acute memory consolidation increases norepinephrine for enhanced encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.2f * pImpl->releaseRate);
    
    NLM_LOG_INFO("Norepinephrine memory consolidation signal activated");
}

} // namespace nlm

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float locusCoeruleusInput;
    float decayRate;
    float tonicLevel;
    float phasicResponse;
    
    Impl() : level(0.0f), arousal(0.0f), vigilance(0.0f), 
             locusCoeruleusInput(0.0f), decayRate(0.08f), tonicLevel(0.1f), phasicResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

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
    // Norepinephrine modulates arousal and vigilance
    // Phasic NE can enhance or impair plasticity depending on context
    if (pImpl->phasicResponse > 0.5f) {
        // High phasic response: enhances plasticity for novel events
        return 1.5f + 0.5f * pImpl->level;
    } else {
        // Tonic/NE baseline: maintains baseline plasticity
        return 0.8f + 0.2f * pImpl->level;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards tonic level
    if (pImpl->level > pImpl->tonicLevel) {
        pImpl->level = std::max(pImpl->tonicLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        // Recovery towards tonic level
        pImpl->level = std::min(pImpl->tonicLevel, pImpl->level + pImpl->decayRate * static_cast<float>(dt) * 0.5f);
    }
    
    // Calculate arousal and vigilance effects
    pImpl->arousal = pImpl->level * 0.7f + pImpl->phasicResponse * 0.3f;
    pImpl->vigilance = pImpl->level * 0.5f + (1.0f - pImpl->phasicResponse) * 0.5f;
    
    if (pImpl->level > 0.0f) {
        NLM_LOG_DEBUG("Norepinephrine: arousal=" + std::to_string(pImpl->arousal) + 
                     ", vigilance=" + std::to_string(pImpl->vigilance) + 
                     ", phasic=" + std::to_string(pImpl->phasicResponse));
    }
}

void Norepinephrine::signalArousal(float intensity, float contextRelevance) {
    // Arousal response to salient or relevant stimuli
    float response = intensity * contextRelevance * 0.5f;
    pImpl->level = std::min(1.0f, pImpl->level + response);
    pImpl->phasicResponse = response;
    
    NLM_LOG_DEBUG("Norepinephrine arousal: intensity=" + std::to_string(intensity) + 
                 ", relevance=" + std::to_string(contextRelevance) + 
                 ", level=" + std::to_string(pImpl->level));
}

void Norepinephrine::signalVigilance(float taskDemandingness, float novelty) {
    // Vigilance response to challenging or novel tasks
    float response = taskDemandingness * novelty * 0.4f;
    pImpl->level = std::min(1.0f, pImpl->level + response * 0.5f);
    pImpl->phasicResponse = std::max(pImpl->phasicResponse, response * 0.8f);
    
    NLM_LOG_DEBUG("Norepinephrine vigilance: demand=" + std::to_string(taskDemandingness) + 
                 ", novelty=" + std::to_string(novelty) + 
                 ", level=" + std::to_string(pImpl->level));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilance;
}

float Norepinephrine::getPhasicResponse() const {
    return pImpl->phasicResponse;
}

float Norepinephrine::getTotalSignal() const {
    return pImpl->level + pImpl->arousal * 0.3f + pImpl->vigilance * 0.3f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->tonicLevel;
    pImpl->arousal = pImpl->level * 0.7f;
    pImpl->vigilance = pImpl->level * 0.5f;
    pImpl->phasicResponse = 0.0f;
}

} // namespace nlm
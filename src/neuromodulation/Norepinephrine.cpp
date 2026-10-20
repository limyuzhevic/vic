#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // NE effects on neural systems
    float arousalLevel;
    float vigilance;
    float stressResponse;
    float spatialFocus;
    
    // For alertness and response modulation
    std::vector<float> arousalTrace;
    std::vector<float> vigilanceTrace;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.15f)
        , releaseRate(1.0f)
        , arousalLevel(0.0f)
        , vigilance(0.0f)
        , stressResponse(0.0f)
        , spatialFocus(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);  // NE can go above 1 for stress
    
    // Update downstream effects
    pImpl->arousalLevel = pImpl->level * 0.8f;  // Strong arousal modulation
    pImpl->vigilance = pImpl->level * 1.5f;     // NE strongly modulates vigilance
    pImpl->stressResponse = std::max(0.0f, pImpl->level - 1.0f);  // Stress response above baseline
    pImpl->spatialFocus = pImpl->level * 0.6f;  // Modulates spatial attention
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances LTP under arousal but can inhibit under high stress
    if (pImpl->level < 0.5f) {
        // Moderate NE enhances LTP
        return 1.0f + pImpl->level * 0.8f;
    } else if (pImpl->level < 1.5f) {
        // Optimal arousal enhances learning
        return 1.5f;
    } else {
        // High stress inhibits LTP (adaptive response)
        return std::max(0.3f, 1.5f - (pImpl->level - 1.5f) * 0.4f);
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay of NE signals
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update downstream effects
    setLevel(pImpl->level);
}

void Norepinephrine::signalAlert(int contextSize, float novelty) {
    // NE bursts for novel, unpredictable, or emotionally salient events
    float burst = novelty * std::min(1.0f, static_cast<float>(contextSize) / 5.0f);
    
    // Higher burst for unexpected events
    burst *= (1.0f + 0.5f * novelty);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate);
    
    // Store arousal trace
    pImpl->arousalTrace.push_back(pImpl->level);
    if (pImpl->arousalTrace.size() > 100) {
        pImpl->arousalTrace.erase(pImpl->arousalTrace.begin());
    }
    
    NLM_LOG_INFO("NE arousal: level=" + std::to_string(pImpl->level) + 
                " contextSize=" + std::to_string(contextSize) + 
                " novelty=" + std::to_string(novelty));
}

void Norepinephrine::signalStress(float stressorIntensity) {
    // Stress response: burst of NE for threatening events
    float stressBurst = stressorIntensity * 1.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressBurst * pImpl->releaseRate);
    
    // Store vigilance trace
    pImpl->vigilanceTrace.push_back(pImpl->level);
    if (pImpl->vigilanceTrace.size() > 100) {
        pImpl->vigilanceTrace.erase(pImpl->vigilanceTrace.begin());
    }
    
    NLM_LOG_INFO("NE stress response: intensity=" + std::to_string(stressorIntensity) + 
                " level=" + std::to_string(pImpl->level));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getStressResponse() const {
    return pImpl->stressResponse;
}

float Norepinephrine::getSpatialFocus() const {
    return pImpl->spatialFocus;
}

} // namespace nlm
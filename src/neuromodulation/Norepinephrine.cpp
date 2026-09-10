#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    // Arousal and vigilance components
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    // Internal dynamics
    float attentionDemand;
    float noveltyResponse;
    float adaptationIndex;
    
    // History and adaptation
    float recentArousalHistory[5]; // Last 5 arousal levels
    int historyIndex;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.05f),
             arousalLevel(0.0f), vigilanceLevel(0.5f), stressLevel(0.0f),
             attentionDemand(0.0f), noveltyResponse(0.0f), adaptationIndex(0.0f),
             historyIndex(0) {
        for (int i = 0; i < 5; ++i) recentArousalHistory[i] = 0.0f;
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and stress
    // Higher arousal increases plasticity, high stress decreases it
    float arousalEffect = pImpl->arousalLevel * 0.5f; // Arousal enhances plasticity
    float stressPenalty = pImpl->stressLevel * 0.3f;   // Stress reduces plasticity
    float vigilanceEffect = pImpl->vigilanceLevel * 0.2f; // Vigilance optimizes plasticity
    
    return std::max(0.1f, std::min(2.0f, 0.5f + arousalEffect - stressPenalty + vigilanceEffect));
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update internal state components
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - pImpl->decayRate * static_cast<float>(dt));
    pImpl->vigilanceLevel = std::max(0.5f, pImpl->vigilanceLevel - pImpl->decayRate * static_cast<float>(dt));
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->decayRate * static_cast<float>(dt) * 0.5f);
    
    // Update adaptation index
    pImpl->adaptationIndex = (pImpl->adaptationIndex + pImpl->decayRate * static_cast<float>(dt)) * 0.9f;
}

void Norepinephrine::signalAttentionDemand(float demand) {
    // Signal that attention is needed (stressful situation)
    pImpl->stressLevel = std::min(pImpl->peak, pImpl->stressLevel + demand * 0.5f);
    pImpl->vigilanceLevel = std::min(pImpl->peak, pImpl->vigilanceLevel + demand * 0.3f);
    pImpl->attentionDemand = demand;
    
    // Norepinephrine spikes in response to attention demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + demand * 0.8f);
    
    // Store in arousal history
    pImpl->recentArousalHistory[pImpl->historyIndex] = pImpl->level;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 5;
    
    NLM_LOG_INFO("Norepinephrine: Attention demand signaled with level " + std::to_string(demand));
}

void Norepinephrine::signalStress(float stressLevel) {
    // Direct stress input (threat, danger, high cognitive load)
    pImpl->stressLevel = std::min(pImpl->peak, pImpl->stressLevel + stressLevel);
    pImpl->vigilanceLevel = std::min(pImpl->peak, pImpl->vigilanceLevel + stressLevel * 0.7f);
    
    // Stress increases NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.6f);
    
    // Arousal response to stress
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + stressLevel * 0.9f);
    
    NLM_LOG_INFO("Norepinephrine: Stress level increased to " + std::to_string(pImpl->stressLevel));
}

void Norepinephrine::signalNovelty(float novelty) {
    // Response to novel stimuli (informational arousal)
    pImpl->noveltyResponse = novelty;
    pImpl->vigilanceLevel = std::min(pImpl->peak, pImpl->vigilanceLevel + novelty * 0.4f);
    
    // Novelty increases arousal and moderate NE release
    pImpl->arousalLevel = std::min(pImpl->peak, pImpl->arousalLevel + novelty * 0.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * 0.3f);
    
    NLM_LOG_INFO("Norepinephrine: Novelty detected with response " + std::to_string(novelty));
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

} // namespace nlm
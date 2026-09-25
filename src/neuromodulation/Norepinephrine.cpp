#include "../neuromodulation/Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    float tonicLevel;
    float phasicLevel;
    float arousalLevel;
    float vigilance;
    float workingMemoryEnhancement;
    float cognitiveControl;
    std::vector<float> history;
    float stressLevel;
    float stressModulation;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), decayRate(0.06f), recoveryRate(0.03f),
             tonicLevel(0.3f), phasicLevel(0.0f), arousalLevel(0.5f), vigilance(0.5f),
             workingMemoryEnhancement(1.0f), cognitiveControl(1.0f),
             stressLevel(0.5f), stressModulation(1.0f) {}
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
    // Update related modulations
    pImpl->phasicLevel = std::max(0.0f, pImpl->level - pImpl->tonicLevel);
    pImpl->arousalLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity: enhances LTP under certain conditions
    // High NE promotes memory consolidation for salient events
    float basePlasticity = 0.7f + 0.6f * pImpl->phasicLevel;
    return std::min(1.8f, std::max(0.4f, basePlasticity * pImpl->vigilance));
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recovery dynamics
    float recovery = pImpl->recoveryRate * static_cast<float>(dt);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Norepinephrine::modulateArousal(float novelty, float error, float reward) {
    // NE release is driven by novelty, unexpected error, and surprise
    float arousalDrive = 0.0f;
    
    // Novelty detection triggers phasic NE release
    arousalDrive += novelty * 0.5f;
    
    // Unexpected error (prediction error) triggers NE release
    arousalDrive += std::abs(error) * 0.6f;
    
    // Reward prediction errors also modulate NE
    arousalDrive += std::abs(reward) * 0.3f;
    
    // Apply arousal boost
    if (arousalDrive > 0.0f) {
        float boost = arousalDrive * 0.01f;
        setLevel(pImpl->level + boost);
    }
}

void Norepinephrine::increaseVigilance(float stimulusIntensity, float taskRelevance) {
    // Vigilance increases with stimulus intensity and task relevance
    float vigilanceBoost = (stimulusIntensity * 0.4f + taskRelevance * 0.6f) * 0.01f;
    setLevel(pImpl->level + vigilanceBoost);
    
    // Update vigilance factor
    pImpl->vigilance = 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::focusAttention(float concentration, float selectivity) {
    // NE modulates attentional focus and selectivity
    float attentionBoost = (concentration * 0.5f + selectivity * 0.5f) * 0.01f;
    setLevel(pImpl->level + attentionBoost);
}

void Norepinephrine::enhanceWorkingMemory(float activeMemorySize, float memoryLoad) {
    // NE enhances working memory capacity and maintenance
    float wmEnhancement = 0.0f;
    
    // Moderate arousal enhances WM performance
    wmEnhancement += pImpl->arousalLevel * 0.3f;
    
    // High memory load triggers NE release for capacity expansion
    wmEnhancement += memoryLoad * 0.4f;
    
    // Apply working memory enhancement
    if (wmEnhancement > 0.0f) {
        float boost = wmEnhancement * 0.01f;
        setLevel(pImpl->level + boost);
        pImpl->workingMemoryEnhancement = std::min(1.5f, pImpl->workingMemoryEnhancement + boost * 2.0f);
    }
}

void Norepinephrine::modulateCognitiveControl(float goalRelevance, float conflict) {
    // NE modulates cognitive control based on task demands
    float controlBoost = 0.0f;
    
    // High goal relevance increases NE release
    controlBoost += goalRelevance * 0.4f;
    
    // Conflict also triggers NE release for increased control
    controlBoost += conflict * 0.6f;
    
    // Apply cognitive control modulation
    if (controlBoost > 0.0f) {
        float boost = controlBoost * 0.01f;
        setLevel(pImpl->level + boost);
        pImpl->cognitiveControl = std::min(1.5f, pImpl->cognitiveControl + boost * 1.5f);
    }
}

void Norepinephrine::updateLocusCoeruleus(float tonic, float phasic, float adaptation) {
    // Direct LC input modulates NE system
    pImpl->tonicLevel = tonic;
    pImpl->phasicLevel = phasic;
    
    // Adaptation response
    float adaptiveChange = (tonic - pImpl->baseline) * adaptation * 0.01f;
    setLevel(pImpl->level + adaptiveChange);
}

void Norepinephrine::modulateStress(float stressLevel, float anxiety) {
    // Stress and anxiety increase NE levels
    float stressImpact = (stressLevel * 0.6f + anxiety * 0.4f) * 0.01f;
    setLevel(pImpl->level + stressImpact);
    
    // Update stress modulation factor
    pImpl->stressModulation = std::max(1.0f, 1.0f + stressLevel * 0.3f);
}

} // namespace nlm
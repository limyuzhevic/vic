#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalState;
    float vigilanceFactor;
    float noradrenergicGain;
    float locusCoeruleusActivity;
    std::vector<float> phasicHistory;
    Timestamp lastBurst;
    
    Impl() : level(0.0f), baseline(0.001f), peak(2.0f), decayRate(0.03f), releaseRate(1.2f),
             arousalState(0.5f), vigilanceFactor(1.0f), noradrenergicGain(1.8f),
             locusCoeruleusActivity(0.0f), lastBurst(0) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and vigilance modulation ready");
}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    pImpl->arousalState = std::min(1.0f, pImpl->level / 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity in a U-shaped curve (inverted U function)
    // Optimal arousal enhances learning and memory consolidation
    // Low and high NE levels impair learning (Yerkes-Dodson law)
    float arousal = pImpl->arousalState;
    float optimalArousal = 0.6f;
    float plasticity = 1.0f - 2.0f * std::abs(arousal - optimalArousal);
    return std::max(0.1f, std::min(plasticity, 2.0f));
}

void Norepinephrine::update(TimestepDuration dt) {
    // Complex dynamics with tonic and phasic components
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update locus coeruleus activity based on arousal demands
    float activityLevel = pImpl->arousalState * pImpl->noradrenergicGain;
    pImpl->locusCoeruleusActivity = activityLevel * 0.1f; // Smoothing
    
    // Maintain phasic response history
    if (pImpl->level > 0.5f) {
        pImpl->phasicHistory.push_back(pImpl->level);
        if (pImpl->phasicHistory.size() > 50) {
            pImpl->phasicHistory.erase(pImpl->phasicHistory.begin());
        }
    }
}

void Norepinephrine::signalAlert(float threatLevel, float noveltyScore) {
    // Phasic NE release in response to salient, potentially threatening events
    // Enhances sensory processing and focuses attention
    float alertLevel = std::min(1.0f, (threatLevel * 0.7f + noveltyScore * 0.3f));
    float burstAmount = alertLevel * pImpl->releaseRate * 1.5f;
    
    // Add some stochasticity to phasic responses (biological realism)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.9f, 1.1f);
    burstAmount *= dis(gen);
    
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstAmount);
    pImpl->lastBurst = pImpl->lastBurst + dt; // Should be Timestamp
    NLM_LOG_INFO("Norepinephrine phasic burst in response to salient stimulus");
}

void Norepinephrine::enhanceVigilance(float vigilanceDemand) {
    // Tonic NE modulation for sustained attention and vigilance
    // Supports sustained cognitive effort and working memory
    float vigilanceBoost = vigilanceDemand * pImpl->vigilanceFactor * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceBoost);
}

} // namespace nlm
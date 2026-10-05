#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float attention;
    float tonicLevel;
    float phasicLevel;
    float decayRate;
    float activationRate;
    float noiseLevel;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.1f)  // Baseline low arousal
        , arousal(0.1f)
        , attention(0.0f)
        , tonicLevel(0.1f)
        , phasicLevel(0.0f)
        , decayRate(0.05f)
        , activationRate(0.5f)
        , noiseLevel(0.05f)
        , dist(0.0f, 1.0f) {
        rng.seed(42);
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized - arousal and attention modulation");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    pImpl->phasicLevel = level;
}

void Norepinephrine::signalArousal(float stimulus) {
    // Phasic response to stimuli
    float burst = stimulus * activationRate;
    pImpl->phasicLevel += burst;
    pImpl->phasicLevel = std::min(2.0f, pImpl->phasicLevel);
}

void Norepinephrine::signalAttention(float attentionSignal) {
    // Attention modulation
    pImpl->phasicLevel += attentionSignal * 0.5f;
    pImpl->phasicLevel = std::min(2.0f, pImpl->phasicLevel);
}

void Norepinephrine::updateArousal(float baseline, float novelty, float predictionError) {
    // Update tonic level based on baseline, novelty, and prediction error
    float noveltyBoost = novelty * 0.5f;
    float predictionErrorBoost = std::abs(predictionError) * 0.3f;
    
    pImpl->tonicLevel = baseline + noveltyBoost + predictionErrorBoost;
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, 0.0f, 2.0f);
}

void Norepinephrine::updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance) {
    // Integrate multiple sources of attention signals
    float integratedAttention = topDownBias * 0.4f + 
                               bottomUpSalience * 0.4f + 
                               goalRelevance * 0.2f;
    
    pImpl->phasicLevel = integratedAttention;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for attended stimuli and novel events
    return 0.8f + 0.4f * pImpl->phasicLevel;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Combine tonic and phasic components with noise
    float noise = (pImpl->dist(pImpl->rng) - 0.5f) * pImpl->noiseLevel * 2.0f;
    
    // Smooth integration of tonic and phasic components
    pImpl->level = 0.7f * pImpl->tonicLevel + 0.3f * pImpl->phasicLevel + noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    
    // Decay phasic component
    pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived measures
    pImpl->arousal = pImpl->level;
    pImpl->attention = std::min(1.0f, pImpl->level / 2.0f);
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getAttention() const {
    return pImpl->attention;
}

const std::vector<float>& Norepinephrine::getHistory() const {
    return pImpl->history;
}

void Norepinephrine::addToHistory(float value) {
    pImpl->history.push_back(value);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Norepinephrine::reset() {
    pImpl->level = 0.1f;
    pImpl->arousal = 0.1f;
    pImpl->attention = 0.0f;
    pImpl->phasicLevel = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm
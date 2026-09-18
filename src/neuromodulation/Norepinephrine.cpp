#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float noveltySensitivity;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(3.0f)
        , arousalLevel(1.0f)
        , noveltySensitivity(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // NE affects arousal and excitability
    pImpl->arousalLevel = 1.0f + pImpl->level * 2.5f;
    pImpl->noveltySensitivity = 1.0f + pImpl->level * 1.5f;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for arousal-related processes and signal detection
    return 1.0f + pImpl->level * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // NE affects neural excitability and arousal
    if (pImpl->brain) {
        // Increase neural excitability - NE neurons enhance firing rates
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, pImpl->level * 0.5f);
        
        // NE enhances novelty detection and salience
        if (pImpl->brain->getNovelty()) {
            pImpl->brain->getNovelty()->setLevel(pImpl->level * 0.7f);
        }
        
        // NE modulates prediction error signaling for salient events
        if (pImpl->brain->getPredictionErrorSignal() && pImpl->level > 0.5f) {
            // Strong NE response enhances prediction error signaling
            pImpl->brain->getPredictionErrorSignal()->setLevel(pImpl->level * 0.5f);
        }
    }
}

void Norepinephrine::enhanceExcitability(float amount) {
    if (pImpl->brain) {
        // Boost neural firing rates and excitability
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, amount * 0.3f);
        pImpl->brain->injectCurrentToNeurons(NeuronType::Sensory, amount * 0.2f);
    }
}

void Norepinephrine::processNovelInformation(const Observation& observation) {
    if (pImpl->brain) {
        // NE prioritizes novel or salient information
        if (pImpl->brain->getNovelty()) {
            pImpl->brain->getNovelty()->setLevel(std::min(1.0f, pImpl->level * 1.2f));
        }
        
        // Strong NE response enhances attention to novel inputs
        if (pImpl->brain->getAttention()) {
            float attentionMod = 1.0f + pImpl->level * 1.5f;
            pImpl->brain->getAttention()->setExcitationStrength(attentionMod);
        }
    }
}

} // namespace nlm
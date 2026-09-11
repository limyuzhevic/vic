#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionGain;
    float memoryConsolidationFactor;
    float learningRateModulation;
    float baselineLevel;
    float peakLevel;
    float decayRate;
    float releaseRate;
    
    // Attention modulation
    float attentionBoost;
    float focusStrength;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionGain(1.0f)
        , memoryConsolidationFactor(1.0f)
        , learningRateModulation(1.0f)
        , baselineLevel(0.0f)
        , peakLevel(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , attentionBoost(1.5f)
        , focusStrength(1.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (attention) system initialized");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-based learning
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryConsolidationFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baselineLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // ACh affects neural excitability and attention
    if (pImpl->brain) {
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // ACh modulates neuronal excitability
                    float modulation = pImpl->level * 0.3f;
                    if (modulation > 0.0f) {
                        // Enhance firing probability
                        neuron->injectCurrent(modulation * 2.0f);
                    }
                }
            }
        }
    }
}

void Acetylcholine::enhanceAttention(float signalStrength) {
    // ACh enhances attention to salient signals
    setLevel(std::min(pImpl->peakLevel, getLevel() + signalStrength * pImpl->releaseRate * 0.5f));
    // Boost attention gain
    pImpl->attentionGain = pImpl->attentionBoost * getLevel();
    pImpl->memoryConsolidationFactor = 1.0f + 0.5f * getLevel();
    
    NLM_LOG_INFO("Acetylcholine enhanced attention by " + std::to_string(getLevel()));
}

void Acetylcholine::consolidateMemory(float memoryStrength) {
    // ACh promotes memory consolidation
    setLevel(std::min(pImpl->peakLevel, getLevel() + memoryStrength * 0.3f));
    pImpl->learningRateModulation = 1.0f + 0.8f * getLevel();
    
    NLM_LOG_INFO("Acetylcholine consolidating memories with factor " + std::to_string(pImpl->learningRateModulation));
}

void Acetylcholine::enhanceFocus(float focusSignal) {
    // ACh improves focus and working memory
    setLevel(std::min(pImpl->peakLevel, getLevel() + focusSignal * 0.2f));
    pImpl->focusStrength = 1.0f + 0.5f * getLevel();
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getFocusStrength() const {
    return pImpl->focusStrength;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baselineLevel;
    pImpl->attentionGain = 1.0f;
    pImpl->memoryConsolidationFactor = 1.0f;
    pImpl->learningRateModulation = 1.0f;
    pImpl->focusStrength = 1.0f;
}

} // namespace nlm
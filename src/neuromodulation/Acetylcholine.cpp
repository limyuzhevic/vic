#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float targetLevel;
    float attentionLevel;
    float corticalExcitability;
    float workingMemoryModulation;
    float memoryConsolidationFactor;
    float pfcModulation;
    float cholinergicTone;
    float acetylcholineReleaseRate;
    float acetylcholineDecayRate;
    
    static constexpr float DEFAULT_BASELINE = 0.05f;
    static constexpr float DEFAULT_ATTENTION = 0.5f;
    static constexpr float DEFAULT_CORTICAL_EXCITABILITY = 0.3f;
    static constexpr float DEFAULT_WORKING_MEMORY_BOOST = 0.4f;
    static constexpr float DEFAULT_CONSOLIDATION_FACTOR = 0.2f;
    static constexpr float DEFAULT_PFC_MODULATION = 0.6f;
    static constexpr float DEFAULT_CHOLINERGIC_TONE = 0.1f;
    static constexpr float DEFAULT_RELEASE_RATE = 2.0f;
    static constexpr float DEFAULT_DECAY_RATE = 0.1f;
    
    Impl()
        : level(DEFAULT_BASELINE)
        , baseline(DEFAULT_BASELINE)
        , targetLevel(DEFAULT_BASELINE)
        , attentionLevel(DEFAULT_ATTENTION)
        , corticalExcitability(DEFAULT_CORTICAL_EXCITABILITY)
        , workingMemoryModulation(DEFAULT_WORKING_MEMORY_BOOST)
        , memoryConsolidationFactor(DEFAULT_CONSOLIDATION_FACTOR)
        , pfcModulation(DEFAULT_PFC_MODULATION)
        , cholinergicTone(DEFAULT_CHOLINERGIC_TONE)
        , acetylcholineReleaseRate(DEFAULT_RELEASE_RATE)
        , acetylcholineDecayRate(DEFAULT_DECAY_RATE) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Acetylcholine system initialized");
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->targetLevel = pImpl->level;
    updateDerivedFunctions();
}

float Acetylcholine::getPlasticityFactor() const {
    float attentionBoost = pImpl->attentionLevel * 0.5f;
    float corticalBoost = pImpl->corticalExcitability * 0.3f;
    float workingMemoryBoost = pImpl->workingMemoryModulation * 0.2f;
    
    float totalBoost = attentionBoost + corticalBoost + workingMemoryBoost;
    float plasticityFactor = 1.0f + totalBoost;
    return std::min(2.0f, plasticityFactor);
}

void Acetylcholine::update(TimestepDuration dt) {
    float timeConstant = 1.0f / pImpl->acetylcholineDecayRate;
    
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - (pImpl->level - pImpl->baseline) * 
                              static_cast<float>(dt) / timeConstant);
    } else {
        pImpl->level = std::min(pImpl->targetLevel,
                              pImpl->level + (pImpl->targetLevel - pImpl->level) * 
                              static_cast<float>(dt) / (timeConstant * 2.0f));
    }
    
    pImpl->cholinergicTone = pImpl->baseline + pImpl->cholinergicTone * 0.9f;
}

float Acetylcholine::getBaselineLevel() const {
    return pImpl->baseline;
}

void Acetylcholine::setTargetLevel(float target) {
    pImpl->targetLevel = std::clamp(target, 0.0f, 1.0f);
}

bool Acetylcholine::isActive() const {
    return pImpl->level > pImpl->baseline + 0.02f;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->targetLevel = pImpl->baseline;
    updateDerivedFunctions();
}

void Acetylcholine::updateDerivedFunctions() {
    pImpl->attentionLevel = std::clamp(pImpl->level * 2.0f, 0.0f, 1.0f);
    pImpl->corticalExcitability = std::min(1.0f, pImpl->level * 3.0f);
    pImpl->workingMemoryModulation = std::min(1.0f, pImpl->level * 2.5f);
    pImpl->memoryConsolidationFactor = pImpl->level * 0.5f;
    pImpl->pfcModulation = std::min(1.0f, pImpl->level * 2.0f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::setAttentionLevel(float level) {
    pImpl->attentionLevel = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::enhanceWorkingMemory(float boostFactor) {
    pImpl->workingMemoryModulation = std::clamp(
        pImpl->workingMemoryModulation + boostFactor * 0.1f, 0.0f, 1.0f);
}

float Acetylcholine::getWorkingMemoryModulation() const {
    return pImpl->workingMemoryModulation;
}

float Acetylcholine::getCorticalExcitability() const {
    return pImpl->corticalExcitability;
}

void Acetylcholine::setCorticalExcitability(float factor) {
    pImpl->corticalExcitability = std::clamp(factor, 0.0f, 1.0f);
}

void Acetylcholine::consolidateMemory(float consolidationStrength) {
    pImpl->memoryConsolidationFactor = std::clamp(
        pImpl->memoryConsolidationFactor + consolidationStrength * 0.05f, 0.0f, 1.0f);
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidationFactor;
}

float Acetylcholine::getPFCModulation() const {
    return pImpl->pfcModulation;
}

void Acetylcholine::setPFCModulation(float modulation) {
    pImpl->pfcModulation = std::clamp(modulation, 0.0f, 1.0f);
}

} // namespace nlm

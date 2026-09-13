#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "Novelty.hpp"
#include "Curiosity.hpp"
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryConsolidationFactor;
    float attentionalModulation;
    float conceptModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             memoryConsolidationFactor(0.5f), attentionalModulation(0.5f), conceptModulation(0.5f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::enhanceWorkingMemory(NeuralWorkingMemory* workingMemory, float factor) {
    if (!workingMemory) return;
    
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.5f) {
        workingMemory->setDecayRate(std::max(0.0f, workingMemory->getDecayRate() - dopamineLevel * factor * 0.1f));
    }
}

void Dopamine::enhanceEpisodicMemory(NeuralEpisodicMemory* episodicMemory, float consolidation) {
    if (!episodicMemory) return;
    
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.5f) {
        episodicMemory->setConsolidationRate(std::min(1.0f, episodicMemory->getConsolidationRate() + dopamineLevel * consolidation * 0.2f));
    }
}

void Dopamine::modulateAttention(AttentionalSelection* attention, float bias) {
    if (!attention) return;
    
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.5f) {
        float attentionalModulation = dopamineLevel * bias;
        attention->setInhibitionStrength(0.5f - attentionalModulation * 0.3f);
        attention->setExcitationStrength(1.5f + attentionalModulation * 0.5f);
    }
}

void Dopamine::modulateConceptFormation(ConceptFormation* conceptFormation, float threshold) {
    if (!conceptFormation) return;
    
    float dopamineLevel = getLevel();
    if (dopamineLevel > 0.5f) {
        conceptFormation->setLearningThreshold(dopamineLevel * threshold);
    }
}

float Dopamine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidationFactor;
}

float Dopamine::getAttentionalModulation() const {
    return pImpl->attentionalModulation;
}

float Dopamine::getConceptModulation() const {
    return pImpl->conceptModulation;
}

void Dopamine::interactWithRewardPrediction(float predictionError, float expectedValue) {
    // Dopamine interacts with prediction error for reward learning
    float rewardValue = std::max(0.0f, predictionError - (expectedValue - 0.5f));
    setLevel(std::min(1.0f, getLevel() + rewardValue * 0.1f));
}

void Dopamine::integrateWithCuriosity(float curiosityLevel) {
    // Dopamine and curiosity have synergistic effects on learning
    if (curiosityLevel > 0.5f && getLevel() > 0.5f) {
        pImpl->memoryConsolidationFactor = std::min(1.0f, pImpl->memoryConsolidationFactor + curiosityLevel * 0.1f);
    }
}

void Dopamine::integrateWithNovelty(float noveltyLevel) {
    // Dopamine and novelty have complementary effects on exploration
    if (noveltyLevel > 0.5f) {
        pImpl->conceptModulation = std::min(1.0f, pImpl->conceptModulation + noveltyLevel * 0.1f);
    }
}

} // namespace nlm

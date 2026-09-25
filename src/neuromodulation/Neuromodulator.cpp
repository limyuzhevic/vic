#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
    // Combined state variables
    float combinedArousal;
    float combinedAttention;
    float combinedMemoryConsolidation;
    
    Impl() : brain(nullptr), combinedArousal(1.0f), combinedAttention(1.0f),
             combinedMemoryConsolidation(1.0f) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Create neuromodulators
    dopamine = std::make_unique<Dopamine>();
    acetylcholine = std::make_unique<Acetylcholine>();
    norepinephrine = std::make_unique<Norepinephrine>();
    serotonin = std::make_unique<Serotonin>();
    
    NLM_LOG_INFO("NeuromodulatorManager initialized with all neuromodulators");
}

void NeuromodulatorManager::updateAll(TimestepDuration dt, float reward,
                                      float novelty, float predictionError) {
    if (!pImpl->brain) return;
    
    // Update each neuromodulator
    dopamine->update(dt, 0.0f, reward, novelty, predictionError);
    acetylcholine->update(dt, 0.0f, reward, novelty, predictionError);
    norepinephrine->update(dt, 0.0f, reward, novelty, predictionError);
    serotonin->update(dt, 0.0f, reward, novelty, predictionError);
    
    // Calculate combined effects
    calculateCombinedEffects();
}

void NeuromodulatorManager::calculateCombinedEffects() {
    // Weighted combination of neuromodulator effects
    float totalArousal = norepinephrine->getArousalLevel() * 0.4f +
                        dopamine->getLevel() * 0.3f +
                        acetylcholine->getLevel() * 0.2f +
                        serotonin->getLevel() * 0.1f;
    
    float totalAttention = acetylcholine->getAttentionModulation() * 0.5f +
                          dopamine->getLevel() * 0.3f +
                          norepinephrine->getArousalLevel() * 0.2f;
    
    float totalMemoryConsolidation = acetylcholine->getMemoryConsolidationFactor() * 0.6f +
                                     dopamine->getLevel() * 0.3f +
                                     serotonin->getLevel() * 0.1f;
    
    // Apply reasonable bounds
    pImpl->combinedArousal = std::clamp(totalArousal, 0.1f, 2.0f);
    pImpl->combinedAttention = std::clamp(totalAttention, 0.1f, 2.0f);
    pImpl->combinedMemoryConsolidation = std::clamp(totalMemoryConsolidation, 0.1f, 2.0f);
}

void NeuromodulatorManager::applyNeuromodulatoryPlasticity(float& stdpLTP,
                                                            float& stdpLTD,
                                                            float& hebbianLearning,
                                                            float& structuralPlasticity) const {
    if (!pImpl->dopamine || !pImpl->acetylcholine || 
        !pImpl->norepinephrine || !pImpl->serotonin) return;
    
    // Apply individual neuromodulator effects
    float dopaminePlasticity = pImpl->dopamine->getPlasticityFactor();
    float achPlasticity = pImpl->acetylcholine->getPlasticityFactor();
    float nePlasticity = pImpl->norepinephrine->getPlasticityFactor();
    float serotoninPlasticity = pImpl->serotonin->getPlasticityFactor();
    
    // Weighted combination of plasticity factors
    float totalPlasticityFactor = dopaminePlasticity * 0.3f +
                                 achPlasticity * 0.3f +
                                 nePlasticity * 0.2f +
                                 serotoninPlasticity * 0.2f;
    
    // Apply to all plasticity rules
    stdpLTP *= totalPlasticityFactor;
    stdpLTD *= totalPlasticityFactor;
    hebbianLearning *= totalPlasticityFactor;
    structuralPlasticity *= totalPlasticityFactor;
}

float NeuromodulatorManager::getCombinedAttentionModulation() const {
    return pImpl->combinedAttention;
}

float NeuromodulatorManager::getCombinedMemoryConsolidation() const {
    return pImpl->combinedMemoryConsolidation;
}

float NeuromodulatorManager::getCombinedArousal() const {
    return pImpl->combinedArousal;
}

void NeuromodulatorManager::addDopamine(std::unique_ptr<Dopamine> dopamine) {
    pImpl->dopamine = std::move(dopamine);
}

void NeuromodulatorManager::addAcetylcholine(std::unique_ptr<Acetylcholine> ach) {
    pImpl->acetylcholine = std::move(ach);
}

void NeuromodulatorManager::addNorepinephrine(std::unique_ptr<Norepinephrine> ne) {
    pImpl->norepinephrine = std::move(ne);
}

void NeuromodulatorManager::addSerotonin(std::unique_ptr<Serotonin> serotonin) {
    pImpl->serotonin = std::move(serotonin);
}

Dopamine* NeuromodulatorManager::getDopamine() const {
    return pImpl->dopamine.get();
}

Acetylcholine* NeuromodulatorManager::getAcetylcholine() const {
    return pImpl->acetylcholine.get();
}

Norepinephrine* NeuromodulatorManager::getNorepinephrine() const {
    return pImpl->norepinephrine.get();
}

Serotonin* NeuromodulatorManager::getSerotonin() const {
    return pImpl->serotonin.get();
}

void NeuromodulatorManager::signalReward(float reward) {
    if (pImpl->dopamine) {
        pImpl->dopamine->signalReward(reward);
    }
}

void NeuromodulatorManager::signalNovelty(float novelty) {
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(novelty);
    }
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->setLevel(novelty);
    }
}

void NeuromodulatorManager::signalPredictionError(float error) {
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(error);
    }
    if (pImpl->serotonin) {
        pImpl->serotonin->setLevel(std::abs(error));
    }
}

} // namespace nlm

// Enhanced NeuromodulatorManager.cpp - Complete implementation for all neuromodulators

#include "NeuromodulatorManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <iostream>

namespace nlm {

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    
    // Neuromodulator references
    Dopamine* dopamine;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    Oxytocin* oxytocin;
    Vasopressin* vasopressin;
    Novelty* novelty;
    Curiosity* curiosity;
    PredictionError* predictionError;
    
    // Integration weights
    float dopamineWeight;
    float achWeight;
    float neWeight;
    float serotoninWeight;
    float oxytocinWeight;
    float vasopressinWeight;
    float noveltyWeight;
    float curiosityWeight;
    float predictionErrorWeight;
    
    Impl() : brain(nullptr), dopamine(nullptr), acetylcholine(nullptr),
             norepinephrine(nullptr), serotonin(nullptr), oxytocin(nullptr),
             vasopressin(nullptr), novelty(nullptr), curiosity(nullptr),
             predictionError(nullptr), dopamineWeight(1.0f), achWeight(0.8f),
             neWeight(0.7f), serotoninWeight(0.6f), oxytocinWeight(0.5f),
             vasopressinWeight(0.4f), noveltyWeight(0.3f), curiosityWeight(0.4f),
             predictionErrorWeight(0.6f) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~ NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(Brain* brain) {
    pImpl->brain = brain;
}

NeuromodulatorManager& NeuromodulatorManager::getInstance() {
    static NeuromodulatorManager instance;
    return instance;
}

void NeuromodulatorManager::reset() {
    // Reset all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->acetylcholine) pImpl->acetylcholine->reset();
    if (pImpl->norepinephrine) pImpl->norepinephrine->reset();
    if (pImpl->serotonin) pImpl->serotonin->reset();
    if (pImpl->oxytocin) pImpl->oxytocin->reset();
    if (pImpl->vasopressin) pImpl->vasopressin->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
}

void NeuromodulatorManager::updateAll(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->update(dt);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(dt);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(dt);
    if (pImpl->serotonin) pImpl->serotonin->update(dt);
    if (pImpl->oxytocin) pImpl->oxytocin->update(dt);
    if (pImpl->vasopressin) pImpl->vasopressin->update(dt);
    if (pImpl->novelty) pImpl->novelty->update(dt);
    if (pImpl->curiosity) pImpl->curiosity->update(dt);
    if (pImpl->predictionError) pImpl->predictionError->update(dt);
    
    // Apply neuromodulation effects
    applyNeuromodulationToNeurons();
    
    // Update integrated neuromodulation signals
    float totalPlasticity = getTotalPlasticityModulation();
    float totalExcitability = getTotalExcitabilityModulation();
    float totalAttention = getTotalAttentionModulation();
    
    NLM_LOG_DEBUG("NeuromodulatorManager updated: plasticity=" + std::to_string(totalPlasticity) +
                  ", excitability=" + std::to_string(totalExcitability) +
                  ", attention=" + std::to_string(totalAttention));
}

void NeuromodulatorManager::registerDopamine(Dopamine* dopamine) {
    pImpl->dopamine = dopamine;
}

void NeuromodulatorManager::registerAcetylcholine(Acetylcholine* acetylcholine) {
    pImpl->acetylcholine = acetylcholine;
}

void NeuromodulatorManager::registerNorepinephrine(Norepinephrine* norepinephrine) {
    pImpl->norepinephrine = norepinephrine;
}

void NeuromodulatorManager::registerSerotonin(Serotonin* serotonin) {
    pImpl->serotonin = serotonin;
}

void NeuromodulatorManager::registerOxytocin(Oxytocin* oxytocin) {
    pImpl->oxytocin = oxytocin;
}

void NeuromodulatorManager::registerVasopressin(Vasopressin* vasopressin) {
    pImpl->vasopressin = vasopressin;
}

void NeuromodulatorManager::registerNovelty(Novelty* novelty) {
    pImpl->novelty = novelty;
}

void NeuromodulatorManager::registerCuriosity(Curiosity* curiosity) {
    pImpl->curiosity = curiosity;
}

void NeuromodulatorManager::registerPredictionError(PredictionError* predictionError) {
    pImpl->predictionError = predictionError;
}

float NeuromodulatorManager::getTotalPlasticityModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total *= pImpl->dopamine->getPlasticityFactor();
    if (pImpl->acetylcholine) total *= pImpl->acetylcholine->getPlasticityFactor();
    if (pImpl->norepinephrine) total *= pImpl->norepinephrine->getPlasticityFactor();
    if (pImpl->serotonin) total *= pImpl->serotonin->getPlasticityFactor();
    if (pImpl->oxytocin) total *= pImpl->oxytocin->getPlasticityFactor();
    if (pImpl->vasopressin) total *= pImpl->vasopressin->getPlasticityFactor();
    if (pImpl->curiosity) total *= pImpl->curiosity->getPlasticityFactor();
    if (pImpl->predictionError) total *= pImpl->predictionError->getPlasticityFactor();
    
    return total;
}

float NeuromodulatorManager::getTotalExcitabilityModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total += pImpl->dopamine->getLevel() * pImpl->dopamineWeight;
    if (pImpl->acetylcholine) total += pImpl->acetylcholine->getLevel() * pImpl->achWeight;
    if (pImpl->norepinephrine) total += pImpl->norepinephrine->getLevel() * pImpl->neWeight;
    if (pImpl->serotonin) total += pImpl->serotonin->getLevel() * pImpl->serotoninWeight;
    if (pImpl->oxytocin) total += pImpl->oxytocin->getLevel() * pImpl->oxytocinWeight;
    if (pImpl->vasopressin) total += pImpl->vasopressin->getLevel() * pImpl->vasopressinWeight;
    if (pImpl->novelty) total += pImpl->novelty->getLevel() * pImpl->noveltyWeight;
    if (pImpl->curiosity) total += pImpl->curiosity->getLevel() * pImpl->curiosityWeight;
    
    return std::max(0.1f, total);
}

float NeuromodulatorManager::getTotalAttentionModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total += pImpl->dopamine->getLevel() * 0.3f;
    if (pImpl->acetylcholine) total += pImpl->acetylcholine->getLevel() * 0.5f;
    if (pImpl->norepinephrine) total += pImpl->norepinephrine->getLevel() * 0.4f;
    if (pImpl->serotonin) total += pImpl->serotonin->getLevel() * 0.2f;
    if (pImpl->oxytocin) total += pImpl->oxytocin->getLevel() * 0.1f;
    if (pImpl->vasopressin) total += pImpl->vasopressin->getLevel() * 0.1f;
    
    return std::max(0.5f, total);
}

void NeuromodulatorManager::applyNeuromodulationToNeurons() {
    if (!pImpl->brain) return;
    
    for (auto& region : pImpl->brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply neuromodulatory effects to individual neurons
                if (pImpl->dopamine) {
                    float dopamineMod = pImpl->dopamine->getLevel() * 0.2f;
                    neuron->injectCurrent(dopamineMod);
                }
                if (pImpl->acetylcholine) {
                    float achMod = pImpl->acetylcholine->getLevel() * 0.3f;
                    neuron->injectCurrent(achMod);
                }
                if (pImpl->norepinephrine) {
                    float neMod = pImpl->norepinephrine->getLevel() * 0.25f;
                    neuron->injectCurrent(neMod);
                }
                if (pImpl->serotonin) {
                    float serotoninMod = pImpl->serotonin->getLevel() * 0.15f;
                    neuron->injectCurrent(serotoninMod);
                }
                if (pImpl->oxytocin) {
                    float oxytocinMod = pImpl->oxytocin->getLevel() * 0.1f;
                    neuron->injectCurrent(oxytocinMod);
                }
                if (pImpl->vasopressin) {
                    float vasopressinMod = pImpl->vasopressin->getLevel() * 0.1f;
                    neuron->injectCurrent(vasopressinMod);
                }
            }
        }
    }
}

void NeuromodulatorManager::applyNeuromodulationToMemory(WorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    // Apply neuromodulatory effects to memory systems
    if (pImpl->dopamine) {
        workingMemory->enhanceLearning(pImpl->dopamine->getLevel() * 0.4f);
    }
    if (pImpl->acetylcholine) {
        workingMemory->enhanceConsolidation(pImpl->acetylcholine->getLevel() * 0.5f);
    }
    if (pImpl->serotonin) {
        workingMemory->enhanceRetrieval(pImpl->serotonin->getLevel() * 0.3f);
    }
}

void NeuromodulatorManager::applyNeuromodulationToAttention(AttentionalSelection* attention) {
    if (!attention) return;
    
    // Apply neuromodulatory effects to attention systems
    if (pImpl->dopamine) {
        attention->enhanceFocus(pImpl->dopamine->getLevel() * 0.3f);
    }
    if (pImpl->acetylcholine) {
        attention->enhanceSelection(pImpl->acetylcholine->getLevel() * 0.4f);
    }
    if (pImpl->norepinephrine) {
        attention->enhanceVigilance(pImpl->norepinephrine->getLevel() * 0.5f);
    }
}

Dopamine* NeuromodulatorManager::getDopamine() const {
    return pImpl->dopamine;
}

Acetylcholine* NeuromodulatorManager::getAcetylcholine() const {
    return pImpl->acetylcholine;
}

Norepinephrine* NeuromodulatorManager::getNorepinephrine() const {
    return pImpl->norepinephrine;
}

Serotonin* NeuromodulatorManager::getSerotonin() const {
    return pImpl->serotonin;
}

Oxytocin* NeuromodulatorManager::getOxytocin() const {
    return pImpl->oxytocin;
}

Vasopressin* NeuromodulatorManager::getVasopressin() const {
    return pImpl->vasopressin;
}

Novelty* NeuromodulatorManager::getNovelty() const {
    return pImpl->novelty;
}

Curiosity* NeuromodulatorManager::getCuriosity() const {
    return pImpl->curiosity;
}

PredictionError* NeuromodulatorManager::getPredictionError() const {
    return pImpl->predictionError;
}

} // namespace nlm
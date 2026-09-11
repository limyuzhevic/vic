// BrainNeuromodulationIntegration.cpp - Dopamine, curiosity, novelty, prediction error integration implementation
// Contains neuromodulation system integration functionality

#include "BrainNeuromodulationIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Implementation of BrainNeuromodulationIntegration

BrainNeuromodulationIntegration::BrainNeuromodulationIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainNeuromodulationIntegration::~BrainNeuromodulationIntegration() = default;

BrainNeuromodulationIntegration::BrainNeuromodulationIntegration(BrainNeuromodulationIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainNeuromodulationIntegration& BrainNeuromodulationIntegration::operator=(BrainNeuromodulationIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainNeuromodulationIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Neuromodulation Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize neuromodulation integration - no brain core available");
        return false;
    }
    
    pImpl->dopamine = pImpl->brainCore->getDopamine();
    pImpl->curiosity = pImpl->brainCore->getCuriosity();
    pImpl->novelty = pImpl->brainCore->getNovelty();
    pImpl->predictionError = pImpl->brainCore->getPredictionErrorSignal();
    
    NLM_LOG_INFO("Brain Neuromodulation Integration initialized successfully");
    return true;
}

void BrainNeuromodulationIntegration::update(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime) {
    updateDopamine();
    updateCuriosity();
    updateNovelty();
    updatePredictionError();
    updatePlasticityModulation();
    applyNeuromodulationToNeurons();
}

Dopamine* BrainNeuromodulationIntegration::getDopamine() {
    return pImpl->dopamine;
}

Curiosity* BrainNeuromodulationIntegration::getCuriosity() {
    return pImpl->curiosity;
}

Novelty* BrainNeuromodulationIntegration::getNovelty() {
    return pImpl->novelty;
}

PredictionError* BrainNeuromodulationIntegration::getPredictionErrorSignal() {
    return pImpl->predictionError;
}

void BrainNeuromodulationIntegration::applyDopamineEffects() {
    if (pImpl->dopamine) {
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        pImpl->dopamineLevel = dopamineLevel;
        
        // Dopamine modulates excitability by injecting additional current
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopamineLevel * 0.5f;
        if (excitabilityMod > 0.0f) {
            // Apply to all neurons in the brain
            if (pImpl->brainCore) {
                for (auto& region : pImpl->brainCore->getRegions()) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }
}

void BrainNeuromodulationIntegration::applyCuriosityEffects() {
    if (pImpl->curiosity) {
        float curiosityLevel = pImpl->curiosity->getLevel();
        pImpl->curiosityLevel = curiosityLevel;
        
        // Curiosity affects exploration and memory encoding
        // Higher curiosity increases learning and exploration behavior
        if (pImpl->brainCore && pImpl->brainCore->getWorkingMemory()) {
            pImpl->brainCore->getWorkingMemory()->setExplorationBias(curiosityLevel);
        }
    }
}

void BrainNeuromodulationIntegration::applyNoveltyEffects() {
    if (pImpl->novelty) {
        float noveltyLevel = pImpl->novelty->getLevel();
        pImpl->noveltyLevel = noveltyLevel;
        
        // Novelty affects attention and memory encoding
        // Higher novelty increases encoding strength
        if (pImpl->brainCore && pImpl->brainCore->getAttention()) {
            pImpl->brainCore->getAttention()->setNoveltySensitivity(noveltyLevel);
        }
    }
}

void BrainNeuromodulationIntegration::applyPredictionErrorEffects() {
    if (pImpl->predictionError) {
        float errorLevel = pImpl->predictionError->getLevel();
        pImpl->predictionErrorLevel = errorLevel;
        
        // Prediction error affects plasticity
        if (pImpl->brainCore) {
            auto* stdp = pImpl->brainCore->getSTDP();
            if (stdp) {
                stdp->setPredictionErrorFactor(errorLevel);
            }
        }
    }
}

void BrainNeuromodulationIntegration::clear() {
    pImpl->neuromodulationHistory.clear();
    pImpl->dopamineLevel = 0.0f;
    pImpl->curiosityLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->predictionErrorLevel = 0.0f;
}

float BrainNeuromodulationIntegration::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float BrainNeuromodulationIntegration::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

float BrainNeuromodulationIntegration::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float BrainNeuromodulationIntegration::getPredictionErrorLevel() const {
    return pImpl->predictionErrorLevel;
}

void BrainNeuromodulationIntegration::Impl::Impl(BrainCore* core) : brainCore(core), 
    dopamine(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr),
    dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f), predictionErrorLevel(0.0f) {}

void BrainNeuromodulationIntegration::updateDopamine() {
    if (pImpl->dopamine) {
        pImpl->dopamine->update(0.001f); // Use default timestep
    }
}

void BrainNeuromodulationIntegration::updateCuriosity() {
    if (pImpl->curiosity) {
        pImpl->curiosity->update(0.001f); // Use default timestep
    }
}

void BrainNeuromodulationIntegration::updateNovelty() {
    if (pImpl->novelty) {
        pImpl->novelty->update(0.001f); // Use default timestep
    }
}

void BrainNeuromodulationIntegration::updatePredictionError() {
    if (pImpl->predictionError) {
        pImpl->predictionError->update(0.001f); // Use default timestep
    }
}

void BrainNeuromodulationIntegration::updatePlasticityModulation() {
    if (pImpl->dopamine) {
        // Get plasticity factor from dopamine for plasticity modulation
        // This is used by the core brain's plasticity update
    }
}

void BrainNeuromodulationIntegration::applyNeuromodulationToNeurons() {
    applyDopamineEffects();
    applyCuriosityEffects();
    applyNoveltyEffects();
    applyPredictionErrorEffects();
}

void BrainNeuromodulationIntegration::calculateNeuromodulationStats() const {}

} // namespace nlm


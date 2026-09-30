// Neuromodulation implementation for Brain.cpp
// Contains: Dopamine, Curiosity, Novelty, PredictionError systems

#pragma once

#include "Neuromodulation.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuromodulationSystem::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    config_ = config;
    brain_ = brain;
    
    // Initialize neuromodulation subsystems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    
    dopamine_->initialize(brain.get());
    curiosity_->initialize(brain.get());
    novelty_->initialize(brain.get());
    predictionError_->initialize(brain.get());
    
    // Configure neuromodulation parameters
    neuromodulationLevel_ = config->getOr<float>("neuromodulation_level", 1.0f);
    dopamineLevel_ = config->getOr<float>("dopamine_level", neuromodulationLevel_);
    curiosityLevel_ = config->getOr<float>("curiosity_level", neuromodulationLevel_ * 0.5f);
    
    NLM_LOG_INFO("NeuromodulationSystem initialized");
}

void NeuromodulationSystem::processStep(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_) return;
    
    // Update novelty detection
    if (novelty_) {
        novelty_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Update curiosity
    if (curiosity_) {
        curiosity_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Update dopamine (reward prediction error)
    if (dopamine_) {
        dopamine_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Apply dopamine effects on neural excitability
        applyDopamineEffects();
    }
    
    // Update prediction error
    if (predictionError_) {
        predictionError_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
    }
}

void NeuromodulationSystem::applyDopamineEffects() {
    if (!brain_ || !dopamine_ || !brain_->getDopamine()) return;
    
    float dopamineLevel = brain_->getDopamine()->getLevel();
    float plasticityFactor = brain_->getDopamine()->getPlasticityFactor();
    
    // Apply dopamine effects on neural excitability
    float excitabilityMod = dopamineLevel * 0.5f;
    
    for (auto& region : brain_->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                if (excitabilityMod > 0.0f) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
    
    // Apply neuromodulation to plasticity
    applyNeuromodulationToPlasticity(plasticityFactor);
}

void NeuromodulationSystem::applyNeuromodulationToPlasticity(float factor) {
    if (!brain_ || !brain_->getSTDP() || !brain_->getHebbian()) return;
    
    // Scale STDP learning rates based on neuromodulation
    float ltpWeight = 0.01f * factor;
    float ltdWeight = 0.012f * factor;
    float tau = 20.0f;
    
    brain_->getSTDP()->configure(ltpWeight, ltdWeight, tau);
}

Dopamine* NeuromodulationSystem::getDopamine() {
    return dopamine_.get();
}

Curiosity* NeuromodulationSystem::getCuriosity() {
    return curiosity_.get();
}

Novelty* NeuromodulationSystem::getNovelty() {
    return novelty_.get();
}

PredictionError* NeuromodulationSystem::getPredictionError() {
    return predictionError_.get();
}

} // namespace nlm

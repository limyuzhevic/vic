// PlasticitySystem implementation - Handles STDP, Hebbian, and structural plasticity

#include "PlasticitySystem.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>

namespace nlm {

PlasticitySystem::PlasticitySystem(std::shared_ptr<Brain> brain)
    : brain_(brain), plasticityMod_(1.0f) {
}

PlasticitySystem::~PlasticitySystem() = default;

bool PlasticitySystem::initialize() {
    // Plasticity systems are created by Brain during its initialization
    // This system just provides access and coordination
    
    return true;
}

void PlasticitySystem::step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    // Apply neuromodulation factor for plasticity
    float neuromodulationFactor = 1.0f;
    if (brain_->getDopamine()) {
        neuromodulationFactor = brain_->getDopamine()->getPlasticityFactor();
    }
    plasticityMod_ = neuromodulationFactor;
    
    // Apply STDP and Hebbian plasticity
    applySTDP(currentStep, currentTime);
    applyHebbianLearning(currentStep, currentTime);
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        applyStructuralPlasticity(currentStep);
    }
    
    // Apply neuromodulation effects on plasticity
    applyNeuromodulationEffects();
}

void PlasticitySystem::applySTDP(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_->getSTDP() || !brain_->getRegion(RegionId(1))) {
        return;
    }
    
    auto* region = brain_->getRegion(RegionId(1));
    
    for (auto& syn : region->getSynapses()) {
        // Apply STDP with neuromodulation
        if (syn->getPlasticityFlags().stdp) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                // Modify weight change based on dopamine
                float weight = syn->getWeight();
                float delta = plasticityMod_ - 1.0f;  // Scale by neuromodulation
                weight += (weight > 0 ? 1.0f : -1.0f) * delta * 0.001f;
                syn->setWeight(weight);
                
                // Apply STDP update
                brain_->getSTDP()->update(syn, preSpikes, postSpikes, timestep);
            }
        }
        
        // Apply Hebbian learning
        if (syn->getPlasticityFlags().hebbian) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                brain_->getHebbian()->update(syn, preSpikes, postSpikes, timestep);
            }
        }
        
        // Update synapse state
        syn->step(currentTime);
    }
}

void PlasticitySystem::applyHebbianLearning(SimulationStep currentStep, Timestamp currentTime) {
    if (!brain_->getHebbian() || !brain_->getRegion(RegionId(1))) {
        return;
    }
    
    auto* region = brain_->getRegion(RegionId(1));
    
    for (auto& syn : region->getSynapses()) {
        if (syn->getPlasticityFlags().hebbian) {
            const auto& preSpikes = syn->getPreSpikeHistory();
            const auto& postSpikes = syn->getPostSpikeHistory();
            
            if (!preSpikes.empty() && !postSpikes.empty()) {
                brain_->getHebbian()->update(syn, preSpikes, postSpikes, timestep);
            }
        }
    }
}

void PlasticitySystem::applyStructuralPlasticity(SimulationStep currentStep) {
    if (structuralPlasticity_ && brain_) {
        structuralPlasticity_->update(brain_.get(), *brain_->getRandomGenerator());
    }
}

void PlasticitySystem::applyNeuromodulationEffects() {
    if (brain_->getDopamine()) {
        float dopamineLevel = brain_->getDopamine()->getLevel();
        
        // Scale STDP learning rates based on dopamine
        if (stdp_) {
            float plasticityFactor = 0.5f + 0.5f * dopamineLevel;
            plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
            
            stdp_->setLTPWeight(0.01f * plasticityFactor);
            stdp_->setLTDWeight(0.012f * plasticityFactor);
        }
    }
}

} // namespace nlm

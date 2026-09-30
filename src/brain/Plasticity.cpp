// Plasticity mechanisms implementation for Brain.cpp
// Contains: STDP, Hebbian, structural plasticity

#pragma once

#include "Plasticity.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void PlasticitySystem::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    config_ = config;
    brain_ = brain;
    
    // Configure STDP parameters
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    
    if (auto* stdp = brain_->getSTDP()) {
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    // Configure structural plasticity parameters
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    if (auto* structural = brain_->getStructuralPlasticity()) {
        structural->setSynaptogenesisRate(synaptogenesisRate);
        structural->setPruningRate(pruningRate);
    }
    
    NLM_LOG_INFO("PlasticitySystem initialized");
}

void PlasticitySystem::applyPlasticity(SimulationStep currentStep, float neuromodulationFactor) {
    if (!brain_) return;
    
    for (auto& region : brain_->getRegions()) {
        for (auto& synapse : region->getSynapses()) {
            applySTDP(synapse, currentStep, neuromodulationFactor);
            applyHebbian(synapse);
            synapse->step(currentTime_);
        }
    }
}

void PlasticitySystem::applySTDP(Synapse* synapse, SimulationStep currentStep, float modulationFactor) {
    if (!synapse || !brain_ || !synapse->getPlasticityFlags().stdp) return;
    
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Calculate STDP weight change based on spike timing
    float weightChange = calculateSTDPWeightChange(preSpikes.back(), postSpikes.back());
    weightChange *= modulationFactor;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + weightChange;
    
    // Ensure weight stays within bounds
    newWeight = std::max(-1.0f, std::min(1.0f, newWeight));
    synapse->setWeight(newWeight);
}

void PlasticitySystem::applyHebbian(Synapse* synapse) {
    if (!synapse || !brain_ || !synapse->getPlasticityFlags().hebbian) return;
    
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    // Hebbian learning: strengthen co-activated synapses
    float hebbianFactor = 0.001f;
    float weightChange = hebbianFactor * (1.0f - synapse->getWeight());
    
    float newWeight = std::min(1.0f, synapse->getWeight() + weightChange);
    synapse->setWeight(newWeight);
}

void PlasticitySystem::applyStructuralPlasticity() {
    if (!brain_ || !brain_->getStructuralPlasticity()) return;
    
    brain_->getStructuralPlasticity()->update(brain_.get(), *brain_->getRandomGenerator());
}

float PlasticitySystem::calculateSTDPWeightChange(Timestamp preTime, Timestamp postTime) {
    float deltaT = postTime - preTime;
    
    if (deltaT < 0) { // Pre before post - LTP
        return std::exp(-std::abs(deltaT) / ltpTimeConstant_);
    } else { // Post before pre - LTD
        return -std::exp(-std::abs(deltaT) / ltdTimeConstant_);
    }
}

} // namespace nlm

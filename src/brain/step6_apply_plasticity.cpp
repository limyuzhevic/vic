// Step 6: Apply plasticity rules (STDP and Hebbian)
// This module applies synaptic plasticity rules including STDP and Hebbian learning
// Implements activity-dependent synaptic weight changes

#include "step6_apply_plasticity.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step6 {

void ApplySTDPWithNeuromodulation(Brain::Impl* impl,
                                   Synapse* syn,
                                   const std::vector<Timestamp>& preSpikeHistory,
                                   const std::vector<Timestamp>& postSpikeHistory,
                                   Timestamp currentTime,
                                   float plasticityMod) {
    if (!syn || !impl) {
        return;
    }
    
    // Calculate weight change based on spike history
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        // Apply STDP update with neuromodulation
        impl->stdp->update(syn, preSpikeHistory, postSpikeHistory, impl->timestep);
        
        // Modulate weight change based on dopamine
        float weight = syn->getWeight();
        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
        syn->setWeight(weight);
    }
}

void ApplyHebbianLearning(Brain::Impl* impl,
                          Synapse* syn,
                          const std::vector<Timestamp>& preSpikeHistory,
                          const std::vector<Timestamp>& postSpikeHistory,
                          Timestamp currentTime) {
    if (!syn || !impl) {
        return;
    }
    
    // Apply Hebbian learning based on spike coincidences
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        impl->hebbian->update(syn, preSpikeHistory, postSpikeHistory, impl->timestep);
    }
}

void UpdateSynapseEligibilityTrace(Synapse* syn, Timestamp currentTime) {
    if (!syn) {
        return;
    }
    
    syn->step(currentTime);
}

void ApplyPlasticity(Brain::Impl* impl,
                     SimulationStep currentStep,
                     Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ApplyPlasticity called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Applying plasticity rules (Step " + std::to_string(currentStep) + ")");
    
    OptimizedPlasticityApplication(impl, currentStep, currentTime);
}

void OptimizedPlasticityApplication(Brain::Impl* impl,
                                    SimulationStep currentStep,
                                    Timestamp currentTime) {
    if (!impl) {
        return;
    }
    
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (impl->dopamine) {
        plasticityMod = impl->dopamine->getPlasticityFactor();
    }
    
    // Apply plasticity to all synapses in all regions
    for (auto& region : impl->regions) {
        if (!region) {
            NLM_LOG_WARNING("Null region encountered during plasticity application");
            continue;
        }
        
        for (auto& syn : region->getSynapses()) {
            if (!syn) {
                NLM_LOG_WARNING("Null synapse encountered during plasticity application");
                continue;
            }
            
            PlasticityFlags flags = syn->getPlasticityFlags();
            
            // Apply STDP with neuromodulation
            if (flags.stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                ApplySTDPWithNeuromodulation(impl, syn, preSpikes, postSpikes,
                                            currentTime, plasticityMod);
            }
            
            // Apply Hebbian learning
            if (flags.hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                ApplyHebbianLearning(impl, syn, preSpikes, postSpikes, currentTime);
            }
            
            // Update synapse state
            UpdateSynapseEligibilityTrace(syn, currentTime);
        }
    }
}

} // namespace Step6

} // namespace nlm
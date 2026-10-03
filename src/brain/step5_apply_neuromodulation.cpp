// Step 5: Apply neuromodulation effects
// This module applies neuromodulatory signals to brain regions
// Implements dopamine, curiosity, novelty, and prediction error effects

#include "step5_apply_neuromodulation.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step5 {

void ApplyDopamineEffects(Brain::Impl* impl, float dopamineLevel) {
    if (!impl) {
        return;
    }
    
    if (dopamineLevel <= 0.0f) {
        return; // No effect if dopamine level is zero or negative
    }
    
    // Dopamine modulates excitability by injecting additional current
    // Higher dopamine increases excitability (lower effective threshold)
    float excitabilityMod = dopamineLevel * 0.5f;
    
    if (excitabilityMod > 0.0f) {
        for (auto& region : impl->regions) {
            if (!region) {
                NLM_LOG_WARNING("Null region encountered during dopamine application");
                continue;
            }
            
            for (auto& pop : region->getPopulations()) {
                if (!pop) {
                    NLM_LOG_WARNING("Null population encountered during dopamine application");
                    continue;
                }
                
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void ApplyNeuromodulationToNeurons(const std::vector<Neuron*>& neurons, float modulationFactor) {
    for (auto* neuron : neurons) {
        if (neuron) {
            neuron->injectCurrent(modulationFactor);
        }
    }
}

void ApplyNeuromodulation(Brain::Impl* impl,
                          SimulationStep currentStep,
                          Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ApplyNeuromodulation called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Applying neuromodulation effects (Step " + std::to_string(currentStep) + ")");
    
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(impl->timestep);
    }
    
    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(impl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl->dopamine->getLevel();
        ApplyDopamineEffects(impl, dopamineLevel);
    }
}

void UpdateNoveltyDetection(Brain::Impl* impl, TimestepDuration timestep) {
    if (impl && impl->novelty) {
        impl->novelty->update(timestep);
    }
}

void UpdateCuriosity(Brain::Impl* impl, TimestepDuration timestep) {
    if (impl && impl->curiosity) {
        impl->curiosity->update(timestep);
    }
}

} // namespace Step5

} // namespace nlm
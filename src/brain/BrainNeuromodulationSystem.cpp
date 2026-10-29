// Brain neuromodulation system implementation
// Handles neuromodulation system initialization and operations

#include "brain/BrainNeuromodulationSystem.hpp"
#include "brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/Dopamine.hpp"

namespace nlm {

void BrainNeuromodulationSystem::initializeNeuromodulationSystems(Brain* brain) {
    brain->pImpl->novelty = std::make_unique<Novelty>();
    brain->pImpl->novelty->initialize(brain);
    
    brain->pImpl->curiosity = std::make_unique<Curiosity>();
    brain->pImpl->curiosity->initialize(brain);
    
    brain->pImpl->dopamine = std::make_unique<Dopamine>();
    brain->pImpl->dopamine->initialize(brain);
    
    brain->pImpl->predictionError = std::make_unique<PredictionError>();
    brain->pImpl->predictionError->initialize(brain);
}

void BrainNeuromodulationSystem::updateNeuromodulation(Brain* brain) {
    // Update novelty detection
    if (brain->pImpl->novelty) {
        brain->pImpl->novelty->update(brain->pImpl->timestep);
    }
    
    // Update curiosity
    if (brain->pImpl->curiosity) {
        brain->pImpl->curiosity->update(brain->pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (brain->pImpl->dopamine) {
        brain->pImpl->dopamine->update(brain->pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = brain->pImpl->dopamine->getLevel();
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void BrainNeuromodulationSystem::applyPlasticityRules(Brain* brain, Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (brain->pImpl->dopamine) {
        plasticityMod = brain->pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    brain->pImpl->stdp->update(syn, preSpikes, postSpikes, brain->pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brain->pImpl->hebbian->update(syn, preSpikes, postSpikes, brain->pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

} // namespace nlm

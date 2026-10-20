#include "brain/BrainNeuromodulation.hpp"
#include "brain/Brain.hpp"

namespace nlm {

void BrainNeuromodulation::updateNoveltyDetection(Novelty* novelty,
                                                 TimestepDuration timestep) {
    if (novelty) {
        novelty->update(timestep);
    }
}

void BrainNeuromodulation::updateCuriosity(Curiosity* curiosity,
                                          TimestepDuration timestep) {
    if (curiosity) {
        curiosity->update(timestep);
    }
}

void BrainNeuromodulation::updateDopamine(Dopamine* dopamine,
                                         TimestepDuration timestep,
                                         std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    if (!dopamine) return;
    
    dopamine->update(timestep);
    
    // Apply dopamine effects on neural excitability
    // Dopamine modulates neural excitability by adjusting effective current injection
    // Higher dopamine increases excitability (lower effective threshold)
    float dopamineLevel = dopamine->getLevel();
    for (auto& region : regions) {
        if (!region) continue;
        
        for (auto& population : region->getPopulations()) {
            if (!population) continue;
            
            for (auto* neuron : population->getNeurons()) {
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

void BrainNeuromodulation::applyNeuromodulationEffects(Neuromodulator* signal,
                                                      STDP* stdpSystem,
                                                      float& plasticityMod) {
    if (!signal || !stdpSystem) return;
    
    float modulation = signal->getLevel();
    
    // Scale STDP learning rates
    stdpSystem->setLTPWeight(0.01f * modulation);
    stdpSystem->setLTDWeight(0.012f * modulation);
    
    plasticityMod = modulation;
}

void BrainNeuromodulation::calculatePlasticityModulation(Dopamine* dopamine,
                                                        float& plasticityMod) {
    plasticityMod = 1.0f;
    if (dopamine) {
        plasticityMod = dopamine->getPlasticityFactor();
    }
}

} // namespace nlm

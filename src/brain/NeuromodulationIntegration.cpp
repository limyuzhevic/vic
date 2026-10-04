#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuromodulationIntegration::initialize(Dopamine* dopaminePtr, Curiosity* curiosityPtr,
                                           Novelty* noveltyPtr, PredictionError* predictionErrorPtr,
                                           Brain* brainPtr) {
    dopamine = dopaminePtr;
    curiosity = curiosityPtr;
    novelty = noveltyPtr;
    predictionError = predictionErrorPtr;
    brain = brainPtr;
}

void NeuromodulationIntegration::update(NeuromodulationIntegration* neuromodPtr, float timestep,
                                       const std::vector<NeuralRegion*>& regions) {
    if (!neuromodPtr) return;
    
    // Step 5: Apply neuromodulation effects
    // Update novelty detection
    if (neuromodPtr->novelty) {
        neuromodPtr->novelty->update(timestep);
    }
    
    // Update curiosity
    if (neuromodPtr->curiosity) {
        neuromodPtr->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (neuromodPtr->dopamine) {
        neuromodPtr->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = neuromodPtr->dopamine->getLevel();
        for (auto& region : regions) {
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

void NeuromodulationIntegration::applyDopamineEffects(Dopamine* dopaminePtr, const std::vector<NeuralRegion*>& regions) {
    if (!dopaminePtr) return;
    
    // Apply neuromodulation effects on plasticity
    float modulation = dopaminePtr->getLevel();
    
    // Scale STDP learning rates (this will be handled by plasticity integration)
    // Dopamine modulates both LTP and LTD weights
}

float NeuromodulationIntegration::getPlasticityModulator(Dopamine* dopaminePtr) const {
    if (!dopaminePtr) return 1.0f;
    
    return dopaminePtr->getPlasticityFactor();
}

} // namespace nlm
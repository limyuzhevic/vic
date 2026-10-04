#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void PlasticityIntegration::initialize(STDP* stdpPtr, Hebbian* hebbianPtr,
                                      StructuralPlasticity* structuralPlasticityPtr,
                                      SpikeSystem* spikeSystemPtr, Brain* brainPtr) {
    stdp = stdpPtr;
    hebbian = hebbianPtr;
    structuralPlasticity = structuralPlasticityPtr;
    spikeSystem = spikeSystemPtr;
    brain = brainPtr;
}

void PlasticityIntegration::applyPlasticityRules(STDP* stdpPtr, Hebbian* hebbianPtr,
                                                StructuralPlasticity* structuralPlasticityPtr,
                                                const std::vector<NeuralRegion*>& regions, float timestep,
                                                float plasticityMod, Timestamp currentTime) {
    if (!stdpPtr || !hebbianPtr) return;
    
    // Step 6: Apply plasticity rules (STDP and Hebbian)
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine (plasticityMod)
                    stdpPtr->update(syn, preSpikes, postSpikes, timestep);
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
                    hebbianPtr->update(syn, preSpikes, postSpikes, timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void PlasticityIntegration::applyStructuralPlasticity(StructuralPlasticity* structuralPlasticityPtr, Brain* brainPtr,
                                                    RandomGenerator* rng, SimulationStep currentStep) {
    if (!structuralPlasticityPtr || !brainPtr) return;
    
    // Step 11: Apply structural plasticity periodically
    if (currentStep % 100 == 0) {  // Update every 100 steps
        structuralPlasticityPtr->update(brainPtr, *rng);
    }
}

} // namespace nlm
#include "NeuromodulationUpdater.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void NeuromodulationUpdater::update(Brain& brain, SimulationStep currentStep,
                                   Timestamp currentTime, TimestepDuration timestep) {
    // Update novelty detection
    if (brain.pImpl->novelty) {
        brain.pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (brain.pImpl->curiosity) {
        brain.pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (brain.pImpl->dopamine) {
        brain.pImpl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = brain.pImpl->dopamine->getLevel();
        for (auto& region : brain.pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

} // namespace nlm

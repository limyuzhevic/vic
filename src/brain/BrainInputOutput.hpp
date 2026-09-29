#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"

namespace nlm {

class BrainInputOutput {
public:
    // Sensory input handling
    static void receiveSensoryInput(Brain& brain, const SensoryInput& input);
    
    // Utility functions for current injection
    static void injectCurrent(Brain& brain, NeuronId neuron, MembranePotential current);
    static void injectCurrentToNeurons(Brain& brain, NeuronType type, MembranePotential current);
    
    // Motor output production
    static std::unique_ptr<Action> produceAction(Brain& brain);
    
    // Neuromodulation handling
    static void applyNeuromodulation(Brain& brain, const Neuromodulator& signal);
    
    // Plasticity and development (kept for API compatibility)
    static void updatePlasticity(Brain& brain);
    static void develop(Brain& brain);
};

} // namespace nlm
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ========== BRAIN INTEGRATION DECLARATIONS ==========

namespace BrainIntegration {
    
    void integrateSensoryInput(Brain::Impl* impl, const class SensoryInput& input);
    
    void injectCurrentToNeuron(Brain::Impl* impl, NeuronId neuron, MembranePotential current);
    
    void injectCurrentToNeurons(Brain::Impl* impl, NeuronType type, MembranePotential current);
    
    std::unique_ptr<class Action> produceAction(Brain::Impl* impl);
    
    void applyNeuromodulation(Brain::Impl* impl, const class Neuromodulator& signal);
    
    void updatePlasticity(Brain::Impl* impl);
    
    void develop(Brain::Impl* impl);
}

} // namespace nlm
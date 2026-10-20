#pragma once

#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/NeuronType.hpp"
#include "brain/SensoryInput.hpp"
#include "brain/Action.hpp"
#include <memory>

namespace nlm {

class BrainAction {
public:
    static std::unique_ptr<Action> produceAction(const std::vector<Neuron*>& motorNeurons);
    
    static void applySensoryInput(Brain* brain,
                                 const SensoryInput& input,
                                 std::vector<Neuron*>& sensoryNeurons);
    
    static void injectCurrentToNeurons(Brain* brain,
                                       NeuronId neuron,
                                       MembranePotential current);
    
    static void injectCurrentToType(Brain* brain,
                                   NeuronType type,
                                   MembranePotential current);
};

} // namespace nlm

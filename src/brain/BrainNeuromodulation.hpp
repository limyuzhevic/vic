#pragma once

#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"
#include "brain/Neuromodulator.hpp"
#include "brain/Neuromodulation/Dopamine.hpp"
#include "brain/Neuromodulation/Curiosity.hpp"
#include "brain/Neuromodulation/Novelty.hpp"
#include "brain/NeuronType.hpp"
#include "brain/PlasticityFlags.hpp"
#include "brain/STDP.hpp"
#include <vector>

namespace nlm {

class BrainNeuromodulation {
public:
    static void updateNoveltyDetection(Novelty* novelty,
                                      TimestepDuration timestep);
    
    static void updateCuriosity(Curiosity* curiosity,
                               TimestepDuration timestep);
    
    static void updateDopamine(Dopamine* dopamine,
                               TimestepDuration timestep,
                               std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    static void applyNeuromodulationEffects(Neuromodulator* signal,
                                           STDP* stdpSystem,
                                           float& plasticityMod);
    
    static void calculatePlasticityModulation(Dopamine* dopamine,
                                              float& plasticityMod);
};

} // namespace nlm

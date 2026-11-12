// BrainIO.hpp - Input/output operations
#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>

namespace nlm {

class BrainIO {
public:
    BrainIO(Brain* brain);
    ~BrainIO();
    
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
    
private:
    Brain* brain_;
};

} // namespace nlm

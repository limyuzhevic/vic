#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Brain::Impl;
class SensoryInput;

class BrainIO {
public:
    BrainIO(Brain::Impl* pImpl);
    
    void performSensoryInput(const SensoryInput& input);
    void performInjectCurrent(NeuronId neuron, MembranePotential current);
    void performInjectCurrentToNeurons(NeuronType type, MembranePotential current);
    
private:
    Brain::Impl* pImpl;
};

} // namespace nlm
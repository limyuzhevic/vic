// BrainSensory.hpp - Sensory input processing interface
#pragma once

#include "../sensory/SensoryInput.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/RegionId.hpp"
#include "../brain/NeuronType.hpp"
#include "../core/Types/MembranePotential.hpp"
#include <vector>

namespace nlm {

class Brain;

class BrainSensory {
public:
    BrainSensory();
    ~BrainSensory();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Process sensory input and inject into sensory neurons
    void receiveSensoryInput(const SensoryInput& input);
    
    // Inject current into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Collect sensory and motor neurons from brain regions
    void collectSensoryMotorNeurons(Brain* brain);
    
private:
    Brain* brain_;
    std::vector<Neuron*> sensoryNeurons_;
    std::vector<Neuron*> motorNeurons_;
};

} // namespace nlm